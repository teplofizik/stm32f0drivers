// ******************************************************
// settings_ll.c
//
// teplofizik
// ******************************************************
 
#include "settings_ll.h"

// TODO: �������� �������� (sd/eeprom/flash, � ������������)
//       ��������� ������� ������ ��� �������� ������ - � �������� � ��� (��������, ������ �� ���������).
//       �� ��������� ��������� ���������� �������� ���� �����. ��� ����� �������� ���������.

// ���������� �������, ��������� ��� ������
int sll_PageCount(void)
{
    return 2;
}
    
// ������� ��������
void sll_Erase(int Page)
{
    
}

// ��������� ���� �� ������
void sll_Read(int Page, int Offset, uint8_t * Data, int Length)
{
    
}

// �������� ���� � ������
void sll_Write(int Page, int Offset, const uint8_t * Data, int Length)
{
    
}

// ������ ����� �� ��� ���������
void sll_Duplicate(int Page, int Length)
{

}    
