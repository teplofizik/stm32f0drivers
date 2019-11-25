// ***********************************************************
//  timer.h
// 	Управление таймерами v2.0.5
//  Таймеры
//
//  Требования: gpio, clock
//  teplofizik, 2017
// ***********************************************************

#include "timer.h"
#include "gpio.h"
#include "clock.h"
#include <stm32f0xx.h>
#include <string.h>

typedef struct
{
	TPeripheral         Peripheral;
    TIM_TypeDef *       TIM;        // Таймер
    bool                Advanced;   // С поддержкой управления двигателем? (требует дополнительных телодвижений)
	uint8_t             PChannels;   // Количество каналов ШИМ прямых
	uint8_t             NChannels;   // Количество каналов ШИМ инверсных
} TTimerDef;

// Описание канала вывода таймера
typedef struct
{
    TPin       Pin;               // Вывод
    uint8_t    AlternateFunction; // Номер альтернативной функции
    TTimer     TIM;               // Таймер
    int8_t     Channel;           // Канал таймера
} TChannel;

static const TTimerDef Timers[] = {
	{ PR_NONE,  0, false, 0, 0},             //  
	{ PR_TIM1,  TIM1,  true,  4, 3},  //  TIM1 - advanced  4P  3N
	{ PR_TIM2,  TIM2, false,  4, 0},  //  TIM2 - common    4P, 0N
	{ PR_TIM3,  TIM3, false,  4, 0},  //  TIM3 - common    4P, 0N
	{ PR_NONE,  0, false, 0, 0},             //  
	{ PR_NONE,  0, false, 0, 0},             //  
	{ PR_NONE,  0, false, 0, 0},             //  
	{ PR_NONE,  0, false, 0, 0},             //  
	{ PR_NONE,  0, false, 0, 0},             // 
	{ PR_NONE,  0, false, 0, 0},             // 
	{ PR_NONE,  0, false, 0, 0},             // 
	{ PR_NONE,  0, false, 0, 0},             // 
	{ PR_NONE,  0, false, 0, 0},             // 
	{ PR_NONE,  0, false, 0, 0},             // 
	{ PR_TIM14, TIM14, false,  1, 0},  // TIM14 - common    1P, 0N
	{ PR_TIM15, TIM15, true,   2, 1},   // TIM15 - common    2P, 1N
	{ PR_TIM16, TIM16, true,   1, 1},  // TIM16 - common    1P, 1N
	{ PR_TIM17, TIM17, true,   1, 1},  // TIM17 - common    1P, 1N
};

