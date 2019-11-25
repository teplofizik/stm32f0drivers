// ***********************************************************
//	clock.h
// 	Информация о тактировании v2.0.0
// 
//  teplofizik, 2016
// ***********************************************************

#include "clock.h"
#include "driver.h"
#include <stm32f0xx.h>

const uint32_t HSI = 8000000UL; // 8 MHz HSI
uint32_t       HSE = 8000000UL; // 8 MHz (настраивабельно)

typedef enum
{
	AHB,
	APB1,
	APB2,
} TClockBus;

typedef struct
{
	TPeripheral Periph;
	TClockBus   Bus;
	uint32_t    Mask;
} TPeriphDesc;

static const TPeripheral TimersX2[] = { PR_TIM2, PR_TIM3, PR_TIM6,
#ifdef AVAIL_TIM7
	PR_TIM7,
#endif
	PR_TIM14
};

static const int TimersX2Count = sizeof(TimersX2) / sizeof(TimersX2[0]);

static const TPeriphDesc Peripheral[] = {
	{ PR_NONE,  AHB, 0                  },   // PR_NONE
	
	// GPIO
	{ PR_PORTA, AHB, RCC_AHBENR_GPIOAEN },   // PR_PORTA
	{ PR_PORTB, AHB, RCC_AHBENR_GPIOBEN },   // PR_PORTB
	{ PR_PORTC, AHB, RCC_AHBENR_GPIOCEN },   // PR_PORTC
	{ PR_PORTD, AHB, RCC_AHBENR_GPIODEN },   // PR_PORTD
#ifdef AVAIL_GPIOE
	{ PR_PORTE, AHB, RCC_AHBENR_GPIOEEN },   // PR_PORTE
#endif
	{ PR_PORTF, AHB, RCC_AHBENR_GPIOFEN },   // PR_PORTF
	
	// Analog
	{ PR_DAC,  APB1, RCC_APB1ENR_DACEN },   // PR_DAC
	{ PR_ADC,  APB2, RCC_APB2ENR_ADCEN },   // PR_ADC

	// SPI
	{ PR_SPI1, APB2, RCC_APB2ENR_SPI1EN },   // PR_SPI1
	{ PR_SPI2, APB1, RCC_APB1ENR_SPI2EN },   // PR_SPI2
	
	// I2C
	{ PR_I2C1, APB1, RCC_APB1ENR_I2C1EN },   // PR_I2C1
	{ PR_I2C2, APB1, RCC_APB1ENR_I2C2EN },   // PR_I2C2
	
	// USART
	{ PR_USART1, APB2, RCC_APB2ENR_USART1EN }, // PR_USART1
	{ PR_USART2, APB1, RCC_APB1ENR_USART2EN }, // PR_USART2
#ifdef AVAIL_USART3
	{ PR_USART3, APB1, RCC_APB1ENR_USART3EN }, // PR_USART3
#endif
#ifdef AVAIL_USART4
	{ PR_USART4, APB1, RCC_APB1ENR_USART4EN }, // PR_USART4
#endif
#ifdef AVAIL_USART5
	{ PR_USART5, APB1, RCC_APB1ENR_USART5EN }, // PR_USART5
#endif
#ifdef AVAIL_USART6
	{ PR_USART6, APB2, RCC_APB2ENR_USART6EN }, // PR_USART6
#endif
#ifdef AVAIL_USART7
	{ PR_USART7, APB2, RCC_APB2ENR_USART7EN }, // PR_USART7
#endif
#ifdef AVAIL_USART8
	{ PR_USART8, APB2, RCC_APB2ENR_USART8EN }, // PR_USART8
#endif
	// DMA
	{ PR_DMA, AHB, RCC_AHBENR_DMAEN },     // PR_DMA
#ifdef AVAIL_DMA2
	{ PR_DMA2, AHB, RCC_AHBENR_DMA2EN },   // PR_DMA2
#endif
	
	// Memory
	{ PR_SRAM, AHB, RCC_AHBENR_SRAMEN },   // PR_SRAM
	{ PR_FLASH, AHB, RCC_AHBENR_FLITFEN }, // PR_FLASH
	
	// Timers
	{ PR_TIM1, APB2, RCC_APB2ENR_TIM1EN }, // PR_TIM1
	{ PR_TIM2, APB1, RCC_APB1ENR_TIM2EN }, // PR_TIM2
	{ PR_TIM3, APB1, RCC_APB1ENR_TIM3EN }, // PR_TIM3
	{ PR_TIM6, APB1, RCC_APB1ENR_TIM6EN }, // PR_TIM6
#ifdef AVAIL_TIM7
	{ PR_TIM7, APB1, RCC_APB1ENR_TIM7EN }, // PR_TIM7
#endif
	{ PR_TIM14, APB1, RCC_APB1ENR_TIM14EN }, // PR_TIM14
	{ PR_TIM15, APB2, RCC_APB2ENR_TIM15EN }, // PR_TIM15
	{ PR_TIM16, APB2, RCC_APB2ENR_TIM16EN }, // PR_TIM16
	{ PR_TIM17, APB2, RCC_APB2ENR_TIM17EN }, // PR_TIM17

	// Other
#ifdef AVAIL_USB
	{ PR_USB,      APB1, RCC_APB1ENR_USBEN },    // PR_USB
#endif
	{ PR_HDMI_CEC, APB1, RCC_APB1ENR_CECEN },    // PR_HDMI_CEC
	{ PR_WWDG,     APB1, RCC_APB1ENR_WWDGEN },   // PR_WWDG
	{ PR_SYSCFG,   APB2, RCC_APB2ENR_SYSCFGEN }, // PR_SYSCFG
	{ PR_PWR,      APB1, RCC_APB1ENR_PWREN },    // PR_PWR
	{ PR_CRC,      AHB,  RCC_AHBENR_CRCEN },     // PR_CRC
	{ PR_TSC,      AHB,  RCC_AHBENR_TSCEN },     // PR_TSC
#ifdef AVAIL_CAN
	{ PR_CAN,     APB1,  RCC_APB1ENR_CANEN },    // PR_CAN
#endif
#ifdef AVAIL_CRS
	{ PR_CRS,     APB1,  RCC_APB1ENR_CRSEN },    // PR_CRS
#endif
	{ PR_DBG,     APB2,  RCC_APB2ENR_DBGMCUEN } // PR_DBG
};

