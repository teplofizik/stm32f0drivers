// ***********************************************************
//	spitest.c
//  Тестовая программа SPI Master
//
//  teplofizik, 2017
// ***********************************************************

#include "test.h"
#include "../drivers.h"
#include <string.h>

static const uint8_t TestData[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0xFF };
static const TPin CS = {PC, 4};

static void TimerTest(void)
{
	uint8_t i = 0;
	gp_Low(&CS);
	for(i = 0; i < sizeof(TestData)/sizeof(TestData[0]); i++)
		spi_Send(PSPI1, TestData[i]);
	gp_High(&CS);
}

// Запуск тестовой программы
static bool Init(void)
{
	gp_Output(&CS);
	gp_High(&CS);
	// SCK:PA5, MISO:PA6, MOSI:PA7
	spi_Init(PSPI1, 0, false, false);
	
	timer_AddFunction(10, &TimerTest);
    return true;
}

// Запуск тестовой программы
static void Uninit(void)
{
	
}

static const char * Drivers[] = { "spi", "gpio", "systimer", 0 };

const TApp spitest = {"spitest", &Drivers[0], &Init, &Uninit, 0};
