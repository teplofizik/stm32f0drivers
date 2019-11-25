// ***********************************************************
//	clock.h
// 	Информация о тактировании и вкл/выкл периферии v2.0.0
// 
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _CLOCK_H
#define _CLOCK_H

	typedef enum
	{
		PR_NONE,     // ...
		// GPIO
		PR_PORTA,    // PORTA
		PR_PORTB,    // PORTB
		PR_PORTC,    // PORTC
		PR_PORTD,    // PORTD
#ifdef AVAIL_GPIOE
		PR_PORTE,    // PORTE
#endif
		PR_PORTF,    // PORTF
		
		// Analog
		PR_DAC,      // DAC
		PR_ADC,      // ADC
		
		// SPI
		PR_SPI1,     // SPI1
		PR_SPI2,     // SPI2
		
		// I2C
		PR_I2C1,     // I2C1
		PR_I2C2,     // I2C2
		
		// USART
		PR_USART1,   // USART1
		PR_USART2,   // USART2
#ifdef AVAIL_USART3
		PR_USART3,    // USART3
#endif
#ifdef AVAIL_USART4
		PR_USART4,    // USART4
#endif
#ifdef AVAIL_USART5
		PR_USART5,    // USART5
#endif
#ifdef AVAIL_USART6
		PR_USART6,    // USART6
#endif
#ifdef AVAIL_USART7
		PR_USART7,    // USART7
#endif
#ifdef AVAIL_USART8
		PR_USART8,    // USART8
#endif

		// DMA
		PR_DMA,      // DMA
#ifdef AVAIL_DMA2
		PR_DMA2,     // DMA2
#endif
		// Memory
		PR_SRAM,     // SRAM (enabled)
		PR_FLASH,    // Flash Interface (enabled)
		
		// Timers
		PR_TIM1,
		PR_TIM2,
		PR_TIM3,
		PR_TIM6,
#ifdef AVAIL_TIM7
		PR_TIM7,
#endif
		PR_TIM14,
		PR_TIM15,
		PR_TIM16,
		PR_TIM17,
		
		// Other
#ifdef AVAIL_USB
		PR_USB,
#endif
		PR_HDMI_CEC, // HDMI CEC
		PR_WWDG,     // Window watchdog
		PR_SYSCFG,   // System config
		PR_PWR,      // Power interface
		PR_CRC,      // CRC
		PR_TSC,      // Touch controller
#ifdef AVAIL_CAN
		PR_CAN,      // CAN
#endif
		PR_CRS,      // Clock Resque system
		PR_DBG,      // Debug module
	} TPeripheral;

    // Частота работы ядра
    uint32_t clock_GetSYSCLK(void);
    
    // Частота шины AHB
    uint32_t clock_GetAHB(void);

    // Частота шины APB1 и APB2
    uint32_t clock_GetAPB(void);
	
	// Включить периферию
	void clock_EnablePeripheral(TPeripheral Peripheral);
	
	// Выключить периферию
	void clock_DisablePeripheral(TPeripheral Peripheral);

	// Сбросить периферию
	void clock_ResetPeripheral(TPeripheral Peripheral);
	
	// Частота периферии
	uint32_t clock_GetPeripheralClock(TPeripheral Peripheral);
	
    extern const TDriver clock;
    
#endif
