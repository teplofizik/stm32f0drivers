// ***********************************************************
//	flash.h
//  Работа с энергонезависимой памятью v2.0.0
//
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _FLASH_H
#define _FLASH_H

     // Очистка памяти перед записью
     void flash_Erase(void * From, void * To);

     // Читать память из FLASH
     void flash_Read(void * Dest, const void * Address, uint32_t Length);
    
     // Записать память во FLASH
     void flash_Write(const void * Src, void * Address, uint32_t Length);
     
	 extern const TDriver flash;
	 
#endif
