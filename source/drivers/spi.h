// ***********************************************************
//	spi.h
//  SPI v1.0.0
//
//  Требования: clock, gpio
//  teplofizik, 2017
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _SPI_H
#define _SPI_H

	#define SPI_COUNT                                                                                2

	// Pins:SCK,MISO,MOSI
	typedef enum
	{
		PSPI1, // 0:PA5,PA6,PA7;    1:PB3,PB4,PB5
		PSPI2, // 0:PB13,PB14,PB15; 1:PD1,PD3,PD4
	} TSPI;

	// Инициализация модуля SPI
	bool spi_Init(TSPI Phy, uint8_t Pins, bool CPHA, bool CPOL);
	
	// Отправка данных
	uint8_t spi_Send(TSPI Phy, uint8_t Data);
	
    extern const TDriver spi;
	
#endif
