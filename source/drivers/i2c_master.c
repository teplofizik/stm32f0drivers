// ***********************************************************
//	i2c_master.c
//  I2C Master v2.0.0
//
//  ����������: i2c, memory, systimer
//  teplofizik, 2017
// ***********************************************************

#include "i2c_master.h"
#include "memory.h"
#include "systick.h"
#include <string.h>

typedef enum
{
    I2C_NONE,
    I2C_START,
    I2C_WRITE,
    I2C_READ,
    I2C_STOP
} TI2CStep;

typedef struct
{
    // TX handler
    I2CM_TX_Handler TX;
    I2CM_RX_Handler RX;
    
    // ����� �������� ����������
    uint8_t         Address;
    // �������
    uint8_t         Register;
    // ������������ ������
    TI2C            Phy;
    
    // ������?
    bool            InUse;
    // ������?
    bool            Read;
    // ���������?
    bool            Completed;
    // ������
    TI2CStatus      Status;
    
    // ����� � ���������� ��������� (������/������)
    uint8_t         Buffer[QUEUE_BUFFER_SIZE];
    uint8_t         Count;
    uint8_t         Index;
} TI2CQueue;

typedef struct
{
	// �������
    uint32_t      Timeout;
    // ������� ��������
    TI2CStep      Step;
    
    // ������?
    TI2CQueue   * Queue; // ������� ������
} TI2CMState;

// � ��� ������
static TI2CQueue QueueList[QUEUE_COUNT];

static TI2CMState * States[I2C_COUNT];

// =============================================
// ��������� ������� ������
// =============================================

// �������� �������
static void ResetTimeout(TI2CMState * S)
{
    S->Timeout = I2C_TIMEOUT_VALUE;
}

// =============================================

// ������ ������
static TI2CQueue * GetFreeQueue(void)
{
    int i;
    
    // ���� ��������� �����
    for(i = 0; i < QUEUE_COUNT; i++) if(QueueList[i].InUse == false) return &QueueList[i];
    
    return 0;
}

// �������� � ����������
bool i2cm_Write(TI2C Phy, uint8_t Address, uint8_t Register, const uint8_t * Data, uint8_t Length, I2CM_TX_Handler Handler)
{
    TI2CQueue * Q = GetFreeQueue();
    
    if(!Q) return false; // ��� ����
    if(Length > QUEUE_BUFFER_SIZE) return false; // ������ �����
    if(Phy >= I2C_COUNT) return false; // ���� ������
    
    Q->InUse = true;
    
    Q->Phy = Phy;
    Q->Address = Address;
    Q->Register = Register;
    
    Q->Count = Length;
    Q->TX = Handler;
    
    // ��������� ������
    memcpy(&Q->Buffer[0], Data, Length);
    return true;
}

// ��������� �� ����������
bool i2cm_Read(TI2C Phy, uint8_t Address, uint8_t Register, uint8_t Length, I2CM_RX_Handler Handler)
{
    TI2CQueue * Q = GetFreeQueue();
    
    if(!Q) return false; // ��� ����
    if(Length > QUEUE_BUFFER_SIZE) return false; // ������ �����
    if(Phy >= I2C_COUNT) return false; // ���� ������
    
    Q->InUse = true;
    Q->Read = true;
    
    Q->Phy = Phy;
    Q->Address = Address;
    Q->Register = Register;

    Q->Count = Length;
    Q->RX = Handler;
    
    return true;
}

// =============================================

static void StopQueue(TI2CMState * S, TI2CStatus Status)
{
    TI2CQueue * Q = S->Queue;
    
    S->Timeout = 0;
    
    if(Q)
    {
        // �������� ���������
        Q->Completed = true;
    
        // ������:
        Q->Status = Status;
    }
}

// ����� ����� �� ������ ��������
static void Timeout(TI2CMState * S)
{
    StopQueue(S, I2C_TIMEOUT);
}

// ����������� ���������
static void Error(TI2CMState * S)
{
    StopQueue(S, I2C_ERROR);
}

static void EventInterrupt(TI2C Phy, TI2CEvent Event, uint8_t Data)
{
    TI2CMState * S = States[Phy];
    TI2CQueue * Q = S->Queue;
    
	switch(Event)
	{
		case I2CE_TXE:        
			if(S->Step == I2C_START)
			{
				i2c_Send(Phy, Q->Register);
				ResetTimeout(S);
				S->Step = I2C_WRITE;
				return;
			}
			// ����� ��������� ���� ������
			if(S->Step == I2C_WRITE)
			{
				i2c_Send(Phy, Q->Buffer[Q->Index]);
				ResetTimeout(S);
				S->Step = I2C_WRITE;
				Q->Index++;
				return;
			}
			break;
		case I2CE_TC:
			// ����� ��������� ��������� �����
			if((Q->Read) && (S->Step == I2C_WRITE))
			{
				Q->Index = 0;
				
				i2c_RepStart(Phy, Q->Address, Q->Count);
				ResetTimeout(S);
				S->Step = I2C_READ;
				return;
			}
			else
			{
				i2c_Stop(Phy);
				StopQueue(S, I2C_OK);
				S->Step = I2C_NONE;
				return;
			}
		case I2CE_RXNE:        
			if(S->Step == I2C_READ)
			{
				Q->Buffer[Q->Index] = Data;
				Q->Index++;
				return;
			}
			break; 
		case I2CE_STOP:
			if(Q->Index == Q->Count)
			{
				StopQueue(S, I2C_OK);
				return;
			}
			break;
		case I2CE_ERROR:
			i2c_Stop(Phy);
			Error(S);
			break;
		default:
			break;
	}
}

