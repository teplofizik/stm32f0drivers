// ***********************************************************
//	system.c
//  Системные функции. Прерывания. И т.д.
//
//  Требования: clock
//  teplofizik, 2013
// ***********************************************************

#include "system.h"
#include "clock.h"
#include <stm32f0xx.h>

// Разрешить прерывания
void irq_enable(void)
{
    __enable_irq();
}

// Запретить прерывания
void irq_disable(void)
{
    __disable_irq();
}

// Перезагрузка
void reset(void)
{
    NVIC_SystemReset();
}

static bool Init(void)
{
    if(!drv_Require(&lowlevelsys, "clock")) return false;
	
    clock_EnablePeripheral(PR_SYSCFG);
    return true;
}

static void Uninit(void)
{
    clock_DisablePeripheral(PR_SYSCFG);
}

const TDriver lowlevelsys = { "lowlevelsys", &Init, &Uninit, 0 };
