// ***********************************************************
//	drivers.h
// 	Инициализация всей необходимой периферии v2.0.0
//
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _DRIVERS_H
#define _DRIVERS_H

    // Драйвер
    #include "drivers/driver.h"

    // Разрешение и запрет прерываний, перезагрузка
    #include "drivers/system.h"

    // Тактирование
    #include "drivers/clock.h"

    // Работа с энергонезависимой памятью
    #include "drivers/flash.h"

    // Монитор активности
    #include "drivers/activity.h"

    // Выделение памяти
    #include "drivers/memory.h"
    
    // GPIO
    #include "drivers/gpio.h"
    
    // EXTI
    #include "drivers/exti.h"
    
	// Таймеры аппаратные
    #include "drivers/timer.h"
	
    // DMA
    #include "drivers/dma.h"
    
    // АЦП
    #include "drivers/adc.h"
    
    // ЦАП
    #include "drivers/dac.h"
    
    // ШИМ
    #include "drivers/ledpwm.h"
    #include "drivers/ledfastpwm.h"
	
    // Последовательный асинхронный порт
    #include "drivers/uart.h"
    
    // Системный таймер
    #include "drivers/systick.h"
    
	// I2C
	#include "drivers/i2c.h"
	
	// I2C Slave
	#include "drivers/i2c_slave.h"
	
	// I2C Master
	#include "drivers/i2c_master.h"
	
	// SPI
	#include "drivers/spi.h"
	
	// Touch
	#include "drivers/touch.h"
	
	// Software UART receiver
	#include "drivers/softuartrx.h"
	
	// Software UART receiver
	#include "drivers/uuid.h"
	
    // BSP
    #include "bsp.h"
    
    // Приложения
    #include "app/app.h"
    
#ifdef THOTO
	#include "thoto/thoto.h"
#endif

    // Инициализация драйверов
    void drivers_Init(void);
    
    // Для "тяжёлого" кода драйверов, вызывается в главном цикле
    void drivers_Main(void);

    // Загрузить драйвера по списку
    void drivers_Load(const TDriver ** List);

    // Программная задержка
    void delay(uint32_t Value);

#endif
