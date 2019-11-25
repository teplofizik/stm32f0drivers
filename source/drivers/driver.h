// ***********************************************************
//	driver.h
// 	Базовый код для работы с драйверами
//
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

// Список поддерживаемого железа
#include "hw_avail.h"
// Стандартные типы
#include "../types.h"
	
#ifndef _DRIVER_H
#define _DRIVER_H

    #define MAX_DRIVER_COUNT 20

    typedef struct
    {
        const char * Name;
        
        // Инициализация - если false, то не загрузилось (не добавляем в список)
        bool (* Init)(void);
        // Выгрузка драйвера
        void (* Uninit)(void);
        // Основной цикл - если false, то выгружаем из-за ошибки.
        bool (* Main)(void);
    } TDriver;

	// Инициализация подсистемы драйверов
    void drv_Init(void);
    
	// Основной цикл для драйверов
    void drv_Main(void);

    // Загрузить драйвер
    void drv_Load(const TDriver * Drv);

    // Драйвер загружен?
    bool drv_IsLoaded(const char * name);
        
    // Требование наличия драйвера
    // Если не загружен - false
    bool drv_Require(const TDriver * Drv, const char * name);
    
    // Строка в лог
    void drv_DebugLog(const TDriver * Drv, const char * Text);
    
#endif
