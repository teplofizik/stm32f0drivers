// ***********************************************************
//	dactest.c
//  Тест ЦАП
//
//  teplofizik, 2016
// ***********************************************************

#include "test.h"
#include "../drivers.h"

int Index = 0;
extern const int Frequency;
extern const uint8_t Table[100];
    
static void OnTimer(void)
{
    dac_Set(DAC_OUT1, Table[Index]);
    
    if(Index < sizeof(Table)/sizeof(Table[0])-1)
        Index++;
    else
        Index = 0;
}

// Запуск тестовой программы
static bool test_Init(void)
{
    dac_Enable(DAC_OUT1);
    
    timer_AddFunction(Frequency, &OnTimer);
    return true;
}

// Завершение тестовой программы
static void test_Uninit(void)
{
    dac_Disable(DAC_OUT1);
    
    timer_Remove(&OnTimer);
}

static const char * Drivers[] = { "systimer", "dac", 0 };

const TApp dactest = {"dactest", &Drivers[0], &test_Init, &test_Uninit, 0};
