// ***********************************************************
// **                 Thoto OS internal                     **
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _THOTO_INTERNAL_H
#define _THOTO_INTERNAL_H

    // Биты регистра CONTROL
    #define CONTROL_SPSEL (1UL << 1) // Включение регистра PSP

    // Код статуса
    typedef uint32_t STATUS;

    // Установить указатель стека процесса, регистр PSP
    // Аргументы:
    //  Address - новый указатель стека.
    // Результат: нет
    void SetProcessSP(uint32_t Address);

    // Установить разделение стека между режимами работы
    // Аргументы: нет
    // Результат: нет
    void EnableProcessSP(void);

    // *******************************************************
    // Статус ОС
    // *******************************************************
    
    // Установить код статуса (ошибки)
    // Аргумент: 1
    //  Code: код статуса
    // Результат: нет
    void SetLastStatus(STATUS Code);

    // Получить код статуса (ошибки)
    // Аргумент: нет
    // Результат: код статуса
    STATUS GetLastStatus(void);


#endif
