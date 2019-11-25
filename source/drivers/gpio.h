// ***********************************************************
//	gpio.h
//  Порты ввода-вывода v2.0.0
//
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _GPIO_H
#define _GPIO_H

    typedef enum { 
		PA, 
		PB, 
		PC, 
		PD, 
#ifdef AVAIL_GPIOE
		PE, 
#endif
		PF 
	} TPort;

    typedef struct
    {
        TPort   Port;
        uint8_t Pin;
    } TPin;
        
    // Настроить вывод на выход
    void gp_Output(const TPin * Pin);

    // Настроить вывод на вход
    void gp_Input(const TPin * Pin);

    // Установить для вывода аналоговый режим
    void gp_Analog(const TPin * Pin);
    
    // Высокий уровень на ножке
    void gp_High(const TPin * Pin);

    // Низкий уровень на ножке
    void gp_Low(const TPin * Pin);

    // Значение на ножке
    bool gp_Get(const TPin * Pin);

    // Установить значение на ножке
    void gp_Set(const TPin * Pin, bool Value);
    
    // Включить Open Drain (иначе двухтактный)
    void gp_OpenDrain(const TPin * Pin, bool Enable);
    
    // Максимальная скорость работы вывода: 50 МГц
    void gp_FastSpeed(const TPin * Pin);
    
    // Выключить подтяжку
    void gp_NoPull(const TPin * Pin);

    // Включить подтяжку к питанию
    void gp_PullUp(const TPin * Pin);
        
    // Включить подтяжку к земле
    void gp_PullDown(const TPin * Pin);

    // Установить для вывода альтернативную функцию (0-3)
    void gp_AlternateFunction(const TPin * Pin, uint8_t Function);

    // Одинаковые ли выводы?
    bool gp_Equals(const TPin * A, const TPin * B);

    extern const TDriver gpio;

#endif

