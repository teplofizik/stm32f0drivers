// ***********************************************************
//  ledpwm.�
// 	��� v2.0.0
//  ��� ��� �����������
//
//  ����������: gpio, clock, timer
//  teplofizik, 2017
// ***********************************************************

#include "ledpwm.h"
#include "gpio.h"
#include "clock.h"
#include <stm32f0xx.h>
#include <string.h>

// ������������ ����
static const int MAX_PWM = 26666;

// ������� ���: 1800 ��
static const int FREQUENCY = 600;

// ��������� �����
void lpwm_Init(TOCChannel Channel, bool Polarity)
{
	TTimer T = tim_GetTimerFromOC(Channel);
	
	if(T != TNA)
	{
		tim_Init(T, MAX_PWM, FREQUENCY);
		tim_InitOC_PWM(Channel, Polarity);
		tim_Start(T);
	}
}

uint16_t lpwm_GetMaxPWMValue(void)
{
	return MAX_PWM;
}

// ������ ���������� (0-MAX_PWM)
void lpwm_SetRaw(TOCChannel Channel, uint16_t Value)
{
    // ��������� ��������
    tim_SetValue(Channel, Value);
}

// ������ ���������� (0-0xFFFF)
void lpwm_Set(TOCChannel Channel, uint16_t Value)
{
    // ������� � ������� ����������
    uint16_t ScaledValue = (uint32_t)Value * MAX_PWM / 0xFFFF;
    
    // ��������� ��������
    tim_SetValue(Channel, ScaledValue);
}

static bool Init(void)
{
    if(!drv_Require(&ledpwm, "gpio")) return false;
    if(!drv_Require(&ledpwm, "clock")) return false;
    if(!drv_Require(&ledpwm, "timer")) return false;
    
    return true;
}

static void Uninit(void)
{
	
}

const TDriver ledpwm = { "ledpwm", &Init, &Uninit, 0 };