// Для таймеров 1,2 - 1, для таймеров 3-5 - 2, для таймеров 8-11 - 3, для таймеров12-14 - 9
// Таблица с настройками ШИМ (можно вписать иные конфигурации с альтернативным маппингом
static const TChannel Channels[] = {
	// PORT A
	{{PA, 0}, 2,  T2,  1},   //  PA0 - TIM2_CH1
	{{PA, 1}, 2,  T2,  2},   //  PA1 - TIM2_CH2
	{{PA, 1}, 5, T15, -1},   //  PA1 - TIM15_CH1N
	{{PA, 2}, 2,  T2,  3},   //  PA2 - TIM2_CH3
	{{PA, 2}, 0, T15,  1},   //  PA2 - TIM15_CH1
	{{PA, 3}, 2,  T2,  4},   //  PA3 - TIM2_CH4
	{{PA, 3}, 0, T15,  2},   //  PA3 - TIM15_CH2
	{{PA, 4}, 4, T14,  1},   //  PA4 - TIM14_CH1
	{{PA, 5}, 2,  T2,  1},   //  PA5 - TIM2_CH1
	{{PA, 6}, 1,  T3,  1},   //  PA6 - TIM3_CH1
	{{PA, 6}, 5, T16,  1},   //  PA6 - TIM16_CH1
	{{PA, 7}, 2,  T1, -1},   //  PA7 - TIM1_CH1N
	{{PA, 7}, 1,  T3,  2},   //  PA7 - TIM3_CH2
	{{PA, 7}, 4, T14,  1},   //  PA7 - TIM14_CH1
	{{PA, 7}, 5, T17,  1},   //  PA7 - TIM17_CH1
	{{PA, 8}, 2,  T1,  1},   //  PA8 - TIM1_CH1
	{{PA, 9}, 2,  T1,  2},   //  PA9 - TIM1_CH2
	{{PA,10}, 2,  T1,  3},   // PA10 - TIM1_CH3
	{{PA,11}, 2,  T1,  4},   // PA11 - TIM1_CH4
	{{PA,15}, 2,  T2,  1},   // PA15 - TIM2_CH1

	// PORT B
	{{PB, 0}, 2,  T1, -2},   //  PB0 - TIM1_CH2N
	{{PB, 0}, 1,  T3,  3},   //  PB0 - TIM3_CH3
	{{PB, 1}, 2,  T1, -3},   //  PB1 - TIM1_CH3N
	{{PB, 1}, 1,  T3,  4},   //  PB1 - TIM3_CH4
	{{PB, 1}, 0, T14,  1},   //  PB1 - TIM14_CH1
	{{PB, 3}, 2,  T2,  2},   //  PB3 - TIM2_CH2
	{{PB, 4}, 1,  T3,  1},   //  PB4 - TIM3_CH1
	{{PB, 5}, 1,  T3,  2},   //  PB5 - TIM3_CH2
	{{PB, 6}, 2, T16, -1},   //  PB6 - TIM16_CH1N
	{{PB, 7}, 2, T17, -1},   //  PB7 - TIM17_CH1N
	{{PB, 8}, 2, T16,  1},   //  PB8 - TIM16_CH1
	{{PB, 9}, 2, T17,  1},   //  PB9 - TIM17_CH1
	
	{{PB,10}, 2,  T2,  3},   // PB10 - TIM2_CH3
	{{PB,11}, 2,  T2,  4},   // PB11 - TIM2_CH4
	
	{{PB,13}, 2,  T1, -1},   // PB13 - TIM1_CH1N
	{{PB,14}, 2,  T1, -2},   // PB14 - TIM1_CH2N
	{{PB,14}, 1, T15,  1},   // PB14 - TIM15_CH1
	{{PB,15}, 2,  T1, -3},   // PB15 - TIM1_CH3N
	{{PB,15}, 1, T15,  2},   // PB13 - TIM15_CH2
	{{PB,15}, 3, T15, -1},   // PB13 - TIM15_CH1N
	
	// PORT C
	{{PC, 6}, 0,  T3,  1},   //  PC6 - TIM3_CH1
	{{PC, 7}, 0,  T3,  2},   //  PC7 - TIM3_CH2
	{{PC, 8}, 0,  T3,  3},   //  PC8 - TIM3_CH3
	{{PC, 9}, 0,  T3,  4},   //  PC9 - TIM3_CH4
	
	// PORT E
#ifdef AVAIL_GPIOE
	{{PE, 0}, 0, T16,  1},   //  PE0 - TIM16_CH1
	{{PE, 1}, 0, T17,  1},   //  PE1 - TIM17_CH1
	{{PE, 3}, 0,  T3,  1},   //  PE3 - TIM3_CH1
	{{PE, 4}, 0,  T3,  2},   //  PE4 - TIM3_CH2
	{{PE, 5}, 0,  T3,  3},   //  PE5 - TIM3_CH3
	{{PE, 6}, 0,  T3,  4},   //  PE6 - TIM3_CH4
	{{PE, 8}, 0,  T1, -1},   //  PE8 - TIM1_CH1N
	{{PE, 9}, 0,  T1,  1},   //  PE9 - TIM1_CH1
	{{PE,10}, 0,  T1, -2},   // PE10 - TIM1_CH2N
	{{PE,11}, 0,  T1,  2},   // PE11 - TIM1_CH2
	{{PE,12}, 0,  T1, -3},   // PE12 - TIM1_CH3N
	{{PE,13}, 0,  T1,  3},   // PE11 - TIM1_CH3
	{{PE,14}, 0,  T1,  4},   // PE11 - TIM1_CH4
#endif
	
	// PORT F
	{{PF, 9}, 0, T15,  1},   //  PF9 - TIM15_CH1
	{{PF,10}, 0, T15,  2},   // PF10 - TIM15_CH2
};

