// ***********************************************************
//	dmxtxtest.c
//  Тестовая программа RMX TX
//
//  teplofizik, 2016
// ***********************************************************

#include "test.h"
#include "../drivers.h"
#include <string.h>

uint8_t DMX[513];
uint8_t DMXSendState;

static void test_SendBreak(void)
{
	const uint8_t Break[] = {0};
	
	DMXSendState = 1;
	uart_SetBaudrate(UART2, 78500);
	uart_Send(UART2, Break, 1);
}

static void test_SendData(void)
{
	DMXSendState = 2;
	uart_SetBaudrate(UART2, 250000);
	uart_Send(UART2, DMX, sizeof(DMX));
}


static void OnTimer(void)
{
	if(DMXSendState == 0)
		test_SendBreak();
}

static void OnTransmitted(TUart Uart)
{
	if(DMXSendState == 1)
		test_SendData();
	else if(DMXSendState == 2)
		DMXSendState = 0;
}

// Запуск тестовой программы
static bool test_Init(void)
{
	int i;
    uart_Init(UART2, 250000, STOP_BITS_2);
    uart_TransmitEnable(UART2, true);
	
    uart_SetHandler(UART2, UART_EVENT_ONTRANSMITCOMPLETED, &OnTransmitted);
    timer_AddFunction(100, &OnTimer);
    
	for(i = 1; i < 31; i++)
        DMX[i] = i;
	
    return true;
}

// Запуск тестовой программы
static void test_Uninit(void)
{
    uart_Uninit(UART2);
    timer_Remove(&OnTimer);
}

static const char * Drivers[] = { "systimer", "uart", 0 };

const TApp dmxtxtest = {"dmxtxtest", &Drivers[0], &test_Init, &test_Uninit, 0};
