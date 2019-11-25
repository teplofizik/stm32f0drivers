// ***********************************************************
//  timer.h
// 	Управление таймерами v2.0.5
//  Таймеры
//
//  Требования: gpio, clock
//  teplofizik, 2017
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _TIMER_H
#define _TIMER_H

	typedef enum
	{
		TNA = 0, T1 = 1, T2 = 2, T3 = 3, T14 = 14, T15 = 15, T16 = 16, T17 = 17
	} TTimer;

    typedef enum
    {
		// PORT A
        OC_PA0,      // PA0,  TIM2_CH1    [X]
        OC_PA1_T2,   // PA1,  TIM2_CH2    [X]
        OC_PA1_T15,  // PA1,  TIM15_CH1N  [X]
        OC_PA2_T2,   // PA2,  TIM2_CH3    [X]
        OC_PA2_T15,  // PA2,  TIM15_CH1   [X]
        OC_PA3_T2,   // PA3,  TIM2_CH4    [X]
        OC_PA3_T15,  // PA3,  TIM15_CH2   [X]
        OC_PA4,      // PA4,  TIM14_CH1   [X]
        OC_PA5,      // PA5,  TIM2_CH1    [X]
        OC_PA6_T3,   // PA6,  TIM3_CH1    [X]
        OC_PA6_T16,  // PA6,  TIM16_CH1   [X]
        OC_PA7_T1,   // PA7,  TIM1_CH1N   [X]
        OC_PA7_T3,   // PA7,  TIM3_CH2    [X]
        OC_PA7_T14,  // PA7,  TIM14_CH1   [X]
        OC_PA7_T17,  // PA7,  TIM17_CH1   [X]
        OC_PA8,      // PA8,  TIM1_CH1    [X]
        OC_PA9,      // PA9,  TIM1_CH2    [X]
        OC_PA10,     // PA10, TIM1_CH3    [X]
        OC_PA11,     // PA11, TIM1_CH4    [X]
        OC_PA15,     // PA15, TIM2_CH1    [X]
		
		// PORT B
        OC_PB0_T1,    // PB0,  TIM1_CH2N  [X]
        OC_PB0_T3,    // PB0,  TIM3_CH3   [X]
        OC_PB1_T1,    // PB1,  TIM1_CH3N  [X]
        OC_PB1_T3,    // PB1,  TIM3_CH4   [X]
        OC_PB1_T14,   // PB1,  TIM14_CH1  [X]
        OC_PB3,       // PB3,  TIM2_CH2   [X]
        OC_PB4,       // PB4,  TIM3_CH1   [X]
        OC_PB5,       // PB5,  TIM3_CH2   [X]
        OC_PB6,       // PB6,  TIM16_CH1N [X]
        OC_PB7,       // PB7,  TIM17_CH1N [X]
        OC_PB8,       // PB8,  TIM16_CH1  [X]
        OC_PB9,       // PB9,  TIM17_CH1  [X]
        OC_PB10,      // PB10, TIM2_CH3   [X]
        OC_PB11,      // PB11, TIM2_CH4   [X]
        OC_PB13,      // PB13, TIM1_CH1N  [X]
        OC_PB14_T1,   // PB14, TIM1_CH2N  [X]
        OC_PB14_T15,  // PB14, TIM15_CH1  [X]
        OC_PB15_T1,   // PB15, TIM1_CH3N  [X]
        OC_PB15_T15,  // PB15, TIM15_CH2  [X]
        OC_PB15_T15N, // PB15, TIM15_CH1N [X]
		
		// PORT C
        OC_PC6,      // PC6,  TIM3_CH1    [X]
        OC_PC7,      // PC7,  TIM3_CH2    [X]
        OC_PC8,      // PC8,  TIM3_CH3    [X]
        OC_PC9,      // PC9,  TIM3_CH4    [X]
		
		// PORT E
#ifdef AVAIL_GPIOE
		OC_PE0,      // PE0,  TIM16_CH1
		OC_PE1,      // PE1,  TIM17_CH1
		OC_PE3,      // PE3,  TIM3_CH1
		OC_PE4,      // PE4,  TIM3_CH2
		OC_PE5,      // PE5,  TIM3_CH3
		OC_PE6,      // PE6,  TIM3_CH4
		OC_PE8,      // PE8,  TIM1_CH1N
		OC_PE9,      // PE9,  TIM1_CH1
		OC_PE10,     // PE10, TIM1_CH2N
		OC_PE11,     // PE11, TIM1_CH2
		OC_PE12,     // PE12, TIM1_CH3N
		OC_PE13,     // PE13, TIM1_CH3
		OC_PE14,     // PE14, TIM1_CH4
#endif
		// PORT F
		OC_PF9,      // PF9,  TIM15_CH1
		OC_PF10,     // PF10, TIM15_CH2
    } TOCChannel;

	
	// Получить связанный с каналом таймер
	TTimer tim_GetTimerFromOC(TOCChannel OC);
	
	// Запуск счёта таймера
	void tim_Start(TTimer T);
	
	// Остановка счёта таймера
	void tim_Stop(TTimer T);
	
	// Настройка таймера
	void tim_Init(TTimer T, uint32_t Counts, uint32_t Frequency);
	
	// Разрешить или запретить прерывания
	void tim_InterruptEnable(TTimer T, bool Set, uint32_t Mask);
	
	// Настроить выход как ШИМ
	void tim_InitOC_PWM(TOCChannel Channel, bool Polarity);
	
	// Установить значение регистра сравнения
	void tim_SetValue(TOCChannel Channel, uint16_t Value);
	
	// Выключение таймера
	void tim_Uninit(TTimer T);
	
    extern const TDriver timer;

#endif
