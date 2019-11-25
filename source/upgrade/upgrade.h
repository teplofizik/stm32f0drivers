// ***********************************************************
//	upgrade.h
// 	Обновление ПО
//
//  teplofizik, 2014
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "../drivers/driver.h"
#include "../types.h"

#ifndef _UPGRADE_H
#define _UPGRADE_H

    // Стирание временного хранилища под прошивку
    void upg_Erase(void);
    
    // Записать кусок прошивки (32 б)
    void upg_Write(uint32_t Offset, uint8_t * Data, uint32_t Length);

    // Проверить контрольную сумму
    bool upg_Checksum(uint32_t Length, uint32_t Sum);

    // Обновить основное ПО
    void upg_Upgrade(void);

    // Занято?
    bool upg_Busy(void);
    
    // Установить функцию на блокировку длительных операций
    void upg_SetLockCallback(TLockHandler Handler);

    // Обновление ПО
    extern const TDriver upgrade;

#endif
