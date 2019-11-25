// ***********************************************************
//	i2c.h
//  I2C v1.0.3
//
//  Требования: clock, gpio
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
		I2CE_ERROR, //     Ошибка
		I2CE_TXE,   // X   Буфер передачи освободился
		I2CE_TC,    // X   Передача завершена
		I2CE_RXNE,  // X   Приняты данные (Data: полученный байт)
		I2CE_ADDR,  // X S Когда адрес на линии совпал с адресом устройства (Data: 0-запись,1-чтение)
		I2CE_NACK,  // X   Получен NACK
		I2CE_STOP,  // X   Завершён трансфер
	} TI2CEvent;
	
	typedef void (* TI2CHandler)(TI2C Phy, TI2CEvent Event, uint8_t Data);

	// Инициализация модуля I2C с указанием обработчика прерывания
	bool i2c_Init(TI2C Phy, uint8_t Pins, TI2CHandler IRQHandler);
	
	// Деинициализация I2C
	void i2c_Deinit(TI2C Phy);
	
	// Режим мастера
	void i2c_Master(TI2C Phy);
	
	// Режим слейва
	void i2c_Slave(TI2C Phy, uint8_t Address);
	
	// Начать трансфер
	void i2c_Start(TI2C Phy, uint8_t Address, uint8_t Count, bool Autoend);
	
	// Повторный старт
	void i2c_RepStart(TI2C Phy, uint8_t Address, uint8_t Count);
	
	// Закончить трансфер
	void i2c_Stop(TI2C Phy);
	
	// Контроль NACK
	void i2c_SetNack(TI2C Phy, bool Value);
	
	// Отправка данных (в процессе трансфера)
	void i2c_Send(TI2C Phy, uint8_t Data);
	
    extern const TDriver i2c;

#endif
