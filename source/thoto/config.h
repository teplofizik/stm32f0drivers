// ***********************************************************
// **                    Thoto OS config                    **
// ***********************************************************

#ifndef _THOTO_CONFIG_H
#define _THOTO_CONFIG_H

    // Частота ядра
    #define F_CPU                                 48000000UL

    // Количество процессов максимальное
    #define THOTO_PROCESS_COUNT                             2
    
    // Количество асинхронных слушателей событий максимальное
    #define THOTO_ASYNC_EVENT_COUNT                         10
    
    // Количество указателей максимальное
    #define THOTO_POINTER_COUNT                             10
    
    // Количество слушателей событий максимальное
    #define THOTO_EVENT_COUNT                               10
    
    // Размер памяти для одного процесса (2^x)
    // 9: 2^10 = 1 кБ
    #define THOTO_PROCESS_STACK_SIZE                        10
    
    // Частота системного таймера (Гц)
    #define THOTO_SYSTICK_FREQ                            SYSTEM_FREQUENCY // "systick.h"

    // Размер стека прерываний
    #define INTERRUPT_STACK_SIZE                          300

    // *******************************************************
    // Рассчитанные величины
    // *******************************************************
    
    // Коэффициент умножения в пересчёте на микросекунды
    // Т.е. микросекунд за один период таймера
    #define THOTO_DELAY_COEFF    (THOTO_SYSTICK_FREQ / 1000UL)

    // Размер блока памяти процесса, байт
    #define THOTO_PROCESS_STACK_SIZE_BYTES (1 << THOTO_PROCESS_STACK_SIZE)

    // Состояние регистра CPSR при запуске процесса
    #define DEFAULT_CPSR                            0x01000000 // бит T

#endif
