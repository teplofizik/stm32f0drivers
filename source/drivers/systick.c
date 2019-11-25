// ***********************************************************
//	systick.с
// 	Системный таймер v2.0.0
//
//  teplofizik, 2016
// ***********************************************************

#include "systick.h"
#include "clock.h"
#include "driver.h"
#include <stm32f0xx.h>
#include <string.h>

// Количество обработчиков максимальное
#define TIMER_HANDLERS    15

typedef struct 
{
	TEventHandler Handler;
	uint16_t      Countdown;
	uint16_t      Reload;
    
    // Запущено
    bool          Run;
    // Сработало
    bool          Fired;
} THandlers;

// Функции, кои будут вызываться
static THandlers Handlers[TIMER_HANDLERS];
static uint32_t  TimerFrequency;
static int       TimerCount;

bool             Error = false;

#ifdef THOTO
void Systick_Timer(void)
#else
void SysTick_Handler(void)
#endif
{
    int i;
    
	for(i = 0; i < TIMER_HANDLERS; i++)
	{
		if(Handlers[i].Run)
		{
			if(--Handlers[i].Countdown == 0)
			{
				Handlers[i].Countdown = Handlers[i].Reload;
		
				// Запишем, что сработало
				Handlers[i].Fired = true;
			}
		}
	}
}

// Запуск таймера с заданной частотой
static bool Init(void)
{
    uint32_t Frequency = SYSTEM_FREQUENCY;
    if(!Frequency) return false;
    
    // Сбросим параметры
    TimerCount = 0;
    Error = false;
    TimerFrequency = Frequency;
    memset(&Handlers[0], 0, sizeof(Handlers));
    
    // Настройка частоты
    SysTick_Config(clock_GetSYSCLK() / Frequency);
    
    // Разрешение прерывания
    NVIC_SetPriority(SysTick_IRQn, 15);
    
    return true;
}

// Завершение таймера
static void Uninit(void)
{
    Error = false;
    TimerFrequency = 0;
    SysTick->CTRL = 0;
    memset(&Handlers[0], 0, sizeof(Handlers));
}

// Добавить функцию в список вызова, Handler будет вызываться с заданной частотой
void timer_AddFunction(uint16_t Frequency, TEventHandler Handler)
{
    if(TimerFrequency)
    {
        int i;
        
        for(i = 0; i < TIMER_HANDLERS; i++)
        {
            // Найдём пустой слот
            if(!Handlers[i].Handler)
            {
                // Обработчик, частота опроса
                Handlers[i].Run       =  true;
                Handlers[i].Fired     =  false;
                Handlers[i].Handler   =  Handler;
                Handlers[i].Reload    = (TimerFrequency / Frequency);
                Handlers[i].Countdown =  Handlers[i].Reload;
                
                TimerCount++;
                
                return;
            }
        }
        
        drv_DebugLog(&systimer, "No enough free timers in driver '%s'\n");
        Error = true;
    }
}

// Изменить частоту таймера
void timer_ChangeFrequency(TEventHandler Handler, uint16_t Frequency)
{
    if(TimerFrequency)
    {
        int i;
        for(i = 0; i < TIMER_HANDLERS; i++)
        {
            if(Handlers[i].Handler == Handler)
            {
                Handlers[i].Reload = (TimerFrequency / Frequency);
                Handlers[i].Countdown = Handlers[i].Reload;
                break;
            }
        }
    }
}

// Удалить таймер
void timer_Remove(TEventHandler Handler)
{
    if(TimerFrequency)
    {
        int i;
        
        for(i = 0; i < TIMER_HANDLERS; i++)
        {
            if(Handlers[i].Handler == Handler)
            {
                memset(&Handlers[i], 0, sizeof(Handlers[i]));
                TimerCount--;
                break;
            }
        }
    }
}

// Включить таймер
void timer_Resume(TEventHandler Handler)
{
    if(TimerFrequency)
    {
        int i;
        
        for(i = 0; i < TIMER_HANDLERS; i++)
        {
            if(Handlers[i].Handler == Handler)
            {
                Handlers[i].Run = true;
                break;
            }
        }
    }
}

// Остановить таймер
void timer_Stop(TEventHandler Handler)
{
    if(TimerFrequency)
    {
        int i;
        
        for(i = 0; i < TIMER_HANDLERS; i++)
        {
            if(Handlers[i].Handler == Handler)
            {
                Handlers[i].Run = false;
                Handlers[i].Fired =  false;
                break;
            }
        }
    }
}

static bool Main(void)
{
    int i;
	
	for(i = 0; i < TIMER_HANDLERS; i++)
	{
        // Если сработало - вызовем.
        if(Handlers[i].Fired)
        {
            Handlers[i].Fired = false;
            Handlers[i].Handler();
        }
    }
    
    return !Error;
}

const TDriver systimer = { "systimer", &Init, &Uninit, &Main};

