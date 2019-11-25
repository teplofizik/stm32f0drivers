// ***********************************************************
//  ledfawstpwm.h
// 	ШИМ v2.0.0
//  Быстрый ШИМ для светодиодов
//
//  Требования: gpio, clock, timer
//  teplofizik, 2017
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"
#include "timer.h"

#ifndef _LEDFASTPWM_H
#define _LEDFASTPWM_H

	// Максимальное значение ШИМ
	uint16_t lfpwm_GetMaxPWMValue(void);
	
    // Инициализация модуля
    void lfpwm_Init(TOCChannel Channel, bool Polarity);

    // Задать скважность
    void lfpwm_Set(TOCChannel Channel, uint8_t Value);
	
	// Задать скважность (0-MAX_PWM)
	void lfpwm_SetRaw(TOCChannel Channel, uint8_t Value);

    extern const TDriver ledfastpwm;

#endif
