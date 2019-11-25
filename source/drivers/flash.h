// ***********************************************************
//	flash.h
//  ������ � ����������������� ������� v2.0.0
//
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _FLASH_H
#define _FLASH_H

     // ������� ������ ����� �������
     void flash_Erase(void * From, void * To);

     // ������ ������ �� FLASH
     void flash_Read(void * Dest, const void * Address, uint32_t Length);
    
     // �������� ������ �� FLASH
     void flash_Write(const void * Src, void * Address, uint32_t Length);
     
	 extern const TDriver flash;
	 
#endif
