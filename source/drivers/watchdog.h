// ***********************************************************
//	watchdog.h
// 	Сторожевой таймер
// 
//  teplofizik, 2015
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _WATCHDOG_H
#define _WATCHDOG_H

    // Запуст таймера
    void wdt_Init(void);
    
	// Сброс таймера
	void wdt_Reset(void);

#endif