static const int PeripheralCount = sizeof(Peripheral) / sizeof(Peripheral[0]);

static const TPeriphDesc * GetPeriphDesc(TPeripheral P)
{
	int i;
	const TPeriphDesc * Fast = &Peripheral[P];
	if(Fast->Periph == P)
		return Fast;
	
	// Если оно не по порядку, ищем по списку
	for(i = 0; i < PeripheralCount; i++)
	{
		if(Peripheral[i].Periph == P)
			return &Peripheral[i];
	}
	
	// Нет такого
	return 0;
}

// Включить периферию
void clock_EnablePeripheral(TPeripheral Peripheral)
{
	const TPeriphDesc * PD = GetPeriphDesc(Peripheral);
	if(PD)
	{
		switch(PD->Bus)
		{
		case AHB:  RCC->AHBENR  |= PD->Mask; break;
		case APB1: RCC->APB1ENR |= PD->Mask; break;
		case APB2: RCC->APB2ENR |= PD->Mask; break;
		}
	}
}

// Выключить периферию
void clock_DisablePeripheral(TPeripheral Peripheral)
{
	const TPeriphDesc * PD = GetPeriphDesc(Peripheral);
	if(PD)
	{
		switch(PD->Bus)
		{
		case AHB:  RCC->AHBENR  &= ~PD->Mask; break;
		case APB1: RCC->APB1ENR &= ~PD->Mask; break;
		case APB2: RCC->APB2ENR &= ~PD->Mask; break;
		}
	}
}

