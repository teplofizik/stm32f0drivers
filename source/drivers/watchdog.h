// ***********************************************************
//	watchdog.h
// 	���������� ������
// 
//  teplofizik, 2015
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _WATCHDOG_H
#define _WATCHDOG_H

    // ������ �������
    void wdt_Init(void);
    
	// ����� �������
	void wdt_Reset(void);

#endif
