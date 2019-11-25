// ***********************************************************
//	i2c.h
//  I2C v1.0.3
//
//  ����������: clock, gpio
//  teplofizik, 2017
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _I2C_H
#define _I2C_H

	#define I2C_COUNT                                                                                2

	typedef enum
	{
		PI2C1, // 0:PB6,PB7;   1:PB8,PB9
		PI2C2, // 0:PB10,PB11; 1:PB13,PB14
	} TI2C;
	
	typedef enum
	{
		I2CE_ERROR, //     ������
		I2CE_TXE,   // X   ����� �������� �����������
		I2CE_TC,    // X   �������� ���������
		I2CE_RXNE,  // X   ������� ������ (Data: ���������� ����)
		I2CE_ADDR,  // X S ����� ����� �� ����� ������ � ������� ���������� (Data: 0-������,1-������)
		I2CE_NACK,  // X   ������� NACK
		I2CE_STOP,  // X   �������� ��������
	} TI2CEvent;
	
	typedef void (* TI2CHandler)(TI2C Phy, TI2CEvent Event, uint8_t Data);

	// ������������� ������ I2C � ��������� ����������� ����������
	bool i2c_Init(TI2C Phy, uint8_t Pins, TI2CHandler IRQHandler);
	
	// ��������������� I2C
	void i2c_Deinit(TI2C Phy);
	
	// ����� �������
	void i2c_Master(TI2C Phy);
	
	// ����� ������
	void i2c_Slave(TI2C Phy, uint8_t Address);
	
	// ������ ��������
	void i2c_Start(TI2C Phy, uint8_t Address, uint8_t Count, bool Autoend);
	
	// ��������� �����
	void i2c_RepStart(TI2C Phy, uint8_t Address, uint8_t Count);
	
	// ��������� ��������
	void i2c_Stop(TI2C Phy);
	
	// �������� NACK
	void i2c_SetNack(TI2C Phy, bool Value);
	
	// �������� ������ (� �������� ���������)
	void i2c_Send(TI2C Phy, uint8_t Data);
	
    extern const TDriver i2c;

#endif
