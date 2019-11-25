// ***********************************************************
//	i2c.с
//  I2C v1.0.3
//
//  Требования: clock, gpio
//  teplofizik, 2017
// ***********************************************************

#include "i2c.h"
#include "clock.h"
#include "gpio.h"
#include <stm32f0xx.h>
#include <string.h>

typedef struct
{
	TPin          SCL;
	TPin          SDA;
	uint8_t       AF;
} TI2CPins;

typedef struct
{
	TPeripheral   Peripheral;
	
	I2C_TypeDef * I2C;
	
	uint8_t       PinsCount;
	TI2CPins      Pins[2];
	
	IRQn_Type     IRQn;
} TI2CPhy;

typedef struct
{
	TI2CHandler Handler;
	uint8_t     Pins;
} TStates;

static const TI2CPhy PHY[] = {
	{PR_I2C1, I2C1, 2, { {{PB, 6},{PB, 7}, 1}, {{PB, 8},{PB, 9}, 1} }, I2C1_IRQn}, // I2C1: PB6,PB7;PB8,PB9
	{PR_I2C2, I2C2, 2, { {{PB,10},{PB,11}, 1}, {{PB,13},{PB,14}, 5} }, I2C2_IRQn}, // I2C2: PB10,PB11;PB13,PB14
};
static const int PHYCount = sizeof(PHY) / sizeof(PHY[0]);

static TStates States[PHYCount];

static void EventInterrupt(TI2C Phy, const TI2CPhy * PHY, TStates * S)
{
    uint32_t Status = PHY->I2C->ISR;
    
    if(Status & I2C_ISR_ARLO)
        PHY->I2C->ICR = I2C_ICR_ARLOCF;
	if(Status & I2C_ISR_NACKF)
	{
        PHY->I2C->ICR = I2C_ICR_NACKCF;
		if(S->Handler) S->Handler(Phy, I2CE_NACK, 0);
		return;
	}
	else if(Status & I2C_ISR_BERR)
    {
        PHY->I2C->ICR = I2C_ICR_BERRCF;
		return;
    }
	else if(Status & I2C_ISR_TXIS)
    {
		if(S->Handler) S->Handler(Phy, I2CE_TXE, 0);
		return;
    }
    else if(Status & I2C_ISR_TC)
    {
		if(S->Handler) S->Handler(Phy, I2CE_TC, 0);
		return;
	}
    else if(Status & I2C_ISR_RXNE)
    {
		if(S->Handler) S->Handler(Phy, I2CE_RXNE, PHY->I2C->RXDR);
		return;
	}
    else if(Status & I2C_ISR_STOPF)
    {
		PHY->I2C->ISR |= I2C_ISR_TXIS | I2C_ISR_TXE;
        PHY->I2C->ICR = I2C_ICR_STOPCF;
		if(S->Handler) S->Handler(Phy, I2CE_STOP, 0);
		return;
	}
	else if(Status & I2C_ISR_ADDR)
	{
        PHY->I2C->ICR = I2C_ICR_ADDRCF;
		if(S->Handler) S->Handler(Phy, I2CE_ADDR, (Status & I2C_ISR_DIR) != 0);
		return;
	}
	
	if(S->Handler) S->Handler(Phy, I2CE_ERROR, 0);
}

// Обработчик i2c, события
void I2C1_IRQHandler(void) { EventInterrupt(PI2C1, &PHY[0], &States[0]); }
void I2C2_IRQHandler(void) { EventInterrupt(PI2C2, &PHY[1], &States[1]); }

static void InitPhy(const TI2CPhy * Phy, const TI2CPins * Pins)
{
	I2C_TypeDef * I2C = Phy->I2C;
	
	clock_EnablePeripheral(Phy->Peripheral);
	
    // PE == 0, сброс
    I2C->CR1 = 0;
	
    // Настраиваем частоту
    I2C->TIMINGR = (0x13UL << 0)  | // SCLL
                   (0x0FUL << 8)  | // SCLH
                   (0x02UL << 16) | // SDADEL
                   (0x04UL << 20) | // SCLDEL
                   (0x0BUL << 28) | // PRESC
                    0;

    // Разрешим прерывания
    I2C->CR1 |= (I2C_CR1_ERRIE | I2C_CR1_TXIE | I2C_CR1_NACKIE | I2C_CR1_TCIE | I2C_CR1_RXIE | I2C_CR1_STOPIE | I2C_CR1_ADDRIE);
    
	// Для слейва
	//I2C->CR1 |= I2C_CR1_SBC;
	
    // Включим модуль
    I2C->CR1 |= (I2C_CR1_PE);
	
	
	// Подтяжка
	gp_PullUp(&Pins->SCL);
	gp_PullUp(&Pins->SDA);
	
	// Режим: открытый сток
	gp_OpenDrain(&Pins->SCL, true);
	gp_OpenDrain(&Pins->SDA, true);
       
	// Альтернативная функция
	gp_AlternateFunction(&Pins->SCL, Pins->AF);
	gp_AlternateFunction(&Pins->SDA, Pins->AF);
         
	// Разрешить прерывания
	NVIC_EnableIRQ(Phy->IRQn);
	NVIC_SetPriority(Phy->IRQn, 10);
}

