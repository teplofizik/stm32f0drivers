// ***********************************************************
//	i2c_slave.h
//  I2C Slave v2.2.3
//
//  Требования: i2c, memory
//  teplofizik, 2017
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"
#include "i2c.h"

#ifndef _I2C_SLAVE_H
#define _I2C_SLAVE_H

	// Запись в регистр (пропускаем обратно с учётом внутренних процессов)
	typedef uint8_t (* I2CS_Handler)(TI2C Phy, uint8_t Register, uint8_t Data);

	// Инициализация модуля I2C с заданным адресом и размером памяти под регистры
	bool i2cs_Init(TI2C Phy, uint8_t Pins, uint8_t Address, uint8_t Size, I2CS_Handler RxHandler, I2CS_Handler TxHandler);
	
	// Отключение модуля и освобождение памяти.
	void i2cs_Deinit(TI2C Phy);
	
	// Прочитать память слейва
	void i2cs_Read(TI2C Phy, uint8_t * Dest, uint8_t Address, uint8_t Size);
	
	// Записать память слейва
	void i2cs_Write(TI2C Phy, uint8_t * Src, uint8_t Address, uint8_t Size);
	
	// Прочитать байт из памяти слейва
	uint8_t i2cs_ReadByte(TI2C Phy, uint8_t Address);
	
	// Записать байт в память слейва
	void i2cs_WriteByte(TI2C Phy, uint8_t Address, uint8_t Data);
	
    extern const TDriver i2cs;

#endif
