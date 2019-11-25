// ***********************************************************
//	led.h
// 	������� ����������� v2.0.0 (stm32f0discovery)
//
//  ����������: gpio, systimer
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "../drivers/driver.h"

#ifndef _LED_H
#define _LED_H

    typedef enum { LED_BLUE, LED_GREEN } TLed;

    // �������� ��������� �� ��� ������
    void led_On(uint8_t Index);
    
    // ��������� ��������� �� ��� ������
    void led_Off(uint8_t Index);

    // ������ ��������� ����������
    void led_Set(uint8_t Index, bool Value);
    
    // ��������� ���������
    void led_Activate(uint8_t Index);
    
    extern const TDriver led;
    
#endif
