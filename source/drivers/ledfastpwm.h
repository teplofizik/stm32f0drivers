// ***********************************************************
//  ledfawstpwm.h
// 	��� v2.0.0
//  ������� ��� ��� �����������
//
//  ����������: gpio, clock, timer
//  teplofizik, 2017
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"
#include "timer.h"

#ifndef _LEDFASTPWM_H
#define _LEDFASTPWM_H

	// ������������ �������� ���
	uint16_t lfpwm_GetMaxPWMValue(void);
	
    // ������������� ������
    void lfpwm_Init(TOCChannel Channel, bool Polarity);

    // ������ ����������
    void lfpwm_Set(TOCChannel Channel, uint8_t Value);
	
	// ������ ���������� (0-MAX_PWM)
	void lfpwm_SetRaw(TOCChannel Channel, uint8_t Value);

    extern const TDriver ledfastpwm;

#endif
