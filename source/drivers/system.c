// ***********************************************************
//	system.c
//  ��������� �������. ����������. � �.�.
//
//  ����������: clock
//  teplofizik, 2013
// ***********************************************************

#include "system.h"
#include "clock.h"
#include <stm32f0xx.h>

// ��������� ����������
void irq_enable(void)
{
    __enable_irq();
}

// ��������� ����������
void irq_disable(void)
{
    __disable_irq();
}

// ������������
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
