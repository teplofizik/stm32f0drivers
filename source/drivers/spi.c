// ***********************************************************
//	spi.c
//  SPI v1.0.0
//
//  Требования: clock, gpio
//  teplofizik, 2017
// ***********************************************************

#include "spi.h"
#include "clock.h"
#include "gpio.h"
#include <stm32f0xx.h>
#include <string.h>

typedef struct
{
	TPin          SCK;
	TPin          MISO;
	TPin          MOSI;
	uint8_t       AF;
} TSPIPins;

typedef struct
{
	TPeripheral   Peripheral;
	
	SPI_TypeDef * SPI;
	
	uint8_t       PinsCount;
	TSPIPins      Pins[2];
} TSPIPhy;

typedef struct
{
	bool        InUse;
	uint8_t     Pins;
} TStates;

static const TSPIPhy PHY[] = {
	{ PR_SPI1, SPI1, 2, { {{PA,5},{PA,6},{PA,7},0},    {{PB,3},{PB,4},{PB,5},0}} },   // 0:PA5,PA6,PA7;1:PB3,PB4,PB5
	{ PR_SPI2, SPI2, 1, { {{PB,13},{PB,14},{PB,15},0}, {{PD,1},{PD,3},{PD,4},1}} }, // 0:PB13,PB14,PB15;1:PD1,PD3,PD4
};
static const int PHYCount = sizeof(PHY) / sizeof(PHY[0]);

static TStates States[PHYCount];

static void InitPhy(const TSPIPhy * Phy, const TSPIPins * Pins, bool CPHA, bool CPOL)
{
	SPI_TypeDef * SPI = Phy->SPI;
	
	clock_EnablePeripheral(Phy->Peripheral);
	
	// Альтернативная функция
	gp_AlternateFunction(&Pins->SCK, Pins->AF);
	gp_AlternateFunction(&Pins->MISO, Pins->AF);
	gp_AlternateFunction(&Pins->MOSI, Pins->AF);
	
    gp_PullUp(&Pins->SCK);
    gp_PullUp(&Pins->MISO);
    gp_PullUp(&Pins->MOSI);
	
    SPI->CR1 = 0;
	
    // Настроим скорость передачи: 0b100 (fp / 8) 1000 КГц
    SPI->CR1 |= SPI_CR1_BR_2;
    
    // Программное управление SS (внутренним)
    SPI->CR1 |= SPI_CR1_SSI | SPI_CR1_SSM;
    
	if(CPHA) SPI->CR1 |= SPI_CR1_CPHA;
	if(CPOL) SPI->CR1 |= SPI_CR1_CPOL;
	
	// Установим флаг мастера (MSTR)
    SPI->CR1 |= SPI_CR1_MSTR;
	
    // DS 8 bit
    SPI->CR2 =  (SPI->CR2 & ~SPI_CR2_DS) | (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2);
    
    // Установим флаг разрешения работы (SPE)
    SPI->CR1 |= SPI_CR1_SPE;
}

// Инициализация модуля SPI
bool spi_Init(TSPI Phy, uint8_t Pins, bool CPHA, bool CPOL)
{
	if(Phy < PHYCount)
	{
		// Свободно ли?
		if(!States[Phy].InUse)
		{
			memset(&States[Phy], 0, sizeof(States[Phy]));
			
			States[Phy].InUse = true;
			States[Phy].Pins = Pins;
				
			InitPhy(&PHY[Phy], &PHY[Phy].Pins[Pins], CPHA, CPOL);
			return true;
		}
	}
	return false;
}
	
// Отправка данных
uint8_t spi_Send(TSPI Phy, uint8_t Data)
{
	if(Phy < PHYCount)
	{
		if(States[Phy].InUse)
		{
			SPI_TypeDef * SPI = PHY[Phy].SPI;
			
			// Отправим
			*((volatile uint8_t *)&SPI->DR) = Data;
			// Подождём завершения
			while((SPI->SR & SPI_SR_BSY));
			
			// Вернём полученное
			return *((volatile uint8_t *)&SPI->DR);
		}
	}
	
	return 0xFF;
}

static bool Init(void)
{
	memset(States, 0, sizeof(States));
	
	return true;
}

static void Uninit(void)
{
	
}

const TDriver spi = { "spi", Init, Uninit, 0 };
