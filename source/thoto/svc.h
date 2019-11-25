// ***********************************************************
// **                Thoto OS Supervisor                    **
// ***********************************************************

#include <stdint.h>
#include "thoto.h"

// Программное прерывание

#ifndef _SVC_H
#define _SVC_H

    // Свойства функций
    #define SVC_F_RESULT       (1UL << 0) // Записи результата функции в R0 процесса
    #define SVC_F_FORCE_SWITCH (1UL << 1) // Сменить контекст

    // Таблица системных вызовов.
    typedef struct
    {
        void * Handler; // Указатель на функцию
        uint32_t Flags; // Свойства
    } SVCFUNC;

    // Автоматическая нумерация системных вызовов
    typedef enum
    {
        SVC_DELAY,
        SVC_CREATEPROCESS,
        SVC_EXITPROCESS,
        SVC_CURRENTPROCESS,
        SVC_WAIT,
        SVC_EVENT,
        SVC_ASYNCEVENT,
        SVC_ASYNCRETURN,
        SVC_PCALLOC,
        SVC_PFREE,
        SVC_LASTITEM // для подсчёта количества функций
    } SVC_NUMBERS;

    void svc_delay(uint32_t Milliseconds);
    PROCESS svc_createprocess(char * Name, TThotoProcess Entry, uint32_t Flags, uint32_t Argument);
    void svc_exitprocess(uint32_t Code);
    PROCESS svc_currentprocess(void);
    void * svc_wait(uint32_t EventID, uint32_t ID);
    void svc_event(uint32_t EventID, uint32_t ID, void * Arg);
    void svc_asyncevent(uint32_t EventID, uint32_t ID, TThotoEvent Handler);
    void svc_asyncreturn(void);
    void * svc_pcalloc(size_t num, size_t size);
    void svc_pfree(void * ptr);
    
#endif
