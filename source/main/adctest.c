// ***********************************************************
//	adctest.c
//  ???????? ?????????
//
//  teplofizik, 2013
// ***********************************************************

#include "test.h"
#include "../drivers.h"

#define TEMP110_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7C2))
#define TEMP30_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7B8))
#define VDD_CALIB ((uint16_t) (330))
#define VDD_APPLI ((uint16_t) (300))

float Temperature = 0;
int   Adc1, Adc2;

static float test_GetTemperature(void)
{
    float temperature = (((int32_t) adc_Get(ADC_TEMP) * VDD_APPLI / VDD_CALIB) - (int32_t) *TEMP30_CAL_ADDR );
    temperature = temperature * (int32_t)(110 - 30);
    temperature = temperature / (int32_t)(*TEMP110_CAL_ADDR - *TEMP30_CAL_ADDR);
    temperature = temperature + 30;
    
    return temperature;
}

static void OnTimer(void)
{
    Temperature = test_GetTemperature();
    Adc1 = adc_Get(ADC_PA1);
    Adc2 = adc_Get(ADC_PA4);
}

// Запуск тестовой программы
static bool test_Init(void)
{
    adc_Enable(ADC_PA1);
    adc_Enable(ADC_PA4);
    adc_Enable(ADC_TEMP);
    
    Temperature = 0;
    Adc1 = 0;
    Adc2 = 0;
    
    timer_AddFunction(100, &OnTimer);
    return true;
}

// Завершение тестовой программы
static void test_Uninit(void)
{
    adc_Disable(ADC_PA1);
    adc_Disable(ADC_PA4);
    adc_Disable(ADC_TEMP);
    
    timer_Remove(&OnTimer);
}

static const char * Drivers[] = { "systimer", "adc", 0 };

const TApp adctest = {"adctest", &Drivers[0], &test_Init, &test_Uninit, 0};
