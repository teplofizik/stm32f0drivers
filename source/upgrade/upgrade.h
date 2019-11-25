// ***********************************************************
//	upgrade.h
// 	���������� ��
//
//  teplofizik, 2014
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "../drivers/driver.h"
#include "../types.h"

#ifndef _UPGRADE_H
#define _UPGRADE_H

    // �������� ���������� ��������� ��� ��������
    void upg_Erase(void);
    
    // �������� ����� �������� (32 �)
    void upg_Write(uint32_t Offset, uint8_t * Data, uint32_t Length);

    // ��������� ����������� �����
    bool upg_Checksum(uint32_t Length, uint32_t Sum);

    // �������� �������� ��
    void upg_Upgrade(void);

    // ������?
    bool upg_Busy(void);
    
    // ���������� ������� �� ���������� ���������� ��������
    void upg_SetLockCallback(TLockHandler Handler);

    // ���������� ��
    extern const TDriver upgrade;

#endif
