// ******************************************************
// settings_ll.h
//
// teplofizik
// ******************************************************
 
#include <stdint.h>
#include <stdbool.h>
 
#ifndef SETTINGS_LL_H_
#define SETTINGS_LL_H_

    // ���������� �������, ��������� ��� ������
    int sll_PageCount(void);

    // ������� ��������
    void sll_Erase(int Page);

    // ��������� ���� �� ������
    void sll_Read(int Page, int Offset, uint8_t * Data, int Length);

    // �������� ���� � ������
    void sll_Write(int Page, int Offset, const uint8_t * Data, int Length);
            

#endif
