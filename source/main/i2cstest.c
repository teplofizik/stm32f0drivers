// ***********************************************************
//	i2cstest.c
//  Тестовая программа I2C Slave
//
//  teplofizik, 2017
// ***********************************************************

#include "test.h"
#include "../drivers.h"
#include <string.h>

static uint8_t RxHandler(TI2C Phy, uint8_t Register, uint8_t Data)
{
	// Прозрачная передача снаружи в буфер
	return Data;
}

static uint8_t TxHandler(TI2C Phy, uint8_t Register, uint8_t Data)
{
	// Прозрачная передача из буфера наружу
	return Data;
}

// Запуск тестовой программы
static bool Init(void)
{
	// SLAVE: PB6,PB7
	i2cs_Init(PI2C1, 0, 0x01, 0x20, &RxHandler, &TxHandler);
    return true;
}

// Запуск тестовой программы
static void Uninit(void)
{

}

static const char * Drivers[] = { "i2cs", 0 };

const TApp i2cstest = {"i2cstest", &Drivers[0], &Init, &Uninit, 0};
