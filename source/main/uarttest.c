// ***********************************************************
//	uarttest.c
//  Тестовая программа
//
//  teplofizik, 2013
// ***********************************************************

#include "test.h"
#include "../drivers.h"
#include <string.h>

uint8_t TestBuffer[10];
int TestIndex = 0;

static const TPin TestPin = {PB, 9};

//const char Test[] = "ADL TEST TEST";

static void OnTimer(void)
{
    //uart_Send(UART1, (const uint8_t*)Test, strlen(Test));
}

static void OnReceive(TUart Uart, char C)
{
	if(TestIndex < sizeof(TestBuffer))
    {
        TestBuffer[TestIndex++] = C;
        
        if((TestIndex == 2) && (TestBuffer[1]) != 1)
        {
            gp_High(&TestPin);
            gp_Low(&TestPin);
        }
    }
}

static void OnBreak(TUart Uart)
{
    //gp_High(&TestPin);
	TestIndex = 0;
    //gp_Low(&TestPin);
}

// Запуск тестовой программы
static bool test_Init(void)
{
    uart_Init(UART1, 250000, STOP_BITS_2);
	uart_ReceiveEnable(UART1, true);
	
	uart_SetHandler(UART1, UART_EVENT_ONBREAK, &OnBreak); 
	uart_SetHandler(UART1, UART_EVENT_ONRECEIVE, &OnReceive); 
    timer_AddFunction(10, &OnTimer);
    
    gp_Output(&TestPin);
    gp_Low(&TestPin);
    return true;
}

// Запуск тестовой программы
static void test_Uninit(void)
{
    uart_Uninit(UART1);
    timer_Remove(&OnTimer);
    
    gp_Input(&TestPin);
}

static const char * Drivers[] = { "systimer", "uart", 0 };

const TApp uarttest = {"uarttest", &Drivers[0], &test_Init, &test_Uninit, 0};
