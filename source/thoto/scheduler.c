// ***********************************************************
// **                   Thoto OS Scheduler                  **
// ***********************************************************

#include "config.h"
#include "event.h"
#include "internal.h"
#include "process.h"

#include <stm32f0xx.h>

#define CONTEXT_SYSTICK_HANDLER 0

// Выход из события (internal svc)
void asyncreturn(void);
    
void BackFromEvent(void)
{
    asyncreturn();
}

// Выбран контекст. Настройка ОС
// Аргументы: 1
//  Slot: номер записи выбранного процесса
// Результат: нет
void SelectContext(uint32_t Slot)
{
    _PROCESS * P = _GetProcess(Slot);
    int i;
    
    // Проверим асинхронные методы
	if(P->AEventWait && !P->AEventActive)
	{
		for(i = 0; i < THOTO_ASYNC_EVENT_COUNT; i++)
		{
			ASYNCEVENT * AE = &P->AEvents[i];
				
			if(!AE->Event) break;
				
			if(AE->Active)
			{
				CalculateStackTop(P);
				// (TProcess * P, EventArgs * E)
				SetupStack(P->SP, (uint32_t)&BackFromEvent, (uint32_t)AE->Handler, AE->Active, (uint32_t)AE->Event->Arg);
				AE->Active = 0;
				P->AEventActive = true;
				P->AEventWait--;
				break;
			}
		}
	}
    
    // Сохраним номер выбранного процесса
    _SetCurrentProcess(Slot);
}

// systick.h
void Systick_Timer(void);

// Вызывается из диспетчера перед сменой контекста
//  * Обработка программных задержек
// Аргументы: нет
// Результат: нет
void SysTick_User(void)
{
    int i;
    
    Systick_Timer();
    for(i = 0; i < ProcessCount; i++)
    {
        // Проверим задержки
        _PROCESS * P = &ProcessTable[i];
        
        // Есть программная задержка
        if(P->Flags & PROC_F_WAIT)
        {
            _EVENT * E = (_EVENT *)P->Event;
            if(E->EventID == EVENT_DELAY)
            {
                if(E->Arg-- == 0)
                {
                    // Empty event
                    E->Active = false;
                    E->EventID = 0;
                    E->ID = 0;
                    E->Arg = 0;

                    // Задержка истекла
                    P->Event = 0;
                    
                    // Флаг задержки снять, процесс возобновить
                    P->Flags &= ~(PROC_F_WAIT | PROC_F_PAUSE);
                }
            }
        }
    }
}

// Сохранить дополнительные параметры процесса
void ContextSaveEx(void)
{
    //_PROCESS * P = _GetProcess(_GetCurrentProcess());
}

// Переключение контекста
// Аргументы: инициатор переключения, номер
// Результат: нет
void ContextSwitch(uint32_t Source)
{
    int i, Selected;
    
    // Если прерывания системного таймера отключены, то выходим.
    if(!(SysTick->CTRL & SysTick_CTRL_TICKINT_Msk)) return;
    
    ContextSaveEx();
    
    // Узнаём текущиё активный процесс
    Selected = _GetCurrentProcess();
    
    if(Selected == 0)
    {
        // Ищем следующий активный процесс
        for(i = Selected + 1; i < ProcessCount; i++)
        {
            _PROCESS * P = &ProcessTable[i];
            
            // Idle: если не найдётся иного
            if(i == 0) continue;
            
            // Если процесс не запущен, пропускаем
            if(P->PID == 0) continue;
            
            // Если процесс не активен, пропускаем.
            if(P->Flags & PROC_F_PAUSE) 
				if(!(P->AEventWait || P->AEventActive))
					continue;
            
            // Процесс выбран.
            SelectContext(i);
            
            return;
        }
        
        // С начала списка до текущего
        for(i = 1; i <= Selected; i++)
        {
            _PROCESS * P = &ProcessTable[i];
            
            // Если процесс не запущен, пропускаем
            if(P->PID == 0) continue;
            
            // Если процесс не активен, пропускаем.
            if(P->Flags & PROC_F_PAUSE) 
				if(!(P->AEventWait || P->AEventActive))
					continue;
				
            // Процесс выбран.
            SelectContext(i);
            
            return;
        }
    }
        
    // Нет активных процессов. Выбираем основной процесс.
    SelectContext(0);
}
