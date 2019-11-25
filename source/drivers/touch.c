// ***********************************************************
//	touch.c
// 	Сенсорные кнопки
//
//  Требования: clock, gpio, systimer
//  teplofizik, 2017
// ***********************************************************

#include "touch.h"
#include "clock.h"
#include "systick.h"
#include <stm32f0xx.h>

static const int ChannelOnLimit = 10; // В пределах 10-270

typedef struct
{
	TPin     Pin;
	uint8_t  AF;
	
	uint8_t  Group;
	uint8_t  IO;
} TTouchPin;

static const TTouchPin TPins[] = {
//    Pin   AF  GR  IO
	{{PA, 0}, 3,  1,  1},
	{{PA, 1}, 3,  1,  2},
	{{PA, 2}, 3,  1,  3},
	{{PA, 3}, 3,  1,  4},
	
	{{PA, 4}, 3,  2,  1},
	{{PA, 5}, 3,  2,  2},
	{{PA, 6}, 3,  2,  3},
	{{PA, 7}, 3,  2,  4},
	
	{{PA, 9}, 3,  4,  1},
	{{PA,10}, 3,  4,  2},
	{{PA,11}, 3,  4,  3},
	{{PA,12}, 3,  4,  4},
	
	{{PB, 0}, 3,  3,  2},
	{{PB, 1}, 3,  3,  3},
	{{PB, 2}, 3,  3,  4},
	
	{{PB, 3}, 3,  5,  1},
	{{PB, 4}, 3,  5,  2},
	{{PB, 6}, 3,  5,  3},
	{{PB, 7}, 3,  5,  4},
	
	{{PB,11}, 3,  6,  1},
	{{PB,12}, 3,  6,  2},
	{{PB,13}, 3,  6,  3},
	{{PB,14}, 3,  6,  4},
	
	{{PC, 5}, 0,  3,  1},
	
	{{PD,12}, 1,  8,  1},
	{{PD,13}, 1,  8,  2},
	{{PD,14}, 1,  8,  3},
	{{PD,15}, 1,  8,  4},
	
#ifdef AVAIL_GPIOE
	{{PE, 2}, 1,  7,  1},
	{{PE, 3}, 1,  7,  2},
	{{PE, 4}, 1,  7,  3},
	{{PE, 5}, 1,  7,  4},
#endif
};
static const int TCount = sizeof(TPins) / sizeof(TPins[0]);
static const uint32_t SampleMasks[4] = {
	0x11111111,
	0x22222222,
	0x44444444,
	0x88888888
};

static uint8_t  SampleIndex = 0;
static uint32_t Enabled = 0;
#if (ENABLE_MEASURE == 0)
static uint32_t Active  = 0;
#endif

static uint16_t TouchBase[32];
#if (ENABLE_MEASURE != 0)
static int16_t TouchResult[32];
#endif

static const TTouchPin * GetTouchPin(const TPin * P)
{
	int i;
	for(i = 0; i < TCount; i++)
	{
		if(gp_Equals(&TPins[i].Pin, P)) return &TPins[i];
	}
	
	return 0;
}

static uint8_t GetId(const TTouchPin * Pin)
{
	int G = Pin->Group - 1;
	int IO = Pin->IO - 1;
	
	return (G * 4 + IO);
}

static uint32_t GetMask(const TTouchPin * Pin)
{
	return 1u << GetId(Pin);
}

static void HysteresisControl(const TTouchPin * Pin, bool Enable)
{
	uint32_t Mask = GetMask(Pin);
	
	if(Enable)
		TSC->IOHCR |= Mask;
	else
		TSC->IOHCR &= ~Mask;
}

//static void AnalogSwControl(const TTouchPin * Pin, bool Enable)
//{
//	uint32_t Mask = GetMask(Pin);
//	
//	if(Enable)
//		TSC->IOASCR |= Mask;
//	else
//		TSC->IOASCR &= ~Mask;
//}

//static void ChannelControl(const TTouchPin * Pin, bool Enable)
//{
//	uint32_t Mask = GetMask(Pin);
//	
//	if(Enable)
//		TSC->IOCCR |= Mask;
//	else
//		TSC->IOCCR &= ~Mask;
//}

