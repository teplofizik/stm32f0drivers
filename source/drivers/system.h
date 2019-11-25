// ***********************************************************
//	system.h
//  Системные функции. Прерывания. И т.д.
//
//  Дает доступ к SYSCFG
//
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _SYSTEM_H
#define _SYSTEM_H

    // Разрешить прерывания
    void irq_enable(void);
    
    // Запретить прерывания
    void irq_disable(void);

    // Перезагрузка
    void reset(void);
    
    extern const TDriver lowlevelsys;

#endif
