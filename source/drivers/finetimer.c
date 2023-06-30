// ***********************************************************
//	finetimer.h
//  Работа с точными задержками
//
//  Требования: clock
//  teplofizik, 2013
// ***********************************************************

#include "finetimer.h"
#include "clock.h"
#include <stm32f0xx.h>

// Используемый таймер - TIM17
static TIM_TypeDef * TIM = TIM17;

// Обработчик таймера
static TFineTimerHandler UserHandler;

// Сработало сравнение
static void OnTimerCompare(void)
{
	if(UserHandler) UserHandler(FINETIMER_EVENT_COMPARE);
}

// Переполнение таймера
static void OnTimerOverflow(void)
{
	if(UserHandler) UserHandler(FINETIMER_EVENT_OVERFLOW);
}

// Прерывание
void TIM17_IRQHandler(void)
{
    if(TIM->SR & TIM_SR_CC1IF)
    {
        TIM->SR &= ~TIM_SR_CC1IF;
        OnTimerCompare();
    }
    else if(TIM->SR & TIM_SR_UIF) // Переполнение
    {
        TIM->SR &= ~TIM_SR_UIF;
        OnTimerOverflow();
    }
}

// Установка обработчика
void ftim_SetHandler(TFineTimerHandler Handler)
{
	UserHandler = Handler;
}

// Запустить таймер на срабатывание после заданного периода времени (мкс)
//  Period: период в мкс
// Compare: время для срабатывания по сравнению
void ftim_Start(int Period, int Compare)
{
    // На сколько тиков запускать таймер
    int Ticks = Period * ((clock_GetAPB() / 100000UL) * 67 / 100);
    int CompareTicks = Compare * ((clock_GetAPB() / 100000UL) * 67 / 100);
    
    // Пределы счета
    TIM->ARR = Ticks;
    TIM->CCR1 = CompareTicks;
    
    // Разрешить прерывание по переполнению и по сравнению
    TIM->DIER |= TIM_DIER_UIE | ((Compare) ? TIM_DIER_CC1IE : 0);
    
    // Счет разрешен
    TIM->CR1 |= TIM_CR1_CEN;    
}

static bool Init(void)
{
    if(!drv_Require(&finetimer, "clock")) return false;
	
    // Настройка таймера
    // Тактирование включено
    RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;

    // Режим одного импульса - после первого переполнения остановка.
    TIM->CR1 = TIM_CR1_OPM;
    
    // Разрешение прерывания
    NVIC_EnableIRQ(TIM17_IRQn);
    NVIC_SetPriority(TIM17_IRQn, 2);
	
    return true;
}

static void Uninit(void)
{
	
}

const TDriver finetimer = { "finetimer", &Init, &Uninit, 0 };
