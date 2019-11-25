// ***********************************************************
//	systick.h
// 	Системный таймер v2.0.0
//
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"
#include "../types.h"

#ifndef _SYSTICK_H
#define _SYSTICK_H

    #define SYSTEM_FREQUENCY 5000

	// Добавить функцию в список вызова, Handler будет вызываться с заданной частотой
	// Аргументы: 2
	//  Frequency - частота вызова (при увеличении снижается точность)
	//  Handler - функция, которая будет вызываться
	// Результат: нет
	void timer_AddFunction(uint16_t Frequency, TEventHandler Handler);
	
    // Остановить таймер
    void timer_Stop(TEventHandler Handler);
	
    // Включить таймер
    void timer_Resume(TEventHandler Handler);

    // Изменить частоту таймера
    void timer_ChangeFrequency(TEventHandler Handler, uint16_t Frequency);
        
    // Удалить таймер
    void timer_Remove(TEventHandler Handler);

    extern const TDriver systimer;
#endif
