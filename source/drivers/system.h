// ***********************************************************
//	system.h
//  ��������� �������. ����������. � �.�.
//
//  ���� ������ � SYSCFG
//
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _SYSTEM_H
#define _SYSTEM_H

    // ��������� ����������
    void irq_enable(void);
    
    // ��������� ����������
    void irq_disable(void);

    // ������������
    void reset(void);
    
    extern const TDriver lowlevelsys;

#endif
