// ***********************************************************
//  exti.h
//  Драйвер внешних прерываний v2.0.0
//
//  Требования: lowlevelsys
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#include "../types.h"
#include "gpio.h"

#ifndef _EXTI_H
#define _EXTI_H

    // Тип детектируемого фронта
    typedef enum
    {
        EDGE_RISING,
        EDGE_FALLING,
        EDGE_BOTH
    } TSignalEdge;

    // Подключение обработчика к заданному выводу для детектирования фронтов указанного типа
    bool exti_Listen(const TPin * Pin, TSignalEdge Edge, TEventHandler Handler);
    
	extern const TDriver exti;
	
#endif
