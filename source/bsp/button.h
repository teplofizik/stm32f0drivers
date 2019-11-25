// ***********************************************************
//	button.h
// 	������� ������ v2.0.0
//
//  ����������: gpio, systimer
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "../drivers/driver.h"

#ifndef _BUTTON_H
#define _BUTTON_H

    typedef void (* TButtonHandler)(uint8_t Button);

    // ���������� ��������� ���������� �������
    void button_SetHandler(TButtonHandler Handler);

    // ��������� ������
    bool button_GetState(int Button);

    extern const TDriver button;
    
#endif
