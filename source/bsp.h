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
    
    // ���������� ��
    #include "upgrade/upgrade.h"

    // ��������� ���� ���������
    extern const TDriver * BSP[];
   
#endif
