// ***********************************************************
//  rtc.h
// 	Управление часами v2.0.0
//  Таймеры
//
//  Требования: clock
//  teplofizik, 2019
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
 #include <time.h>
#include "driver.h"

#ifndef _RTC_H
#define _RTC_H

    extern const TDriver rtc;

	int rtc_SetTime(struct tm *time);
	int rtc_GetTime(struct tm *time);
	int rtc_SetTimeDebug(uint32_t Time, uint32_t Date, uint8_t Mask);
	
	bool rtc_Available(void);
 
#endif
