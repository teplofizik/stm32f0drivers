// ***********************************************************
// **                 Thoto OS processes                    **
// ***********************************************************

#include "process.h"
#include "internal.h"
#include "error_codes.h"
#include <string.h>
#include <stdio.h>
#include <stm32f0xx.h> // SCB reg

// Таблица процессов
_PROCESS ProcessTable[THOTO_PROCESS_COUNT];

// Блоки памяти процессов
__align(THOTO_PROCESS_STACK_SIZE_BYTES) PROCESSBLOCK ProcessBlock[THOTO_PROCESS_COUNT];

// Количество процессов
const uint32_t ProcessCount = THOTO_PROCESS_COUNT;

uint32_t AllocatedMemory = 0;

// Счётчик PID для назначения новых.
volatile uint32_t PIDCounter = 1;

// Текущий процесс
volatile uint32_t CurrentProcess = 0;
// Когда CurrentProcess == 0, то это основной код

// Сбросить контекст процесса на начальные значения
// Аргументы: 1
//  ProcessSlot: номер слота, в котором расположен процесс
// Результат: нет
void DefaultProcessContext(uint32_t ProcessSlot)
{
    int i;
    _PROCESS * P = &ProcessTable[ProcessSlot];
    
    // Указатель стека в конец блока
    P->SP = (uint32_t)&(ProcessBlock[ProcessSlot].Data[THOTO_PROCESS_STACK_SIZE_BYTES]);
    
    // Сбросить содержимое старших регистров
    for(i = 0; i < 8; i++) P->Regs[i] = 0;
}

// Установить состояние стека процесса
// Аргументы: 3
//  SP: указатель вершины стека, куда записываем состояние
//  Entry: точка входа в процесс
//  Argument: аргумент
// Результат: нет
void SetupStack(uint32_t SP, uint32_t LR, uint32_t Entry, uint32_t A1, uint32_t A2)
{
    _STACKFRAME * S = (_STACKFRAME *)SP;
    
    // Сохранить аргумент процесса
    S->R0   = A1;
    S->R1   = A2;
    S->R2   = 0;
    S->R3   = 0;
    S->R12  = 0;
    
    S->CPSR = DEFAULT_CPSR; // обычно тут только бит режима T
    
    S->PC = Entry;
    S->LR = LR;
}

// Посчитать расположение вершины стека после входа в прерывание согласно режиму
// Аргументы: 1
//  P: запись о процессе
// Результат: нет
void CalculateStackTop(_PROCESS * P)
{
    // TODO: написать атоматический определятор
#ifndef THOTO_USE_FPU
    P->SP -= sizeof(_STACKFRAME);
#else
    P->SP -= sizeof(_STACKFRAMEFPU);
#endif
}

// Посчитать расположение вершины стека после выхода из прерывания и из асинхронного метода
// Аргументы: 1
//  P: запись о процессе
// Результат: нет
void CalculateStackBottom(_PROCESS * P)
{
    // TODO: написать атоматический определятор
    P->SP += sizeof(_STACKFRAME);
}

// Инициализировать таблицу процессов
// Аргументы: нет
// Результат: нет
void InitProcessTable(void)
{
    int i;
    
    PIDCounter = 0;
    
    // Очистка таблицы процессов как гарантия того, что там не окажется 
    // чего-нибудь лишнего
    for(i = 0; i < ProcessCount; i++)
    {
        _PROCESS * P = &ProcessTable[i];
        
        P->PID = 0;
        P->Flags = 0;
        P->Name = 0;
        P->Event = 0;
        
        // Контекст процесса сбросится при добавлении процесса
    }
}

// Добавить процесс
// Аргументы: 4
//  Name: название
//  Entry: точка входа
//  Flags: параметры создания процесса
//  Argument: параметр, будет передан как аргумент в процесс
// Результат: код ошибки
PROCESS _CreateProcess(char * Name, TThotoProcess Entry, uint32_t Flags, uint32_t Argument)
{
    int i;
    _PROCESS * P = 0;
        
    // Сначала найдём незанятый слот:
    for(i = 1; i < ProcessCount; i++)
    {
        P = &ProcessTable[i];
        
        if(P->PID == 0) break;
        
        // Сбросим указатель, как факт того, что он занят
        P = 0;
    }
    
    // Если не нашли, вернём 0
    if(!P)
    {
        SetLastStatus(ERROR_CORE_TOO_MANY_PROCESSES);
        return 0;
    }
    
    // Сбросим контекст
    DefaultProcessContext(i);
    
    // Изменить SP согласно режиму (в разных режимах работы сохраняется на стек
    // разное количество регистров
    CalculateStackTop(P);
    
    // Инициализируем сохранённый на стеке контекст
    SetupStack(P->SP, (uint32_t)&ProcessStub, (uint32_t)Entry, Argument, 0);
    
    // Назначим PID
    P->PID  = (++PIDCounter);
    // Сохраним название
    P->Name = Name;
	
	// Не ждём событий
	P->AEventWait = false;
	P->AEventActive = false;
    
    // Сбросим служебные поля
    P->Flags = 0;
    P->Event = 0;
    P->Allocated = 0;
    
    memset(&P->AEvents[0], 0, sizeof(P->AEvents));
    memset(&P->Pointer[0], 0, sizeof(P->Pointer));
    memset(&P->MemSize[0], 0, sizeof(P->MemSize));
    
    SetLastStatus(ERROR_SUCCESS);
    return P->PID;
}

