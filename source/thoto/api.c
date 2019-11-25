// ***********************************************************
// **                    Thoto OS API                       **
// ***********************************************************

// Реализация вызовов SVC

#include "thoto.h"
#include "svc.h"

// Таблица функций
__svc(SVC_DELAY) void __delay(uint32_t Milliseconds);
__svc(SVC_CREATEPROCESS) PROCESS __createprocess(char * Name, TThotoProcess Entry, uint32_t Flags, uint32_t Argument);
__svc(SVC_EXITPROCESS) void __exitprocess(uint32_t Code);
__svc(SVC_CURRENTPROCESS) PROCESS __currentprocess(void);
__svc(SVC_WAIT)  void * __wait(uint32_t EventID, uint32_t ID);
__svc(SVC_EVENT)  void __event(uint32_t EventID, uint32_t ID, void * Arg);
__svc(SVC_ASYNCEVENT)  void __asyncevent(uint32_t EventID, uint32_t ID, TThotoEvent Handler);
__svc(SVC_ASYNCRETURN)  void __asyncreturn(void);
    
__svc(SVC_PCALLOC)  void * __pcalloc(size_t num, size_t size);
__svc(SVC_PFREE)  void __pfree(void * ptr);
    
// Задержка в миллисекундах
void delay_ms(uint32_t Milliseconds)
{
    __delay(Milliseconds);
}

// Запуск процесса
PROCESS createprocess(char * Name, TThotoProcess Entry, uint32_t Argument)
{
    return __createprocess(Name, Entry, 0, Argument);
}

// Завершние процесса
void exitprocess(uint32_t Code)
{
    __exitprocess(Code);
}

// ID текущего процесса
PROCESS currentprocess(void)
{
    return __currentprocess();
}

void * wait(uint32_t EventID, uint32_t ID)
{
    return __wait(EventID, ID);
}

void event(uint32_t EventID, uint32_t ID, void * Arg)
{
    __event(EventID, ID, Arg);
}

void asyncevent(uint32_t EventID, uint32_t ID, TThotoEvent Handler)
{
    __asyncevent(EventID, ID, Handler);
}

void asyncreturn(void)
{
    __asyncreturn();
}

void * pcalloc(size_t num, size_t size)
{
    return __pcalloc(num, size);
}

void pfree(void * ptr)
{
    __pfree(ptr);
}
