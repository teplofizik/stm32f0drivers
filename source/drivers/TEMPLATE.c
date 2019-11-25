// ***********************************************************
//  template.c
// 	Template Driver v2.0.0
//  Шаблон драйвера
//
//  Требования: gpio
//  teplofizik, 2017
// ***********************************************************

#include "pwm.h"
#include "gpio.h"
#include <stm32f0xx.h>
#include <string.h>

static bool Init(void)
{
    if(!drv_Require(&dtemplate, "gpio")) return false;
    
    return true;
}

static void Uninit(void)
{
	
}

const TDriver dtemplate = { "dtemplate", &Init, &Uninit, 0 };
