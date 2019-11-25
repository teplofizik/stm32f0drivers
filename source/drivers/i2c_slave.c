// ***********************************************************
//	i2c_slave.c
//  I2C Slave v2.2.3
//
//  Требования: i2c, memory
//  teplofizik, 2017
// ***********************************************************

#include "i2c_slave.h"
#include "memory.h"
#include "gpio.h"
#include <string.h>

typedef struct
{
	I2CS_Handler    RxHandler;
	I2CS_Handler    TxHandler;
	uint8_t         Address;
	uint8_t         Size;
	
	bool            Read;   // Читаем?
	uint8_t         Index;  // Номер байта при записи (если 0 - начальный адрес)
	uint8_t         Offset; // Смещение в буфере
	               
	uint8_t         Data[];
} TI2CSState;

static TI2CSState * States[I2C_COUNT];

static void EventInterrupt(TI2C Phy, TI2CEvent Event, uint8_t Data)
{
    TI2CSState * S = States[Phy];
    
	switch(Event)
	{
		case I2CE_ADDR:
			// Connect, reset index
			S->Index = 0;
			S->Read = (Data != 0);
			if(!S->Read) S->Offset = 0;
			i2c_SetNack(Phy, false);
			if(S->Read)
			{
				// Первый байт
				uint8_t O = S->Offset;
				uint8_t In = (O < S->Size) ? S->Data[O] : 0;
				if(S->TxHandler) In = S->TxHandler(Phy, O, In);
				i2c_Send(Phy, In);
				S->Offset++;
			}
			break;
		case I2CE_NACK:
			break;
		case I2CE_TXE:
			{
				uint8_t O = S->Offset;
				uint8_t In = (O < S->Size) ? S->Data[O] : 0;
				if(S->TxHandler) In = S->TxHandler(Phy, O, In);
				i2c_Send(Phy, In);
				S->Offset++;
			}
			break;
		case I2CE_RXNE:
			if(S->Index)
			{
				if(S->Offset < S->Size)
				{
					uint8_t O = S->Offset;
					S->Offset++;
					i2c_SetNack(Phy, S->Offset == S->Size); // Set NACK
					
					if(S->RxHandler) 
						S->Data[O] = S->RxHandler(Phy, O, Data);
					else
						S->Data[O] = Data;
					return;
				}
			}
			else
				S->Offset = Data;
			S->Index++;
			break;
		default:
			break;
	}		
}
// Инициализация модуля I2C с заданным адресом и размером памяти под регистры
bool i2cs_Init(TI2C Phy, uint8_t Pins, uint8_t Address, uint8_t Size, I2CS_Handler RxHandler, I2CS_Handler TxHandler)
{
	if(Phy < I2C_COUNT)
	{
		// Свободно ли?
		if(!States[Phy])
		{
			uint16_t TotalLength = Size + sizeof(TI2CSState);
			uint8_t* Buffer = (uint8_t*)mem_Alloc(TotalLength);
			if(Buffer)
			{
				memset(Buffer, 0, TotalLength);
				States[Phy] = (TI2CSState*)Buffer;
				States[Phy]->Address = Address;
				States[Phy]->RxHandler = RxHandler;
				States[Phy]->TxHandler = TxHandler;
				States[Phy]->Size = Size;
				
				i2c_Init(Phy, Pins, EventInterrupt);
				i2c_Slave(Phy, (Address << 1) & 0xFE);
				return true;
			}
		}
	}
	return false;
}

void i2cs_Deinit(TI2C Phy)
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

// Прочитать память слейва
void i2cs_Read(TI2C Phy, uint8_t * Dest, uint8_t Address, uint8_t Size)
{
	if(Phy < I2C_COUNT)
	{
		// Свободно ли?
		if(States[Phy])
		{
			uint16_t Sz = States[Phy]->Size;
			uint8_t* Raw = &States[Phy]->Data[0];
			int i;
			
			for(i = 0; i < Size; i++)
			{
				uint16_t A = Address + i;
				if(A < Sz)
					Dest[i] = Raw[A];
				else
					Dest[i] = 0;
			}
		}
	}
}
	
// Записать память слейва
void i2cs_Write(TI2C Phy, uint8_t * Src, uint8_t Address, uint8_t Size)
{
	if(Phy < I2C_COUNT)
	{
		// Свободно ли?
		if(States[Phy])
		{
			uint16_t Sz = States[Phy]->Size;
			uint8_t* Raw = &States[Phy]->Data[0];
			int i;
			
			for(i = 0; i < Size; i++)
			{
				uint16_t A = Address + i;
				if(A < Sz)
					Raw[A] = Src[i];
				else
					break;
			}
		}
	}
}
	
// Прочитать байт из памяти слейва
uint8_t i2cs_ReadByte(TI2C Phy, uint8_t Address)
{
	if(Phy < I2C_COUNT)
	{
		// Свободно ли?
		if(States[Phy])
		{
			if(Address < States[Phy]->Size)
				return States[Phy]->Data[Address];
		}
	}
	
	return 0;
}
	
// Записать байт в память слейва
void i2cs_WriteByte(TI2C Phy, uint8_t Address, uint8_t Data)
{
	if(Phy < I2C_COUNT)
	{
		// Свободно ли?
		if(States[Phy])
		{
			if(Address < States[Phy]->Size)
				States[Phy]->Data[Address] = Data;
		}
	}
}
	
// Инициализация
static bool Init(void)
{
    if(!drv_Require(&i2cs, "i2c")) return false;
    if(!drv_Require(&i2cs, "memory")) return false;
	
	memset(States, 0, sizeof(States));
    return true;
}

static void Uninit(void)
{
	int i;
	for(i = 0; i < I2C_COUNT; i++)
	{
		if(States[i]) 
		{
			mem_Free(States[i]);
		}
	}
	memset(States, 0, sizeof(States));
}

const TDriver i2cs = { "i2cs", Init, Uninit, 0 };