// ������
static void OnTimer(void)
{
    int i;
    
    // ���������, ���� �� �������?
    for(i = 0; i < I2C_COUNT; i++)
    {
        TI2CMState * S = States[i];
        
        // ���� ��� �����-�� ��������
        if(S && S->Timeout)
        {
            S->Timeout--;
            
            //if(!C->Timeout) i2c_Timeout(C);
        }
    }
}

// =============================================

// ������ ������
static void ClearQueue(int Index)
{
    if(Index >= QUEUE_COUNT) return;
    
    memset(&QueueList[Index], 0, sizeof(QueueList[0]));
}

// ������������� ������ I2C
bool i2cm_Init(TI2C Phy, uint8_t Pins)
{
	if(Phy < I2C_COUNT)
	{
		// �������� ��?
		if(!States[Phy])
		{
			uint16_t TotalLength = sizeof(TI2CMState);
			uint8_t * Buffer = (uint8_t*)mem_Alloc(TotalLength);
			if(Buffer)
			{
				memset(Buffer, 0, TotalLength);
				States[Phy] = (TI2CMState*)Buffer;
				i2c_Init(Phy, Pins, EventInterrupt);
				i2c_Master(Phy);
				return true;
			}
		}
	}
	return false;
}

void i2cm_Deinit(TI2C Phy)
{
	if(Phy < I2C_COUNT)
	{
		// �������� ��?
		if(States[Phy])
		{
			i2c_Deinit(Phy);
			mem_Free(&States[Phy]);
			States[Phy] = 0;
		}
	}
}

// ������? (���� ������ �� ���������)
bool i2cm_IsBusy(void)
{
    int i;
    // ���� ��������� �����
    for(i = 0; i < QUEUE_COUNT; i++) if(QueueList[i].InUse == false) return false;
    
    return true;
}

// �������������
static bool Init(void)
{
    if(!drv_Require(&i2cm, "i2c")) return false;
    if(!drv_Require(&i2cm, "memory")) return false;
    if(!drv_Require(&i2cm, "systimer")) return false;
	
    // ��� ��������� ��������
    timer_AddFunction(100, &OnTimer);
	
    return true;
}

static void Uninit(void)
{
	int i;
	for(i = 0; i < I2C_COUNT; i++)
	{
		if(States[i])
			mem_Free(States[i]);
	}
	memset(States, 0, sizeof(States));
}

static bool Main(void)
{
    int i;
    
    // ���������, ���� �� �������?
    for(i = 0; i < QUEUE_COUNT; i++)
    {
        TI2CQueue * Q = &QueueList[i];
        
        // ��������� ������?
        if((Q->Completed))
        {
            // ������?
            if(Q->Read)
            {
                uint8_t * Buffer = 0;
                uint8_t   Readed = 0;
                
                // ���� �� ��, �� �������, ��� ��������� � �������
                if(!Q->Status)
                {
                    Buffer = &Q->Buffer[0];
                    Readed = Q->Index;
                }
                
                if(Q->RX) Q->RX(Q->Status, Buffer, Readed);
            }
            else
            {
                if(Q->TX) Q->TX(Q->Status);
            }

            // ������, ��� ������ ���������
            {
                TI2CMState * S = States[Q->Phy];
                
				if(S)
				{
					// ����� �� ������
					S->Queue = 0;
				}
            }
            
            // ������� ���� ��� ��������� ������
            ClearQueue(i);
        }
    }
    
    // ������ ������ ��� ��������������
    for(i = 0; i < QUEUE_COUNT; i++)
    {
        TI2CQueue * Q = &QueueList[i];
        TI2CMState * S;
        
        // ����� - �����
        if(!Q->InUse) continue;
        
        // ������� �����...
        S = States[Q->Phy];
        
		if(!S) continue;
		
        // ���� ����� ����� ���-�� ����, ��� ������
        if(S->Queue) continue;
        
        // ��������! �����������
        S->Queue = Q;
        
        // ����� � ������ � ����
        S->Timeout = 0;
        S->Step = I2C_NONE;

        // ��� START, ������ ������� ����������, ��� ������
        i2c_Start(Q->Phy, Q->Address, (Q->Read) ? 1 : (Q->Count + 1), !Q->Read);
		ResetTimeout(S);
        S->Step = I2C_START;
    }
	return true;
}

const TDriver i2cm = { "i2cm", Init, Uninit, Main };
