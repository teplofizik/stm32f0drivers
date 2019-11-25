// ***********************************************************
//	app.h
// 	Базовый код для работы с приложениями
//
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _APP_H
#define _APP_H

    #define MAX_APP_COUNT 5

    typedef struct
    {
        // Название приложения
        const char  * Name;
        
        // Список необходимых драйверов
        const char ** Required;
        
        // Запуск приложения - если false, то не загрузилось (не добавляем в список)
        bool (* Init)(void);
        // Выгрузка приложения
        void (* Uninit)(void);
        // Основной цикл - если false, то выгружаем из-за ошибки.
        bool (* Main)(void);
    } TApp;

	// Инициализация подсистемы приложений
    void app_Init(void);
    
	// Основной цикл для программ
    void app_Main(void);

    // Загрузить приложение
    bool app_Start(const TApp * App);

    // Остановить приложение
    void app_Stop(const TApp * App);
    
#endif
