// ***********************************************************
//	touch.h
// 	��������� ������
//
//  ����������: clock, gpio, systimer
//  teplofizik, 2017
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"
#include "gpio.h"

#ifndef _TOUCH_H
#define _TOUCH_H

	#define ENABLE_MEASURE                                     1

	// ������������ ����� ��� ��������� ����
	void touch_ConfigIn  (const TPin * P, TEventHandler Handler);
	
	// ������������ ����� ��� ����� �� ������������� �����������
	void touch_ConfigCap (const TPin * P);
	
	// ������� �� ����� ���������
	bool touch_IsActivated(const TPin * P);
	
#if (ENABLE_MEASURE != 0)
	int16_t touch_Get(const TPin * P);
#endif

    extern const TDriver touch;
	
#endif