// Сбросить периферию
void clock_ResetPeripheral(TPeripheral Peripheral)
{
	const TPeriphDesc * PD = GetPeriphDesc(Peripheral);
	if(PD)
	{
		switch(PD->Bus)
		{
		case AHB:  RCC->AHBRSTR  |= PD->Mask; RCC->AHBRSTR  &= ~PD->Mask; break;
		case APB1: RCC->APB1RSTR |= PD->Mask; RCC->APB1RSTR &= ~PD->Mask; break;
		case APB2: RCC->APB2RSTR |= PD->Mask; RCC->APB2RSTR &= ~PD->Mask; break;
		}
	}
}

bool IsPeripheralAPB1Timer(TPeripheral P)
{
	int i;
	for(i = 0; i < TimersX2Count; i++)
		if(TimersX2[i] == P)
			return true;
		
	return false;
}

// Выключить периферию
uint32_t clock_GetPeripheralClock(TPeripheral Peripheral)
{
	if(Peripheral == PR_FLASH)
		return HSI;
	else
	{
		const TPeriphDesc * PD = GetPeriphDesc(Peripheral);
		if(PD)
		{
			switch(PD->Bus)
			{
				case AHB: return clock_GetAHB();
				case APB1:
				{
					uint32_t Clk = clock_GetAPB();
					// У таймеров на APB1, если частота шины ниже системной (делитель есть), то
					// добавляется множитель *2
					if(IsPeripheralAPB1Timer(Peripheral) && (Clk < SystemCoreClock))
						Clk *= 2;
					
					return Clk;
				}
				case APB2: return clock_GetAPB();
			}
		}
	}
	
	return 0;
}

// Доступен ли внешний источник тактирования (HSE)
bool clock_IsAvailableHSE(void)
{
    return (RCC->CR & RCC_CR_HSEON) && (RCC->CR & RCC_CR_HSERDY);
}

// Доступен ли внутренний источник тактирования (HSI)
bool clock_IsAvailableHSI(void)
{
    return (RCC->CR & RCC_CR_HSION) && (RCC->CR & RCC_CR_HSIRDY);
}

// Частота на выходе PLL
uint32_t clock_GetPLLout(void)
{
    uint32_t In = 0;
    uint32_t PLLMUL = ((RCC->CFGR >> 18) & 0x0F) + 2;
    if(PLLMUL > 16) PLLMUL = 16; // 0..15 => 2..16
    
    if(RCC->CFGR & (1 << 16))
    {
        uint32_t PREDIV = RCC->CFGR2 & 0x0F;
        if(PREDIV)
            In = HSE / (PREDIV+1);
        else
            In = HSE;
    }
    else
        In = HSI / 2;
    
    return In * PLLMUL;
}

uint32_t clock_GetSYSCLK(void)
{
    int SYSCLKsrc = (RCC->CFGR >> 2) & 0x3;
    
    switch(SYSCLKsrc)
    {
    case 0: // HSI clock
        return HSI;
    case 1: // HSE clock
        return HSE;
    case 2: // PLL clock
        return clock_GetPLLout();
    // HSI48 not available
    }
    
    return 0;
}

uint32_t clock_GetAHB(void)
{
    const int Table[8] = { 2, 4, 8, 16, 64, 128, 256, 512 };

    if(RCC->CFGR & (1 << 7))
    {
        uint32_t AHBPRE = Table[(RCC->CFGR >> 4) & 0x07];
    
        return clock_GetSYSCLK() / AHBPRE;
    }
    else
        // No divide
        return clock_GetSYSCLK();
}

uint32_t clock_GetAPB(void)
{
    if(RCC->CFGR & (1 << 2))
    {
        uint8_t PPRE = (RCC->CFGR & 0x03);
        uint32_t APB1PRE = (2 << PPRE);
        
        return clock_GetAHB() / APB1PRE;
    }
    else
        return clock_GetAHB();
}

// Доступен ли выход ФАПЧ?
bool clock_IsAvailablePLL(void)
{
    return (RCC->CR & RCC_CR_PLLON) && (RCC->CR & RCC_CR_PLLRDY); 
}

// Частота работы ядра
uint32_t clock_GetSYS(void)
{
    return SystemCoreClock;
}

// Инициализация
static bool Init(void)
{
    return true;
}

const TDriver clock = { "clock", &Init, 0, 0};
