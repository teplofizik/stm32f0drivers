// ***********************************************************
//	thototest.c
//  Тестовая программа
//
//  teplofizik, 2013
// ***********************************************************

#include "test.h"
#include "../drivers.h"
#
#include <string.h>

static void proc1(uint32_t Argument)
{
	bool Test = false;
	
	while(true)
	{
		led_Set(1, Test = !Test);
		delay_ms(500);
	}
}

// Запуск тестовой программы
static bool test_Init(void)
{
	createprocess("proc1", &proc1, 0);
    return true;
}

// Запуск тестовой программы
static void test_Uninit(void)
{
	
}

static const char * Drivers[] = { "led", "thoto", 0 };

const TApp thototest = {"thototest", &Drivers[0], &test_Init, &test_Uninit, 0};
