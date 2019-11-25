// ***********************************************************
//	adc.h
// 	АЦП v2.1.1
//  АЦП на 16 каналов + каналы батарейки, опорного напряжения и 
//  температуры
//
//  Требования: gpio, dma, clock
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"
#include "gpio.h"

#ifndef _ADC_H
#define _ADC_H

    typedef enum
    {
        ADC_PA0,   //  0: PA0
        ADC_PA1,   //  1: PA1
        ADC_PA2,   //  2: PA2
        ADC_PA3,   //  3: PA3
        ADC_PA4,   //  4: PA4
        ADC_PA5,   //  5: PA5
        ADC_PA6,   //  6: PA6
        ADC_PA7,   //  7: PA7
        
        ADC_PB0,   //  8: PB0
        ADC_PB1,   //  9: PB1
        
        ADC_PC0,   // 10: PC0
        ADC_PC1,   // 11: PC1
        ADC_PC2,   // 12: PC2
        ADC_PC3,   // 13: PC3
        ADC_PC4,   // 14: PC4
        ADC_PC5,   // 15: PC5
        
        ADC_TEMP,  // Температура
        ADC_VREF,  // Опорное напряжение
        ADC_VBAT2  // Половина напряжения с батарейки (Vbat/2)
    } TADCChannel;

    // Получить номер канала (TADCChannel) по выводу или -1, если там нет канала АЦП
    int adc_GetChannel(const TPin * Pin);
    
    // Включить канал измерения
    void adc_Enable(TADCChannel Channel);
    
    // Выключить канал измерения
    void adc_Disable(TADCChannel Channel);
    
    // Получить результат измерения
	int adc_Get(TADCChannel Index);
    
    extern const TDriver adc;
    
#endif
