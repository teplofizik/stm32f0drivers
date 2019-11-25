// ***********************************************************
//	pwmtest.c
//  Тестовая программа ШИМ
//
//  teplofizik, 2017
// ***********************************************************

#include "test.h"
#include "../drivers.h"
#include <string.h>

static const TOCChannel PWM[] = {
	OC_PC9
};

static const int PWMCount = sizeof(PWM) / sizeof(PWM[0]);
uint8_t Value = 0x80;
uint16_t Value12 = 700;

static void OnTimerDAC(void)
{
	Value12++;
	if(Value12 > 3072) Value12 = 700;
	
	dac_Set12(DAC_OUT1, Value12);
}

static void OnTimer(void)
{
	int i;
	uint16_t CurvedPWM = led_Curve4[Value];
	Value++;
	
	for(i = 0; i < PWMCount; i++)
		lpwm_SetRaw(PWM[i], CurvedPWM);
}

// Запуск тестовой программы
static bool Init(void)
{
	int i;
	for(i = 0; i < PWMCount; i++)
		lpwm_Init(PWM[i], true);
	
	dac_Enable(DAC_OUT1);
    timer_AddFunction(5, &OnTimer);
    timer_AddFunction(50, &OnTimerDAC);

    return true;
}

// Запуск тестовой программы
static void Uninit(void)
{

}

static const char * Drivers[] = { "systimer", "ledpwm", 0 };

const TApp pwmtest = {"pwmtest", &Drivers[0], &Init, &Uninit, 0};
