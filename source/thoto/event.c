// ***********************************************************
// **                 Thoto OS events                       **
// ***********************************************************

#include "event.h"
#include "config.h"
#include <string.h>

_EVENT Events[THOTO_EVENT_COUNT];

// Инициализировать таблицу событий
// Аргументы: нет
// Результат: нет
void InitEventTable(void)
{
    memset(&Events[0], 0, sizeof(Events));
}

// Добавить событие для ожидания
// Аргументы: 3
//  EventID: Глобальный тип события
//  ID: Номер подсобытия
//  Arg: Данные
// Результат: нет
_EVENT * AddEvent(uint32_t EventID, uint32_t ID, uint32_t Arg)
{
    int i;
    for(i = 0; i < THOTO_EVENT_COUNT; i++)
    {
        if(!Events[i].Active)
        {
            Events[i].Active = true;
            Events[i].EventID = EventID;
            Events[i].ID = ID;
            Events[i].Arg = Arg;
            return &Events[i];
        }
    }
    return NULL;
}
