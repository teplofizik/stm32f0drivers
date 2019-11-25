// ***********************************************************
//	systick.h
// 	��������� ������ v2.0.0
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

	// �������� ������� � ������ ������, Handler ����� ���������� � �������� ��������
	// ���������: 2
	//  Frequency - ������� ������ (��� ���������� ��������� ��������)
	//  Handler - �������, ������� ����� ����������
	// ���������: ���
	void timer_AddFunction(uint16_t Frequency, TEventHandler Handler);
	
    // ���������� ������
    void timer_Stop(TEventHandler Handler);
	
    // �������� ������
    void timer_Resume(TEventHandler Handler);

    // �������� ������� �������
    void timer_ChangeFrequency(TEventHandler Handler, uint16_t Frequency);
        
    // ������� ������
    void timer_Remove(TEventHandler Handler);

    extern const TDriver systimer;
#endif
