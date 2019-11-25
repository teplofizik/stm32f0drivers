// ***********************************************************
//	i2c_master.c
//  I2C Master v2.0.0
//
//  Требования: i2c, memory, systimer
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
    
    // Адрес внешнего устройства
    uint8_t         Address;
    // Регистр
    uint8_t         Register;
    // Используемый модуль
    TI2C            Phy;
    
    // Занято?
    bool            InUse;
    // Чтение?
    bool            Read;
    // Завершено?
    bool            Completed;
    // Статус
    TI2CStatus      Status;
    
    // Буфер и количество элементов (запись/чтение)
    uint8_t         Buffer[QUEUE_BUFFER_SIZE];
    uint8_t         Count;
    uint8_t         Index;
} TI2CQueue;

typedef struct
{
	// Таймаут
    uint32_t      Timeout;
    // Текущая операция
    TI2CStep      Step;
    
    // Занято?
    TI2CQueue   * Queue; // Текущий запрос
} TI2CMState;

// А вот список
static TI2CQueue QueueList[QUEUE_COUNT];

static TI2CMState * States[I2C_COUNT];

// =============================================
// Процедуры низкого уровня
// =============================================

// Сбросить таймаут
static void ResetTimeout(TI2CMState * S)
{
    S->Timeout = I2C_TIMEOUT_VALUE;
}

// =============================================

// Пустая запись
static TI2CQueue * GetFreeQueue(void)
{
    int i;
    
    // Ищем свободное место
    for(i = 0; i < QUEUE_COUNT; i++) if(QueueList[i].InUse == false) return &QueueList[i];
    
    return 0;
}

// Записать в устройство
bool i2cm_Write(TI2C Phy, uint8_t Address, uint8_t Register, const uint8_t * Data, uint8_t Length, I2CM_TX_Handler Handler)
{
    TI2CQueue * Q = GetFreeQueue();
    
    if(!Q) return false; // Нет мест
    if(Length > QUEUE_BUFFER_SIZE) return false; // Больно жирно
    if(Phy >= I2C_COUNT) return false; // Нема такого
    
    Q->InUse = true;
    
    Q->Phy = Phy;
    Q->Address = Address;
    Q->Register = Register;
    
    Q->Count = Length;
    Q->TX = Handler;
    
    // Скопируем данные
    memcpy(&Q->Buffer[0], Data, Length);
    return true;
}

// Прочитать из устройства
bool i2cm_Read(TI2C Phy, uint8_t Address, uint8_t Register, uint8_t Length, I2CM_RX_Handler Handler)
{
    TI2CQueue * Q = GetFreeQueue();
    
    if(!Q) return false; // Нет мест
    if(Length > QUEUE_BUFFER_SIZE) return false; // Больно жирно
    if(Phy >= I2C_COUNT) return false; // Нема такого
    
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
        // Операция завершена
        Q->Completed = true;
    
        // Статус:
        Q->Status = Status;
    }
}

// Время вышло по данной операции
static void Timeout(TI2CMState * S)
{
    StopQueue(S, I2C_TIMEOUT);
}

// Неожиданное состояние
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
			// Нужно отправить байт данных
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
			// Нужно отправить повторный старт
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

// Таймер
static void OnTimer(void)
{
    int i;
    
    // Посмотрим, есть ли готовые?
    for(i = 0; i < I2C_COUNT; i++)
    {
        TI2CMState * S = States[i];
        
        // Если идёт какая-то операция
        if(S && S->Timeout)
        {
            S->Timeout--;
            
            //if(!C->Timeout) i2c_Timeout(C);
        }
    }
}

// =============================================

// Удалим заявку
static void ClearQueue(int Index)
{
    if(Index >= QUEUE_COUNT) return;
    
    memset(&QueueList[Index], 0, sizeof(QueueList[0]));
}

// Инициализация модуля I2C
bool i2cm_Init(TI2C Phy, uint8_t Pins)
{
	if(Phy < I2C_COUNT)
	{
		// Свободно ли?
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
		// Свободно ли?
		if(States[Phy])
		{
			i2c_Deinit(Phy);
			mem_Free(&States[Phy]);
			States[Phy] = 0;
		}
	}
}

// Занято? (пока ничего не отправить)
bool i2cm_IsBusy(void)
{
    int i;
    // Ищем свободное место
    for(i = 0; i < QUEUE_COUNT; i++) if(QueueList[i].InUse == false) return false;
    
    return true;
}

// Инициализация
static bool Init(void)
{
    if(!drv_Require(&i2cm, "i2c")) return false;
    if(!drv_Require(&i2cm, "memory")) return false;
    if(!drv_Require(&i2cm, "systimer")) return false;
	
    // Для таймаутов операций
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
    
    // Посмотрим, есть ли готовые?
    for(i = 0; i < QUEUE_COUNT; i++)
    {
        TI2CQueue * Q = &QueueList[i];
        
        // Завершена задача?
        if((Q->Completed))
        {
            // читали?
            if(Q->Read)
            {
                uint8_t * Buffer = 0;
                uint8_t   Readed = 0;
                
                // Если всё ок, то смотрим, что прочитали и сколько
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

            // Скажем, что задача завершена
            {
                TI2CMState * S = States[Q->Phy];
                
				if(S)
				{
					// Ничем не заняты
					S->Queue = 0;
				}
            }
            
            // Очистим поле для следующей заявки
            ClearQueue(i);
        }
    }
    
    // Теперь поищем ещё неотработанные
    for(i = 0; i < QUEUE_COUNT; i++)
    {
        TI2CQueue * Q = &QueueList[i];
        TI2CMState * S;
        
        // Пуста - лесом
        if(!Q->InUse) continue;
        
        // Смотрим канал...
        S = States[Q->Phy];
        
		if(!S) continue;
		
        // Если канал занят чем-то иным, ждём дальше
        if(S->Queue) continue;
        
        // Свободен! Резервируем
        S->Queue = Q;
        
        // Сразу и пустим в дело
        S->Timeout = 0;
        S->Step = I2C_NONE;

        // Шлём START, дальше автомат разберется, что делать
        i2c_Start(Q->Phy, Q->Address, (Q->Read) ? 1 : (Q->Count + 1), !Q->Read);
		ResetTimeout(S);
        S->Step = I2C_START;
    }
	return true;
}

const TDriver i2cm = { "i2cm", Init, Uninit, Main };
