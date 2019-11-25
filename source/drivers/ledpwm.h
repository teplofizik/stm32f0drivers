// ***********************************************************
//  ledpwm.h
// 	ШИМ v2.0.0
//  ШИМ для светодиодов
//
//  Требования: gpio, clock, timer
//  teplofizik, 2017
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"
#include "timer.h"

#ifndef _LEDPWM_H
#define _LEDPWM_H

	#define ENABLE_CURVE_16BIT                 0
	#define ENABLE_CURVE_2                     0
	#define ENABLE_CURVE_4                     1

	// Максимальное значение ШИМ
	uint16_t lpwm_GetMaxPWMValue(void);
	
    // Инициализация модуля
    void lpwm_Init(TOCChannel Channel, bool Polarity);

    // Задать скважность
    void lpwm_Set(TOCChannel Channel, uint16_t Value);
	
	// Задать скважность (0-MAX_PWM)
	void lpwm_SetRaw(TOCChannel Channel, uint16_t Value);

    extern const TDriver ledpwm;

	// Кривые ШИМ
#if (ENABLE_CURVE_16BIT != 0)
	extern const uint16_t led_Curve16[256];
#endif
#if (ENABLE_CURVE_2 != 0)
	extern const uint16_t led_Curve2[256];
#endif
#if (ENABLE_CURVE_4 != 0)
	extern const uint16_t led_Curve4[256];
#endif
    
#endif
