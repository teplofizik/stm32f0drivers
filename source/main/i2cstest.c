// ***********************************************************
//	i2cstest.c
//  �������� ��������� I2C Slave
//
//  teplofizik, 2017
// ***********************************************************

#include "test.h"
#include "../drivers.h"
#include <string.h>

static uint8_t RxHandler(TI2C Phy, uint8_t Register, uint8_t Data)
{
	// ���������� �������� ������� � �����
	return Data;
}

static uint8_t TxHandler(TI2C Phy, uint8_t Register, uint8_t Data)
{
	// ���������� �������� �� ������ ������
	return Data;
}

// ������ �������� ���������
static bool Init(void)
{
	// SLAVE: PB6,PB7
	i2cs_Init(PI2C1, 0, 0x01, 0x20, &RxHandler, &TxHandler);
    return true;
}

// ������ �������� ���������
static void Uninit(void)
{

}

static const char * Drivers[] = { "i2cs", 0 };

const TApp i2cstest = {"i2cstest", &Drivers[0], &Init, &Uninit, 0};
