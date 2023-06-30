// ***********************************************************
//	finetimer.h
//  Работа с точными задержками
//
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _FINETIMER_H
#define _FINETIMER_H

	typedef enum {
		FINETIMER_EVENT_OVERFLOW,
		FINETIMER_EVENT_COMPARE
	} EFineTimerEvent;

	// Обработчик событий таймера
	typedef void (*TFineTimerHandler)(EFineTimerEvent Event);

	// Запустить таймер на срабатывание после заданного периода времени (мкс)
	//  Period: период в мкс
	// Compare: время для срабатывания по сравнению
	void ftim_Start(int Period, int Compare);

	// Установка обработчика
	void ftim_SetHandler(TFineTimerHandler Handler);

    extern const TDriver finetimer;

#endif
