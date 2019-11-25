// ***********************************************************
// **                 Thoto OS events                       **
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _THOTO_EVENTS_H
#define _THOTO_EVENTS_H

	typedef struct
    {
        bool     Active;
            
        uint32_t EventID;
        uint32_t ID;
        
        uint32_t Arg;
    } _EVENT;
    
    typedef enum
    {
        EVENT_DELAY = 0x0001,
    } _SYSTEMEVENTS;
    
    // Инициализировать таблицу событий
    // Аргументы: нет
    // Результат: нет
    void InitEventTable(void);
        
    // Добавить событие для ожидания
    // Аргументы: 3
    //  EventID: Глобальный тип события
    //  ID: Номер подсобытия
    // Результат: указатель на событие
    _EVENT * AddEvent(uint32_t EventID, uint32_t ID, uint32_t Arg);
    
#endif
