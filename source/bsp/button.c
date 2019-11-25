// ***********************************************************
//	button.с
// 	Драйвер кнопок v2.0.0 (stm32f0discovery)
//
//  Требования: gpio, systimer
//  teplofizik, 2016
// ***********************************************************

#include "button.h"
#include "../drivers.h"

// Задержка между нажатиями при зажатой кнопке
#define DEFAULT_COUNTDOWN  30

static TButtonHandler button_Handler;
static bool ButtonLoaded = false;

typedef struct
{
    TPin Pin;
    bool ActiveLevel; // Активный уровень
    bool Pull;        // Необходима подтяжка внутренняя?
} TButtonPin;

// Распиновка кнопок
static TButtonPin Buttons[] = {
    {{PA, 0}, true, false},   // user
};

// Кроличество кнопок
static const int ButtonCount = sizeof(Buttons) / sizeof(Buttons[0]);

// Отсчёт
static int Countdown[ButtonCount];


// Добавить кнопку для опроса
//void button_Add(const TPin * Pin, bool ActiveLevel, bool Pull, uint8_t Index);


// Опрос ножки, частота 100 Гц
static void OnTimer(void)
{
    int i;
    
    for(i = 0; i < ButtonCount; i++)
    {
        if(gp_Get(&Buttons[i].Pin) == Buttons[i].ActiveLevel)
        {
            // Если нажатие зафиксировано, то считаем
            if(Countdown[i])
            {
                Countdown[i]--;
            }
            else
            {
                // Задержка перед вторым нажатием
                Countdown[i] = DEFAULT_COUNTDOWN;
                // Если есть обработчик - вызовем его
                // Кнопка всего одна - передаём 0
                if(button_Handler) button_Handler(i);
            }
        }
        else
        {
            // Первое нажатие фиксируется сразу
            Countdown[i] = 0;
        }
    }
}

// Состояние кнопки
bool button_GetState(int Button)
{
    if(ButtonLoaded)
    {
        if((Button >= ButtonCount) || (Button < 0)) return false;
    
        // Активна?
        return (gp_Get(&Buttons[Button].Pin) == Buttons[Button].ActiveLevel);
    }
    else
        return false;
}

// Инициализация драйвера кнопки
static bool Init(void)
{
    int i;
    
    if(!drv_Require(&button, "gpio")) return false;
    if(!drv_Require(&button, "systimer")) return false;
    
    for(i = 0; i < ButtonCount; i++)
    {
        // Вывод в режиме входа
        gp_Input(&Buttons[i].Pin);
        
        // Включить подтяжку, если надо
        if(Buttons[i].Pull) 
        {
            if(Buttons[i].ActiveLevel)
                gp_PullDown(&Buttons[i].Pin);
            else
                gp_PullUp(&Buttons[i].Pin);
        }
        
        // Счётчик, нажатия фиксируются только когда он 0
        Countdown[i] = 0;
    }
    
    // Запустим процесс опроса кнопки
    timer_AddFunction(100, &OnTimer);
    ButtonLoaded = true;
    return true;
}

// Выгрузка драйвера
static void Uninit(void)
{
    int i;
    
    for(i = 0; i < ButtonCount; i++)
        gp_NoPull(&Buttons[i].Pin);
    
    timer_Remove(&OnTimer);
    button_Handler = 0;
    ButtonLoaded = false;
}

// Установить обработчик нажатия
void button_SetHandler(TButtonHandler Handler)
{
    if(ButtonLoaded)
    {
        button_Handler = Handler;
    }
}

const TDriver button = { "button", &Init, &Uninit, 0 };