static void DeinitPhy(const TI2CPhy * Phy, const TI2CPins * Pins)
{
	I2C_TypeDef * I2C = Phy->I2C;
	
    I2C->CR1 = 0;
	
	// TODO: Deinnit all
	
	// Альтернативная функция
	gp_Input(&Pins->SCL);
	gp_Input(&Pins->SDA);
         
	NVIC_DisableIRQ(Phy->IRQn);
}

// Инициализация модуля I2C с указанием обработчика прерывания
bool i2c_Init(TI2C Phy, uint8_t Pins, TI2CHandler IRQHandler)
{
	if(Phy < PHYCount)
	{
		// Свободно ли?
		if(!States[Phy].Handler)
		{
			memset(&States[Phy], 0, sizeof(States[Phy]));
			States[Phy].Handler = IRQHandler;
			States[Phy].Pins = Pins;
				
			InitPhy(&PHY[Phy], &PHY[Phy].Pins[Pins]);
			return true;
		}
	}
	return false;
}

// Деинициализация I2C
void i2c_Deinit(TI2C Phy)
{
	if(Phy < PHYCount)
	{
		// Свободно ли?
		if(States[Phy].Handler)
		{
			DeinitPhy(&PHY[Phy], &PHY[Phy].Pins[States[Phy].Pins]);
			memset(&States[Phy], 0, sizeof(States[Phy]));
		}
	}
}

// Режим мастера
void i2c_Master(TI2C Phy)
{
	if(Phy < PHYCount)
		PHY[Phy].I2C->OAR1 = 0;
}
	
// Режим слейва
void i2c_Slave(TI2C Phy, uint8_t Address)
{
	// Свой адрес:
	if(Phy < PHYCount)
		PHY[Phy].I2C->OAR1 = I2C_OAR1_OA1EN | Address;
}

// Контроль NACK
void i2c_SetNack(TI2C Phy, bool Value)
{
	if(Phy < PHYCount)
	{
		if(Value)
			PHY[Phy].I2C->CR2 |= I2C_CR2_NACK;
		else
			PHY[Phy].I2C->CR2 &= ~I2C_CR2_NACK;
	}
}

// Начать трансфер
void i2c_Start(TI2C Phy, uint8_t Address, uint8_t Count, bool Autoend)
{
	if(Phy < PHYCount)
	{
        PHY[Phy].I2C->CR2 = (Address << 1) | // Slave address
                            (Count << 16) | // Number of bytes
                            ((Autoend) ? I2C_CR2_AUTOEND : 0) |       // Stop or restart after completion
                            0;
        
		PHY[Phy].I2C->CR2 |= (I2C_CR2_START);
	}
}
	
// Закончить трансфер
void i2c_Stop(TI2C Phy)
{
	if(Phy < PHYCount)
		PHY[Phy].I2C->CR2 |= (I2C_CR2_STOP);
}
	
// Отправка данных (в процессе трансфера)
void i2c_Send(TI2C Phy, uint8_t Data)
{
	if(Phy < PHYCount)
		PHY[Phy].I2C->TXDR = Data;
}
	
void i2c_RepStart(TI2C Phy, uint8_t Address, uint8_t Count)
{
	if(Phy < PHYCount)
	{
		PHY[Phy].I2C->CR2 = (Address << 1) | // Slave address
						    (Count << 16) |  // Number of bytes
						    // I2C_CR2_AUTOEND | // Stop or restart after completion
						    I2C_CR2_RD_WRN |    // Read direction
						    0;
		
		PHY[Phy].I2C->CR2 |= (I2C_CR2_START);
	}
}

static bool Init(void)
{
    if(!drv_Require(&i2c, "clock")) return false;
    if(!drv_Require(&i2c, "gpio")) return false;
	
	memset(States, 0, sizeof(States));
    return true;
}

static void Uninit(void)
{
	int i;
	for(i = 0; i < PHYCount; i++)
	{
		if(States[i].Handler)
			clock_DisablePeripheral(PHY[i].Peripheral);
	}
	
	memset(States, 0, sizeof(States));
}

const TDriver i2c = { "i2c", Init, Uninit, 0 };

