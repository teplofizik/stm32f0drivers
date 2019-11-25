// ***********************************************************
// **                 Thoto OS processes                    **
// ***********************************************************

#include <stdint.h>
#include "thoto.h"
#include "event.h"
#include "config.h"

#ifndef _THOTO_PROCESS_H
#define _THOTO_PROCESS_H

    // Флаги:
    #define PROC_F_PAUSE   (1UL << 30) // Процесс приостановлен по какой-то причине
    
    #define PROC_F_WAIT    (1UL << 1)  // Ожидание события
    #define PROC_F_ERROR   (1UL << 2)  // Ошибка
    
    typedef struct
    {
        _EVENT     * Event;
        TThotoEvent  Handler;
        
        uint32_t     Active; // TProcess *
    } ASYNCEVENT;
    
    // Процесс, описание
    typedef struct
    {
        // Контекст
        // Эти структуры расположены в начале, чтобы не перерасчитывать их расположение
        // в ассемблерном коде
        uint32_t   SP;         // Указатель стека
        uint32_t   Regs[8];    // Содержимое старших регистров R4-R11
        
        // Свойства
        PROCESS    PID;        // Уникальный номер процесса
        uint32_t   Flags;      // Флаги времени выполнения
        char *     Name;       // Название процесса
        EVENT      Event;      // Поле события
        
        ASYNCEVENT AEvents[THOTO_ASYNC_EVENT_COUNT]; // Поля для добавления асинхронных событий
        
		int        AEventWait;
		bool       AEventActive;
        uint32_t   Allocated;
        uint32_t   Pointer[THOTO_POINTER_COUNT];
        uint16_t   MemSize[THOTO_POINTER_COUNT];
    } _PROCESS;
    
    // Регион памяти процесса
    typedef struct
    {
        uint8_t Data[THOTO_PROCESS_STACK_SIZE_BYTES];  
    } PROCESSBLOCK;

    // Описание расположения регистров в стеке после входа в прерывание
    typedef struct
    {
        uint32_t  R0;  // Регистры общего назначения
        uint32_t  R1;
        uint32_t  R2;
        uint32_t  R3;
        
        uint32_t  R12;
        
        uint32_t  LR;   // Регистр с адресом возврата
        uint32_t  PC;   // Регистр-указатель команды
        
        uint32_t  CPSR; // Регистр статуса
    } _STACKFRAME;
    
#ifdef THOTO_USE_FPU
    // Описание расположения регистров в стеке после входа в прерывание
    // при включённом FPU
    typedef struct
    {
        uint32_t  R0;   // Регистры общего назначения
        uint32_t  R1;
        uint32_t  R2;
        uint32_t  R3;
        
        uint32_t  R12;
        
        uint32_t  LR;    // Регистр с адресом возврата
        uint32_t  PC;    // Регистр-указатель команды
        
        uint32_t  CPSR;  // Регистр статуса
        
        uint32_t  S[16]; // 16 регистров FPU
        uint32_t  FPSCR; // Регистр статуса
        uint32_t  Reserved; // Для чётности

    } _STACKFRAMEFPU;
#endif
    
    typedef enum
    {
        HARDFAULT_SOURCE // Ошибка Hard Fault
    } ERRORSOURCES;
    
    // Количество процессов
    extern const uint32_t ProcessCount;
    
    // Текущий процесс
    extern volatile uint32_t CurrentProcess;
    
    // Таблица процессов
    extern _PROCESS ProcessTable[THOTO_PROCESS_COUNT];
    
    // Посчитать расположение вершины стека после выхода из прерывания и из асинхронного метода
    // Аргументы: 1
    //  P: запись о процессе
    // Результат: нет
    void CalculateStackBottom(_PROCESS * P);
     
    // Посчитать расположение вершины стека после входа в прерывание согласно режиму
    // Аргументы: 1
    //  P: запись о процессе
    // Результат: нет
    void CalculateStackTop(_PROCESS * P);
        
    // Установить состояние стека процесса
    // Аргументы: 3
    //  SP: указатель вершины стека, куда записываем состояние
    //  Entry: точка входа в процесс
    //  Argument: аргумент
    // Результат: нет
    void SetupStack(uint32_t SP, uint32_t LR, uint32_t Entry, uint32_t A1, uint32_t A2);

    // Инициализировать таблицу процессов
    // Аргументы: нет
    // Результат: нет
    void InitProcessTable(void);
    
    // Сбросить контекст процесса на начальные значения
    // Аргументы: 1
    //  ProcessSlot: номер слота, в котором расположен процесс
    // Результат: нет
    void DefaultProcessContext(uint32_t ProcessSlot);
    
    // Добавить процесс
    // Аргументы: 4
    //  Name: название
    //  Entry: точка входа
    //  Flags: параметры создания процесса
    //  Argument: параметр, будет передан как аргумент в процесс
    // Результат: код ошибки
    PROCESS _CreateProcess(char * Name, TThotoProcess Entry, uint32_t Flags, uint32_t Argument);

    // Завершить процесс
    // Аргументы: 1
    //  P: указатель на таблицу процесса
    // Результаты: нет
    void _FinalizeProcess(_PROCESS * P);

    // Получить номер слота текущего процесса
    // Аргументы: нет
    // Результаты: номер слота
    uint32_t _GetCurrentProcess(void);
    
    // Получить регистр РС процесса
    // Аргументы: 1
    //  P: указатель на таблицу процесса
    // Результаты: PC
    uint32_t _GetProcessPC(_PROCESS * P);

    // Получить регистр LR процесса
    // Аргументы: 1
    //  P: указатель на таблицу процесса
    // Результаты: LR
    uint32_t _GetProcessLR(_PROCESS * P);

    // Сохранить номер слота текущего процесса
    // Аргументы: 1
    //  Slot: номер слота
    // Результаты: нет
    void _SetCurrentProcess(uint32_t Slot);

    // Указатель на таблицу процессов
    // Аргументы: 1
    //  Slot: номер слота
    // Результаты: указатель на запись таблицы
    _PROCESS * _GetProcess(uint32_t Slot);

    // Указатель на таблицу процессов
    // Аргументы: 1
    //  pid: идентификатор процесса
    // Результаты: указатель на запись таблицы
    _PROCESS * _GetProcessByID(PROCESS pid);

    // Добавить в таблицу указатель на выделенную память
    // Аргументы: 2
    //  P: указатель на таблицу процессов
    //  Pointer: указатель на память
    // Результаты: указатель на запись таблицы
    void _AddPointer(_PROCESS * P, uint32_t Pointer, uint32_t Size, uint32_t PC);
    
    // Удалить в таблице указатель на выделенную память
    // Аргументы: 2
    //  P: указатель на таблицу процессов
    //  Pointer: указатель на память
    // Результаты: указатель на запись таблицы
    void _DelPointer(_PROCESS * P, uint32_t Pointer);

    // Процедура-заглушка для процесса
    // core.s
    void ProcessStub(void);

    // Обработать ошибку процесса
    // Аргументы: 1
    //  Error: источник ошибки (0 - HF)
    //  PC: адрес команды, где возникла ошибка
    // Результат: нет
    void HandleProcessError(uint32_t Error, uint32_t PC);

#endif
