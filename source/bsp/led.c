// ***********************************************************
//	led.�
// 	������� ����������� v2.0.0 (stm32f0discovery)
//
//  ����������: gpio, systimer
//  teplofizik, 2016
// ***********************************************************

#include "led.h"
#include "../drivers.h"

static const int LedTimeout = 10; // 0.2s

// ���������� ����� �� ����� D, ������� 12-15.
static const TPin Leds[] = { {PC, 8}, {PC, 9} };
static const int LedCount = sizeof(Leds) / sizeof(Leds[0]);

static bool LedLoaded = false;
static int LedTimer[LedCount];
static bool LedMode[LedCount];

// �������� ��������� �� ��� ������
void ledi_On(uint8_t Index)
{
    if(Index >= LedCount) return;
    
    gp_High(&Leds[Index]);
}
    
// ��������� ��������� �� ��� ������
void ledi_Off(uint8_t Index)
{
    if(Index >= LedCount) return;
    
    gp_Low(&Leds[Index]);
}

// ������ ��������� ����������
void ledi_Set(uint8_t Index, bool Value)
{
    if(Value)
        ledi_On(Index);
    else
        ledi_Off(Index);
}

void OnTimer(void)
{
    int i;
    
    for(i = 0; i < LedCount; i++) 
    {
        if(!LedMode[i]) continue;
        if(LedTimer[i] > 0)
            LedTimer[i]--;
        
        ledi_Set(i, LedTimer[i] > 0);
    }
}

// ������������� �������� �����������
static bool Init(void)
{
    int i;
    
    if(!drv_Require(&led, "gpio")) return false;
    if(!drv_Require(&led, "systimer")) return false;
    
    for(i = 0; i < LedCount; i++) 
    {
        LedTimer[i] = 0;
        gp_Output(&Leds[i]);
        ledi_Off(i);
    }
    
    LedLoaded = true;
    timer_AddFunction(50, &OnTimer);
    return true;
}

// ���������� �������
static void Uninit(void)
{
    int i;
    
    for(i = 0; i < LedCount; i++)
        gp_Input(&Leds[i]);
    
    timer_Remove(&OnTimer);
    LedLoaded = false;
}

// �������� ��������� �� ��� ������
void led_On(uint8_t Index)
{
    if(LedLoaded)
    {
        if(Index >= LedCount) return;
        
        LedMode[Index] = false;
        ledi_On(Index);
    }
}
    
// ��������� ��������� �� ��� ������
void led_Off(uint8_t Index)
{
    if(LedLoaded)
    {
        if(Index >= LedCount) return;
        
        LedMode[Index] = false;
        ledi_Off(Index);
    }
}

// ������ ��������� ����������
void led_Set(uint8_t Index, bool Value)
{
    if(LedLoaded)
    {
        if(Index >= LedCount) return;
        
        LedMode[Index] = false;
        ledi_Set(Index, Value);
    }
}

void led_Activate(uint8_t Index)
{
    if(LedLoaded)
    {
        if(Index >= LedCount) return;
        
        LedMode[Index] = true;
        LedTimer[Index] = LedTimeout;
    }
}

const TDriver led = { "led", &Init, &Uninit, 0 };