// Завершить процесс
// Аргументы: 1
//  P: указатель на таблицу процесса
// Результаты: нет
void _FinalizeProcess(_PROCESS * P)
{
    int i;
	for(i = 0; i < THOTO_ASYNC_EVENT_COUNT; i++)
	{
		if(P->AEvents[i].Event)
			memset(P->AEvents[i].Event, 0, sizeof(_EVENT));
	}
    for(i = 0; i < THOTO_POINTER_COUNT; i++)
    {
        if(P->Pointer[i])
        {
//            printf("Free memory: %08X (%d b)\n", P->Pointer[i], P->MemSize[i]);
            free((void *)P->Pointer[i]);
            break;
        }
    }
    memset(P, 0, sizeof(_PROCESS));
}


// Получить регистр РС процесса
// Аргументы: 1
//  P: указатель на таблицу процесса
// Результаты: PC
uint32_t _GetProcessPC(_PROCESS * P)
{
    _STACKFRAME * S = (_STACKFRAME *)P->SP;
    
    return S->PC;
}

// Получить регистр LR процесса
// Аргументы: 1
//  P: указатель на таблицу процесса
// Результаты: LR
uint32_t _GetProcessLR(_PROCESS * P)
{
    _STACKFRAME * S = (_STACKFRAME *)P->SP;
    
    return S->LR;
}

// Получить номер слота текущего процесса
// Аргументы: нет
// Результаты: номер слота
uint32_t _GetCurrentProcess(void)
{
    return CurrentProcess;
}

// Сохранить номер слота текущего процесса
// Аргументы: 1
//  Slot: номер слота
// Результаты: нет
void _SetCurrentProcess(uint32_t Slot)
{
    CurrentProcess = Slot;
}

// Указатель на таблицу процессов
// Аргументы: 1
//  Slot: номер слота
// Результаты: указатель на запись таблицы
_PROCESS * _GetProcess(uint32_t Slot)
{
    return &ProcessTable[Slot];
}

// Указатель на таблицу процессов
// Аргументы: 1
//  pid: идентификатор процесса
// Результаты: указатель на запись таблицы
_PROCESS * _GetProcessByID(PROCESS pid)
{
    int i;
    
    for(i = 0; i < ProcessCount; i++)
    {
        _PROCESS * P = &ProcessTable[i];
        
        if(P->PID == pid) return P;
    }
    
    // Не найдено
    return 0;
}

typedef struct
{
    uint32_t CFSR; // Consists of MMSR, BFSR and UFSR
    uint32_t HFSR; // Hard Fault Status Register
    uint32_t DFSR; // Debug Fault Status Register
    uint32_t AFSR; // Auxiliary Fault Status Register
    uint32_t MMAR; // MemManage Fault Address Register
    uint32_t BFAR; // Bus Fault Address Register
    
    uint32_t SHCSR; // System handler control and state register
} TFaultRegs;

volatile TFaultRegs FaultRegs;

// Обработать ошибку процесса
// Аргументы: 1
//  Error: источник ошибки (0 - HF)
//  PC: адрес команды, где возникла ошибка
// Результат: нет
void HandleProcessError(uint32_t Error, uint32_t PC)
{
    _PROCESS * P = _GetProcess(_GetCurrentProcess());
    
    // Установить флаги завершения процесса
    P->Flags |= PROC_F_PAUSE | // Процесс остановлен
                PROC_F_ERROR | // Причина остановки: ошибка
                0;
    
    // Запомнить адрес ошибки
    P->Event = PC;
    
    FaultRegs.CFSR = *((volatile uint32_t *)(0xE000ED28));
    FaultRegs.HFSR = *((volatile uint32_t *)(0xE000ED2C));
    FaultRegs.DFSR = *((volatile uint32_t *)(0xE000ED30));
    FaultRegs.MMAR = *((volatile uint32_t *)(0xE000ED34));
    FaultRegs.BFAR = *((volatile uint32_t *)(0xE000ED38));
    FaultRegs.AFSR = *((volatile uint32_t *)(0xE000ED3C));
    FaultRegs.SHCSR = SCB->SHCSR;
    
//    printf("Process %d stopped, error 0x%08X at 0x%08X\n", P->PID, Error, PC);
}

// Добавить в таблицу указатель на выделенную память
// Аргументы: 2
//  P: указатель на таблицу процессов
//  Pointer: указатель на память
// Результаты: указатель на запись таблицы
void _AddPointer(_PROCESS * P, uint32_t Pointer, uint32_t Size, uint32_t PC)
{
    int i;
    for(i = 0; i < THOTO_POINTER_COUNT; i++)
    {
        if(P->Pointer[i] == 0)
        {
			AllocatedMemory += Size;
            P->Allocated += Size;
            P->Pointer[i] = Pointer;
            P->MemSize[i] = Size;
            break;
        }
    }
}

// Удалить в таблице указатель на выделенную память
// Аргументы: 2
//  P: указатель на таблицу процессов
//  Pointer: указатель на память
// Результаты: указатель на запись таблицы
void _DelPointer(_PROCESS * P, uint32_t Pointer)
{
    int i;
    for(i = 0; i < THOTO_POINTER_COUNT; i++)
    {
        if(P->Pointer[i] == Pointer)
        {
			AllocatedMemory -= P->MemSize[i];
            P->Allocated -= P->MemSize[i];
            P->Pointer[i] = 0;
            P->MemSize[i] = 0;
            break;
        }
    }
}
