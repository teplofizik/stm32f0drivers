// ***********************************************************
//	i2cmtest.c
//  Тестовая программа I2C Master
//
//  teplofizik, 2017
// ***********************************************************

#include "test.h"
#include "../drivers.h"
#include <string.h>

static const uint8_t TestData[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0xFF };
static uint8_t TestReadout[5];

static void OnWrite(TI2CStatus Status)
{
	
}

static void OnRead(TI2CStatus Status, uint8_t * Buffer, uint8_t Length)
{
	if(Status == I2C_OK)
	{
		memcpy(TestReadout, Buffer, Length);
	}
}

static void TimerTest(void)
{
	static bool Tick = false;
	
	if(!Tick)
		i2cm_Write(PI2C2, 0x01, 0x00, TestData, sizeof(TestData), OnWrite);
	else
		i2cm_Read(PI2C2, 0x01, 0x03, sizeof(TestReadout), OnRead);
	
	Tick = !Tick;
}

// Запуск тестовой программы
static bool Init(void)
{
	// MASTER: PB10,PB11
	i2cm_Init(PI2C2, 0);
	
	timer_AddFunction(10, &TimerTest);
    return true;
}

// Запуск тестовой программы
static void Uninit(void)
{
	
}

static const char * Drivers[] = { "i2cm", "systimer", 0 };

const TApp i2cmtest = {"i2cmtest", &Drivers[0], &Init, &Uninit, 0};
