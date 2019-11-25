// ***********************************************************
// **                    Thoto OS API                       **
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../drivers/driver.h"

#ifndef _THOTO_H
#define _THOTO_H

    // Событие
    typedef uint32_t EVENT;
    
    // Идентификатор процесса
    typedef uint32_t PROCESS;

    // Прототип процесса
    typedef void (* TThotoProcess)(uint32_t Argument);
    
    // Прототип обработчика события
    typedef void (* TThotoEvent)(PROCESS P, void * Args);

    // Задержка в миллисекундах
    void    delay_ms(uint32_t Milliseconds);

    // Запуск процесса
    PROCESS createprocess(char * Name, TThotoProcess Entry, uint32_t Argument);

    // ID текущего процесса
    PROCESS currentprocess(void);
    
    // Завершение процесса с кодом...
    void    exitprocess(uint32_t Code);

    // Ожидание события
    void *  wait(uint32_t EventID, uint32_t ID);

    // Генерация события
    void    event(uint32_t EventID, uint32_t ID, void * Arg);
    
    // Добавить асинхронный слушатель события
    void asyncevent(uint32_t EventID, uint32_t ID, TThotoEvent Handler);
    
    // Выделить блок памяти, привязанный к процессу
    void * pcalloc(size_t num, size_t size);
    
    // Удалить блок памяти, привязанный к процессу
    void pfree(void * ptr);
    
	extern const TDriver thoto;

#endif