// Количество таймеров
static const int TimerCount = sizeof(Timers) / sizeof(Timers[0]);

// Количество выходов
static const int ChannelCount = sizeof(Channels) / sizeof(Channels[0]);

static const TTimerDef * GetTimer(TTimer T)
{
	uint8_t TI = (uint8_t)T;
	
	if(TI < TimerCount)
	{
		const TTimerDef * TD = &Timers[TI];
		return (TD->TIM) ? TD : 0;
	}
	else
		return 0;
}

// Получить связанный с каналом таймер
TTimer tim_GetTimerFromOC(TOCChannel OC)
{
	if(OC < ChannelCount)
	{
		return Channels[OC].TIM;
	}
	return TNA;
}

void tim_InterruptEnable(TTimer T, bool Set, uint32_t Mask)
{
	const TTimerDef * TD = GetTimer(T);
	if(TD)
	{
		if(Set)
			TD->TIM->DIER |= Mask;
		else
			TD->TIM->DIER &= ~Mask;
	}
}

// Запуск счёта таймера
void tim_Start(TTimer T)
{
	const TTimerDef * TD = GetTimer(T);
	if(TD)
		TD->TIM->CR1 |= TIM_CR1_CEN;
}

// Запуск счёта таймера
void tim_Stop(TTimer T)
{
	const TTimerDef * TD = GetTimer(T);
	if(TD)
		TD->TIM->CR1 &= ~TIM_CR1_CEN;
}

// Установка значения регистра сравнения
static void SetValue(const TTimerDef * T, const TChannel * Channel, uint16_t Value)
{
    switch(Channel->Channel)
    {
	case -1: T->TIM->CCR1 = Value; break;
	case -2: T->TIM->CCR2 = Value; break;
	case -3: T->TIM->CCR3 = Value; break;
    case 1:  T->TIM->CCR1 = Value; break;
    case 2:  T->TIM->CCR2 = Value; break;
    case 3:  T->TIM->CCR3 = Value; break;
    case 4:  T->TIM->CCR4 = Value; break;
    }
}

// Инициализировать таймер под ШИМ
static void InitTimer(const TTimerDef * TD, uint32_t Counts, uint32_t Frequency)
{
    uint32_t Clock = clock_GetPeripheralClock(TD->Peripheral); // Частота шины
	uint32_t Presc = (Clock / Frequency / Counts);
    // Включить тактирование модуля
    clock_EnablePeripheral(TD->Peripheral);
    
    // Разрядность: N бит
    TD->TIM->ARR = Counts - 1;
    
    // Предделитель:
    TD->TIM->PSC = (Presc) ? Presc - 1 : 0;
    
    if(TD->Advanced)
        // Разрешить выход
        TD->TIM->BDTR |= TIM_BDTR_MOE;
}

// Запись о канале
static const TChannel * GetChannel(TOCChannel Channel)
{
    if (Channel >= ChannelCount) return 0;
    
    return &Channels[Channel];
}

