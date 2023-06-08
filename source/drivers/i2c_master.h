// ***********************************************************
//	i2c_master.h
//  I2C Master v2.0.1
//
//  ����������: i2c, memory, systimer
//  teplofizik, 2017
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"
#include "i2c.h"

#ifndef _I2C_MASTER_H
#define _I2C_MASTER_H

	// ������������ ���������� ��������
	#define QUEUE_COUNT  2

	// ������ ������
	#define QUEUE_BUFFER_SIZE 16

	// �������
	#define I2C_TIMEOUT_VALUE 100

    // �������
    typedef enum
    {
        I2C_OK,
        I2C_NACK, // ��� ������ �� ������ ������
        I2C_ERROR,
        I2C_TIMEOUT
    } TI2CStatus;
	
    // ���������� ��������/���������� ������
    typedef void (* I2CM_TX_Handler)(TI2CStatus Status);
    typedef void (* I2CM_RX_Handler)(TI2CStatus Status, uint8_t * Buffer, uint8_t Length);
    
	// ������������� ������ I2C
	bool i2cm_Init(TI2C Phy, uint8_t Pins);
	
	// ���������� ������ � ������������ ������.
	void i2cm_Deinit(TI2C Phy);
	
    // ������? (���� ������ �� ���������)
    bool i2cm_IsBusy(void);
    
    // ��������� �� ����������
    bool i2cm_Read(TI2C Phy, uint8_t Address, uint8_t Register, uint8_t Length, I2CM_RX_Handler Handler);
    
    // �������� � ����������
    bool i2cm_Write(TI2C Phy, uint8_t Address, uint8_t Register, const uint8_t * Data, uint8_t Length, I2CM_TX_Handler Handler);
    
    extern const TDriver i2cm;

#endif