static void SamplingCapControl(const TTouchPin * Pin, bool Enable)
{
	uint32_t Mask = GetMask(Pin);
	
	if(Enable)
		TSC->IOSCR |= Mask;
	else
		TSC->IOSCR &= ~Mask;
}

static void ConfigGP(const TPin * P, bool Cap)
{
	const TTouchPin * TP = GetTouchPin(P);
	
	if(TP)
	{
		uint8_t Id = GetId(TP);
		
		gp_OpenDrain(P, Cap);
		gp_AlternateFunction(P, TP->AF);
		
		HysteresisControl(TP, false);
		//ChannelControl(TP, !Cap);
		SamplingCapControl(TP, Cap);
		
		if(!Cap) Enabled |= (1 << Id);
		
		TSC->IOGCSR |= 1 << (TP->Group - 1);
	}
}

static void SetupTimings(int CTPH, int CTPL, int PGPSC, int MCV)
{
	uint32_t CR = TSC->CR;
	
	CR = (CR & 0x0FFFFFFF) | ((CTPH & 0x0F) << 28);
	CR = (CR & 0xF0FFFFFF) | ((CTPL & 0x0F) << 24);
	CR = (CR & 0xFFFF0FFF) | ((PGPSC & 0x0F) << 12);
	CR = (CR & 0xFFFFFF1F) | ((MCV & 0x07) << 5);
	
	TSC->CR = CR;
}

// Использовать вывод как сенсорный вход
void touch_ConfigIn  (const TPin * P, TEventHandler Handler)
{
	ConfigGP(P, false);
}
	
// Использовать вывод как выход на измерительный конденсатор
void touch_ConfigCap (const TPin * P)
{
	ConfigGP(P, true);
}
	
// Активен ли канал измерения
bool touch_IsActivated(const TPin * P)
{
	const TTouchPin * TP = GetTouchPin(P);
	
	if(TP)
	{
		uint8_t Id = GetId(TP);
#if (ENABLE_MEASURE != 0)
		return TouchResult[Id] > ChannelOnLimit;
#else
		return (Active & (1 << Id));
#endif
	}
	return false;
}

#if (ENABLE_MEASURE != 0)
int16_t touch_Get(const TPin * P)
{
	const TTouchPin * TP = GetTouchPin(P);
	
	if(TP)
	{
		uint8_t Id = GetId(TP);
		
		return TouchResult[Id];
	}
	return 0;
}
#endif

static int GetResult(int Index)
{
	if((Index >= 0) && (Index < 8))
		return TSC->IOGXCR[Index];
	else
		return 0;
}

static void StartSample(void)
{
	TSC->IOCCR = SampleMasks[SampleIndex] & Enabled;
	
	if(Enabled != 0) 
		TSC->CR |= TSC_CR_START;
}

static void EndSample(void)
{
	int i;
	for(i = 0; i < 8; i++)
	{
		uint8_t Id = SampleIndex + i * 4;
		if(Enabled & (1 << Id))
		{
			if(TouchBase[Id])
			{
				uint16_t Res = TouchBase[i] - GetResult(i);
#if (ENABLE_MEASURE != 0)
				TouchResult[Id] = Res;
#else
				if(Res > ChannelOnLimit)
					Active |=  (1 << Id);
				else
					Active &= ~(1 << Id);
#endif
			}
			else
			{
				TouchBase[Id] = GetResult(i);
			}
		}
	}
	
	if(SampleIndex < 3)
		SampleIndex++;
	else
		SampleIndex = 0;
}


static void Timer(void)
{
	if(TSC->ISR & TSC_ISR_EOAF)
	{
		EndSample();
		StartSample();
	}
}

static bool Init(void)
{
    if(!drv_Require(&touch, "clock")) return false;
    if(!drv_Require(&touch, "gpio")) return false;
    if(!drv_Require(&touch, "systimer")) return false;
	
	clock_EnablePeripheral(PR_TSC);
	timer_AddFunction(50, Timer);
	
	TSC->CR = TSC_CR_TSCE;
	SetupTimings(15, 15, 4, 6);
	StartSample();
	
	return true;
}

static void Uninit(void)
{
	clock_DisablePeripheral(PR_TSC);
	timer_Remove(Timer);
}

const TDriver touch = { "touch", Init, Uninit, 0 };
