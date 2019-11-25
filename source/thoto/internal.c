// ***********************************************************
// **                 Thoto OS internal                     **
// ***********************************************************

#include "config.h"
#include "internal.h"
#include <stm32f0xx.h>

// Объявления служебных регистров
register uint32_t CONTROL __asm("control");
register uint32_t MSP     __asm("msp");
register uint32_t PSP     __asm("psp");

uint8_t MSPStack[INTERRUPT_STACK_SIZE];

// Код последней ошибки
volatile STATUS LastStatus;

// Установить указатель стека процесса, регистр PSP
// Аргументы:
//  Address - новый указатель стека.
// Результат: нет
void SetProcessSP(uint32_t Address)
{
    PSP = Address;
}

// Установить разделение стека между режимами работы
// Аргументы: нет
// Результат: нет
void EnableProcessSP(void)
{
    // Сохранить указатель стека (до начала работы диспетчера)
    PSP = MSP;
    MSP = (uint32_t)&MSPStack[INTERRUPT_STACK_SIZE];
    
    // Включить разделение стеков
    CONTROL |= CONTROL_SPSEL;
}

// Установить код статуса (ошибки)
// Аргумент: 1
//  Code: код статуса
// Результат: нет
void SetLastStatus(STATUS Code)
{
    LastStatus = Code;
}

// Получить код статуса (ошибки)
// Аргумент: нет
// Результат: код статуса
STATUS GetLastStatus(void)
{
    return LastStatus;
}
