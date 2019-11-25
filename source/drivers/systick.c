// ***********************************************************
//	systick.�
// 	��������� ������ v2.0.0
//
//  teplofizik, 2016
// ***********************************************************

#include "systick.h"
#include "clock.h"
#include "driver.h"
#include <stm32f0xx.h>
#include <string.h>

// ���������� ������������ ������������
#define TIMER_HANDLERS    15

typedef struct 
{
	TEventHandler Handler;
	uint16_t      Countdown;
	uint16_t      Reload;
    
    // ��������
    bool          Run;
    // ���������
    bool          Fired;
} THandlers;

// �������, ��� ����� ����������
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
		
				// �������, ��� ���������
				Handlers[i].Fired = true;
			}
		}
	}
}

// ������ ������� � �������� ��������
static bool Init(void)
{
    uint32_t Frequency = SYSTEM_FREQUENCY;
    if(!Frequency) return false;
    
    // ������� ���������
    TimerCount = 0;
    Error = false;
    TimerFrequency = Frequency;
    memset(&Handlers[0], 0, sizeof(Handlers));
    
    // ��������� �������
    SysTick_Config(clock_GetSYSCLK() / Frequency);
    
    // ���������� ����������
    NVIC_SetPriority(SysTick_IRQn, 15);
    
    return true;
}

// ���������� �������
static void Uninit(void)
{
    Error = false;
    TimerFrequency = 0;
    SysTick->CTRL = 0;
    memset(&Handlers[0], 0, sizeof(Handlers));
}

// �������� ������� � ������ ������, Handler ����� ���������� � �������� ��������
void timer_AddFunction(uint16_t Frequency, TEventHandler Handler)
{
    if(TimerFrequency)
    {
        int i;
        
        for(i = 0; i < TIMER_HANDLERS; i++)
        {
            // ����� ������ ����
            if(!Handlers[i].Handler)
            {
                // ����������, ������� ������
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

// �������� ������� �������
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

// ������� ������
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

// �������� ������
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

// ���������� ������
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
        // ���� ��������� - �������.
        if(Handlers[i].Fired)
        {
            Handlers[i].Fired = false;
            Handlers[i].Handler();
        }
    }
    
    return !Error;
}

const TDriver systimer = { "systimer", &Init, &Uninit, &Main};

