// ***********************************************************
//	dac.h
// 	ЦАП v2.0.0
//  ЦАП на два канала
//
//  Требования: gpio, clock
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _DAC_H
#define _DAC_H

    typedef enum
    {
        DAC_OUT1, // PA4
#ifdef AVAIL_DAC2
        DAC_OUT2  // PA5 (STM32F07x, STM32F09x)
#endif
    } TDACChannel;

    // Включить канал ЦАП
    void dac_Enable(TDACChannel Channel);
    
    // Выключить канал ЦАП
    void dac_Disable(TDACChannel Channel);
    
    // Задать значение выхода ЦАП 8 бит
	void dac_Set(TDACChannel Channel, uint8_t Value);
    
    // Задать значение выхода ЦАП 12 бит
    void dac_Set12(TDACChannel Channel, uint16_t Value);
    
    extern const TDriver dac;
    
#endif
