// ***********************************************************
//	test.c
//  Тестовая программа
//
//  teplofizik, 2016
// ***********************************************************

#include "test.h"
#include "../drivers.h"
#include <string.h>

int Allocated = 0;
int HeapUsage = 0;
int StackUsage = 0;

int AHB, APB1, SYSCLK;

static void OnTimer(void)
{
    static bool LedState = false;
    led_Set(LED_BLUE, LedState = !LedState);
    
    //Allocated = mem_HeapUsage();
    //HeapUsage = mem_HeapUsage() * 100 / mem_HeapSize();
    //StackUsage = mem_StackUsage() * 100 / mem_StackSize();
    
    //AHB = clock_GetAHB();
    //APB1 = clock_GetAPB1();
    //SYSCLK = clock_GetSYSCLK();
}

static void OnButton(uint8_t Button)
{
    led_Activate(LED_GREEN);
}

// Запуск тестовой программы
static bool Init(void)
{
    timer_AddFunction(100, &OnTimer);
    button_SetHandler(&OnButton);
    
    return true;
}

// Запуск тестовой программы
static void Uninit(void)
{
    timer_Remove(&OnTimer);
    button_SetHandler(0);
}

static const char * Drivers[] = { "systimer", "led", "button", 0 };

const TApp test = {"test", &Drivers[0], &Init, &Uninit, 0};
