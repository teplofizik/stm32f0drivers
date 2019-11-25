// ***********************************************************
//	touch.h
// 	Сенсорные кнопки
//
//  Требования: clock, gpio, systimer
//  teplofizik, 2017
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"
#include "gpio.h"

#ifndef _TOUCH_H
#define _TOUCH_H

	#define ENABLE_MEASURE                                     1

	// Использовать вывод как сенсорный вход
	void touch_ConfigIn  (const TPin * P, TEventHandler Handler);
	
	// Использовать вывод как выход на измерительный конденсатор
	void touch_ConfigCap (const TPin * P);
	
	// Активен ли канал измерения
	bool touch_IsActivated(const TPin * P);
	
#if (ENABLE_MEASURE != 0)
	int16_t touch_Get(const TPin * P);
#endif

    extern const TDriver touch;
	
#endif
