// ***********************************************************
//	activity.h
// 	������� ���������� v2.0.0
//
//  ����������: systimer
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _ACTIVITY_H
#define _ACTIVITY_H

    // ���������� ����������, �� ����������� ������� ���������
    #define ACT_TIMER_COUNT 10
    // ����� ��������� ����������, ��
    #define ACTIVITY_TIME   300

    typedef enum { ACT_TEST } TActivity;

    // �������� �� ����������
    void act_Activate(TActivity Act);
    
    // ��������� - ���� �� ������� ����������
    bool act_Get(TActivity Act);
    
    extern const TDriver activity;
    
#endif
