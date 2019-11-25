// ***********************************************************
//	bsp.h
// 	Board support package
//
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _BSP_H
#define _BSP_H

    // LED
    #include "bsp/led.h"

    // Button
    #include "bsp/button.h"
    
    // Обновление ПО
    #include "upgrade/upgrade.h"

    // Запустить софт поддержки
    extern const TDriver * BSP[];
   
#endif