// Инициализация вывода ШИМ
static void InitChannel(const TChannel * Channel, bool Polarity)
{
	const TTimerDef * T = GetTimer(Channel->TIM);
	uint32_t EN = (T->TIM->CR1 & TIM_CR1_CEN);
	
    // Выберем альтернативную функцию вывода
    gp_AlternateFunction(&Channel->Pin, Channel->AlternateFunction);
    
    // Выключить счёт на время настройки
    if(EN) T->TIM->CR1 &= ~TIM_CR1_CEN;
    
    switch(Channel->Channel)
    {
    case -1:
        // Режим: ШИМ1 0b110
        T->TIM->CCMR1 &= ~TIM_CCMR1_OC1M;  // очистим поле режима
        T->TIM->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1); // Запишем 0b110
        
        // Канал 1 включить
		if(!Polarity) T->TIM->CCER |= TIM_CCER_CC1NP;
        T->TIM->CCER |= TIM_CCER_CC1NE;
        break;
    case -2:
        // Режим: ШИМ1 0b110
        T->TIM->CCMR1 &= ~TIM_CCMR1_OC2M;  // очистим поле режима
        T->TIM->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1); // Запишем 0b110
        
        // Канал 2 включить
		if(!Polarity) T->TIM->CCER |= TIM_CCER_CC2NP;
        T->TIM->CCER |= TIM_CCER_CC2NE;
        break;
    case -3:
        // Режим: ШИМ1 0b110
        T->TIM->CCMR2 &= ~TIM_CCMR2_OC3M;  // очистим поле режима
        T->TIM->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1); // Запишем 0b110
        
        // Канал 3 включить
		if(!Polarity) T->TIM->CCER |= TIM_CCER_CC3NP;
        T->TIM->CCER |= TIM_CCER_CC3NE;
        break;
    case 1:
        // Режим: ШИМ1 0b110
        T->TIM->CCMR1 &= ~TIM_CCMR1_OC1M;  // очистим поле режима
        T->TIM->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1); // Запишем 0b110
        
        // Канал 1 включить
		if(!Polarity) T->TIM->CCER |= TIM_CCER_CC1P;
        T->TIM->CCER |= TIM_CCER_CC1E;
        break;
    case 2:
        // Режим: ШИМ1 0b110
        T->TIM->CCMR1 &= ~TIM_CCMR1_OC2M;  // очистим поле режима
        T->TIM->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1); // Запишем 0b110
        
        // Канал 2 включить
		if(!Polarity) T->TIM->CCER |= TIM_CCER_CC2P;
        T->TIM->CCER |= TIM_CCER_CC2E;
        break;
    case 3:
        // Режим: ШИМ1 0b110
        T->TIM->CCMR2 &= ~TIM_CCMR2_OC3M;  // очистим поле режима
        T->TIM->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1); // Запишем 0b110
        
        // Канал 3 включить
		if(!Polarity) T->TIM->CCER |= TIM_CCER_CC3P;
        T->TIM->CCER |= TIM_CCER_CC3E;
        break;
    case 4:
        // Режим: ШИМ1 0b110
        T->TIM->CCMR2 &= ~TIM_CCMR2_OC4M;  // очистим поле режима
        T->TIM->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1); // Запишем 0b110
        
        // Канал 4 включить
		if(!Polarity) T->TIM->CCER |= TIM_CCER_CC4P;
        T->TIM->CCER |= TIM_CCER_CC4E;
        break;
    }
    
    // Обнулим вывод
    SetValue(T, Channel, 0);
    
    // Передадим сигнал о сбросе счётчика, чтоб все внутренние регистры точно обновились.
    T->TIM->EGR = TIM_EGR_UG;
    
    // Включить счёт
    if(EN) T->TIM->CR1 |= TIM_CR1_CEN;
}

// Установить значение регистра сравнения
void tim_SetValue(TOCChannel Channel, uint16_t Value)
{
    const TChannel * C = GetChannel(Channel);
	const TTimerDef * T = GetTimer(C->TIM);
	
	if(T) SetValue(T, C, Value);
}


// Настроить выход как ШИМ
void tim_InitOC_PWM(TOCChannel Channel, bool Polarity)
{
    const TChannel * C = GetChannel(Channel);
	
	InitChannel(C, Polarity);
}

// Настройка таймера
void tim_Init(TTimer T, uint32_t Counts, uint32_t Frequency)
{
	const TTimerDef * TD = GetTimer(T);
	if(TD)
	{
		InitTimer(TD, Counts, Frequency);
	}
}

void tim_Uninit(TTimer T)
{
	const TTimerDef * TD = GetTimer(T);
	if(TD)
	{
		clock_DisablePeripheral(TD->Peripheral);
	}
}

static bool Init(void)
{
    if(!drv_Require(&timer, "gpio")) return false;
    if(!drv_Require(&timer, "clock")) return false;
    
    return true;
}

static void Uninit(void)
{
	
}

const TDriver timer = { "timer", &Init, &Uninit, 0 };

