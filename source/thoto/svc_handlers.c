// ***********************************************************
// **                Thoto OS Supervisor                    **
// ***********************************************************

// Обработчики системных вызовов
#include "../drivers.h"
#include "internal.h"
#include "process.h"
#include "event.h"
#include "config.h"
#include "error_codes.h"
#include "svc.h"
#include <stdio.h>

// Задержка
void svc_delay(uint32_t Milliseconds)
{
    _PROCESS * P = _GetProcess(_GetCurrentProcess());
    
    // Процесс приостановлен, процесс ожидает завершения счёта
    P->Flags |= PROC_F_PAUSE | 
                PROC_F_WAIT |
                0;
    
    P->Event = (uint32_t)AddEvent(EVENT_DELAY, 0, Milliseconds * THOTO_DELAY_COEFF);
}

// Запуск процесса
PROCESS svc_createprocess(char * Name, TThotoProcess Entry, uint32_t Flags, uint32_t Argument)
{
    PROCESS id = _CreateProcess(Name, Entry, Flags, Argument);
    
    // + обработка флагов запуска, не связанных с запуском
    
    return id;
}

// Завершние процесса
void svc_exitprocess(uint32_t Code)
{
    _PROCESS * P = _GetProcess(_GetCurrentProcess());
    
    // Можно известить систему о завершении процесса
    _FinalizeProcess(P);
}

// ID текущего процесса
PROCESS svc_currentprocess(void)
{
    _PROCESS * P = _GetProcess(_GetCurrentProcess());
    
    return P->PID;
}

// Ожидание события
void * svc_wait(uint32_t EventID, uint32_t ID)
{
    _PROCESS * P = _GetProcess(_GetCurrentProcess());
        
    P->Event = (uint32_t)AddEvent(EventID, ID, 0);
    if(P->Event)
    {
        // Процесс приостановлен, процесс ожидает события
        P->Flags |= PROC_F_PAUSE | 
                    PROC_F_WAIT |
                    0;
    
    }
    return 0;
}

// Генерация события
void svc_event(uint32_t EventID, uint32_t ID, void * Arg)
{
    int i, j;
    for(i = 0; i < THOTO_PROCESS_COUNT; i++)
    {
        _PROCESS * P = &ProcessTable[i];
        if(P->Event)
        {
            _EVENT * E = (_EVENT *)P->Event;
            
            if((E->EventID == EventID) && (E->ID == ID))
            {
                // Обновим R0 перед продолжением работы
                _STACKFRAME * S = (_STACKFRAME *)P->SP;
                
                S->R0 = (uint32_t)Arg;
                
                // Empty event
                E->Active = 0;
                E->EventID = 0;
                E->ID = 0;
                E->Arg = 0;

                // Задержка истекла
                P->Event = 0;
                
                // Флаг задержки снять, процесс возобновить
                P->Flags &= ~(PROC_F_WAIT | PROC_F_PAUSE);
            }
        }
        
        for(j = 0; j < THOTO_ASYNC_EVENT_COUNT; j++)
        {
            ASYNCEVENT * AE = &P->AEvents[j];
            
            if(!AE->Event) break;
            
            if((AE->Event->EventID == EventID) && (AE->Event->ID == ID))
            {
                AE->Event->Arg = (uint32_t)Arg;
                AE->Active = (uint32_t)_GetProcess(_GetCurrentProcess());
				P->AEventWait++;
                break;
            }
        }
    }
}

// Ожидание события
void svc_asyncevent(uint32_t EventID, uint32_t ID, TThotoEvent Entry)
{
    _PROCESS * P = _GetProcess(_GetCurrentProcess());
    int i;
    
    for(i = 0; i < THOTO_ASYNC_EVENT_COUNT; i++)
    {
        ASYNCEVENT * AE = &P->AEvents[i];
        if(AE->Event == 0)
        {
            // Определим фактический адрес обработчика, виртуальный ли это или не виртуальный
            AE->Event = AddEvent(EventID, ID, 0);
            AE->Handler = Entry;
                
            AE->Active = false;//(uint32_t)P;
            break;
        }
    }
}

// Ожидание события
void svc_asyncreturn()
{
    _PROCESS * P = _GetProcess(_GetCurrentProcess());
    
    CalculateStackBottom(P);
	SetProcessSP(P->SP);
	P->AEventActive = false;
}

void * svc_pcalloc(size_t num, size_t size)
{
    _PROCESS * P = _GetProcess(_GetCurrentProcess());
    
    void * ptr = calloc(num, size);
    
    if(ptr)
    {
#ifdef DEBUG_CALLOC
    _AddPointer(P, (uint32_t)ptr, num*size, _GetProcessLR(P));
#else
    _AddPointer(P, (uint32_t)ptr, num*size, 0);
#endif
    //printf("calloc(%d): %08X, %d\n", P->PID, (uint32_t)ptr, size*num);
    }
    else
    {
		while(1) {}
//        printf("calloc(%d): %d bytes alloc fail from 0x%08X\n", P->PID, size*num, _GetProcessLR(P));
//        printf("total allocated memory: %d bytes\n", P->Allocated);
    }
    return ptr;
}

void svc_pfree(void * ptr)
{
    _PROCESS * P = _GetProcess(_GetCurrentProcess());
    
    _DelPointer(P, (uint32_t)ptr);
    //printf("free(%d): %08X\n", P->PID, (uint32_t)ptr);
    free(ptr);
}
