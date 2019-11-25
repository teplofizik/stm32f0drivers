// ***********************************************************
//	button.�
// 	������� ������ v2.0.0 (stm32f0discovery)
//
//  ����������: gpio, systimer
//  teplofizik, 2016
// ***********************************************************

#include "button.h"
#include "../drivers.h"

// �������� ����� ��������� ��� ������� ������
#define DEFAULT_COUNTDOWN  30

static TButtonHandler button_Handler;
static bool ButtonLoaded = false;

typedef struct
{
    TPin Pin;
    bool ActiveLevel; // �������� �������
    bool Pull;        // ���������� �������� ����������?
} TButtonPin;

// ���������� ������
static TButtonPin Buttons[] = {
    {{PA, 0}, true, false},   // user
};

// ����������� ������
static const int ButtonCount = sizeof(Buttons) / sizeof(Buttons[0]);

// ������
static int Countdown[ButtonCount];


// �������� ������ ��� ������
//void button_Add(const TPin * Pin, bool ActiveLevel, bool Pull, uint8_t Index);


// ����� �����, ������� 100 ��
static void OnTimer(void)
{
    int i;
    
    for(i = 0; i < ButtonCount; i++)
    {
        if(gp_Get(&Buttons[i].Pin) == Buttons[i].ActiveLevel)
        {
            // ���� ������� �������������, �� �������
            if(Countdown[i])
            {
                Countdown[i]--;
            }
            else
            {
                // �������� ����� ������ ��������
                Countdown[i] = DEFAULT_COUNTDOWN;
                // ���� ���� ���������� - ������� ���
                // ������ ����� ���� - ������� 0
                if(button_Handler) button_Handler(i);
            }
        }
        else
        {
            // ������ ������� ����������� �����
            Countdown[i] = 0;
        }
    }
}

// ��������� ������
bool button_GetState(int Button)
{
    if(ButtonLoaded)
    {
        if((Button >= ButtonCount) || (Button < 0)) return false;
    
        // �������?
        return (gp_Get(&Buttons[Button].Pin) == Buttons[Button].ActiveLevel);
    }
    else
        return false;
}

// ������������� �������� ������
static bool Init(void)
{
    int i;
    
    if(!drv_Require(&button, "gpio")) return false;
    if(!drv_Require(&button, "systimer")) return false;
    
    for(i = 0; i < ButtonCount; i++)
    {
        // ����� � ������ �����
        gp_Input(&Buttons[i].Pin);
        
        // �������� ��������, ���� ����
        if(Buttons[i].Pull) 
        {
            if(Buttons[i].ActiveLevel)
                gp_PullDown(&Buttons[i].Pin);
            else
                gp_PullUp(&Buttons[i].Pin);
        }
        
        // �������, ������� ����������� ������ ����� �� 0
        Countdown[i] = 0;
    }
    
    // �������� ������� ������ ������
    timer_AddFunction(100, &OnTimer);
    ButtonLoaded = true;
    return true;
}

// �������� ��������
static void Uninit(void)
{
    int i;
    
    for(i = 0; i < ButtonCount; i++)
        gp_NoPull(&Buttons[i].Pin);
    
    timer_Remove(&OnTimer);
    button_Handler = 0;
    ButtonLoaded = false;
}

// ���������� ���������� �������
void button_SetHandler(TButtonHandler Handler)
{
    if(ButtonLoaded)
    {
        button_Handler = Handler;
    }
}

const TDriver button = { "button", &Init, &Uninit, 0 };
