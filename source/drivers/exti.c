// ***********************************************************
//  exti.c
//  Драйвер внешних прерываний v2.0.0
//
//  Требования: lowlevelsys
//  teplofizik, 2016
// ***********************************************************

#include "exti.h"
#include <stm32f0xx.h>

#include <string.h>

#define FAST_DETECT 1

typedef struct
{
    TPin        Pin;
    bool        Enabled;
    TSignalEdge Edge;
    
    TEventHandler Handler;
} TEXTIRecord;

static const int HandlerCount = 16;
static TEXTIRecord EXTIHandlerList[HandlerCount];
static bool EXTIFlag[HandlerCount];
static bool Detect = false;
static bool ExtiEnabled = false;

static __inline void EXTI_IRQHandler(uint8_t Index)
{
    EXTI->PR = (1 << Index); 
#if (FAST_DETECT == 0)
	EXTIFlag[Index] = true;
    Detect = true;
#else
	{
		TEXTIRecord * Record = &EXTIHandlerList[Index];
	
		if(Record->Handler) Record->Handler();
	}
#endif
}

void EXTI0_1_IRQHandler(void)
{
    int i;
    for(i = 0; i < 2; i++) if(EXTI->PR & (1 << i)) EXTI_IRQHandler(i);
}

void EXTI2_3_IRQHandler(void)
{
    int i;
    for(i = 2; i < 4; i++) if(EXTI->PR & (1 << i)) EXTI_IRQHandler(i);
}

void EXTI4_15_IRQHandler(void)
{
    int i;
    for(i = 4; i < 16; i++) if(EXTI->PR & (1 << i)) EXTI_IRQHandler(i);
}

static void exti_EnableInterrupt(uint8_t Index)
{
    EXTI->IMR |= (1 << Index);
    
    switch(Index)
    {
    case 0: 
    case 1: NVIC_EnableIRQ(EXTI0_1_IRQn); NVIC_SetPriority(EXTI0_1_IRQn, 1); break;
    case 2: 
    case 3: NVIC_EnableIRQ(EXTI2_3_IRQn); NVIC_SetPriority(EXTI2_3_IRQn, 1); break;
    case 4: 
    case 5:
    case 6:
    case 7:
    case 8:
    case 9: 
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15: NVIC_EnableIRQ(EXTI4_15_IRQn); NVIC_SetPriority(EXTI4_15_IRQn, 1); break;
    }
}

static void exti_DisableInterrupt(uint8_t Index)
{
    EXTI->IMR &= ~(1 << Index);
    
    switch(Index)
    {
    case 0: 
    case 1: NVIC_DisableIRQ(EXTI0_1_IRQn); break;
    case 2: 
    case 3: NVIC_DisableIRQ(EXTI2_3_IRQn); break;
    case 4: 
    case 5:
    case 6:
    case 7:
    case 8:
    case 9: 
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15: NVIC_DisableIRQ(EXTI4_15_IRQn); break;
    }
}

// void Handler(void);
bool exti_Listen(const TPin * Pin, TSignalEdge Edge, TEventHandler Handler)
{
	if(ExtiEnabled)
	{
		uint8_t Register;
		uint8_t Shift;
		uint32_t Temp;
		uint8_t Index;
		
		Index = Pin->Pin;
		
		if(Index > 15) return false;
		if(Pin->Port > PD) return false;
		
		Register = Index >> 2;
		Shift = (Index & 0x03) * 4;
		
		// Выберем порт для прослушивания
		Temp = SYSCFG->EXTICR[Register];
		Temp &= ~(0xF << Shift);
		Temp |= (Pin->Port << Shift);
		SYSCFG->EXTICR[Register] = Temp;
		
		EXTIHandlerList[Index].Edge = Edge;
		EXTIHandlerList[Index].Enabled = true;
		EXTIHandlerList[Index].Handler = Handler;
		memcpy(&EXTIHandlerList[Index].Pin, Pin, sizeof(TPin));
		
		switch(Edge)
		{
		case EDGE_RISING:
			EXTI->RTSR |=  (1 << Index);
			EXTI->FTSR &= ~(1 << Index);
			break;
		case EDGE_FALLING:
			EXTI->RTSR &= ~(1 << Index);
			EXTI->FTSR |=  (1 << Index);
			break;
		case EDGE_BOTH:
			EXTI->RTSR |= (1 << Index);
			EXTI->FTSR |= (1 << Index);
			break;
		}
		
		// Сбросим флаг прерывания, если он был до этого
		EXTI->PR = (1 << Index);
		
		exti_EnableInterrupt(Index);
		
		return true;
	}
	else
		return false;
}

static bool Init(void)
{
    int i;
	
    if(!drv_Require(&exti, "lowlevelsys")) return false;
	
    for(i = 0; i < HandlerCount; i++) EXTIFlag[i] = false;
    memset(EXTIHandlerList, 0, sizeof(EXTIHandlerList));
    
	ExtiEnabled = true;
	return true;
}

static void Uninit(void)
{
    int i;
    for(i = 0; i < HandlerCount; i++)
	{
		EXTIFlag[i] = false;
		
		if(EXTIHandlerList[i].Enabled)
			exti_DisableInterrupt(i);
	}
	EXTI->RTSR = 0;
	EXTI->FTSR = 0;
	EXTI->PR = 0xFFFF;
	
    memset(EXTIHandlerList, 0, sizeof(EXTIHandlerList));
	
	ExtiEnabled = false;
}

static bool Main(void)
{
    int i;
    
#if (FAST_DETECT == 0)
    if(Detect)
	{		
		Detect = false;
    
		for(i = 0; i < HandlerCount; i++)
		{
			if(EXTIFlag[i])
			{
				TEXTIRecord * Record = &EXTIHandlerList[i];
				EXTIFlag[i] = false;
            
				if(Record->Handler) Record->Handler();
			}
		}
	}
#endif
	return true;
}

const TDriver exti = { "exti", &Init, Uninit, &Main };
