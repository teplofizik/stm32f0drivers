// ***********************************************************
//	dma.c
// 	Прямой доступ к памяти v2.0.0
// 
//  Требования: lowlevelsys, clock
//  teplofizik, 2016
// ***********************************************************

#include "dma.h"
#include "clock.h"
#include <stm32f0xx.h>
#include <string.h>

typedef enum
{
    Channel1,
    Channel2,
    Channel3,
    Channel4,
    Channel5,
#if (DMA_CHANNELS > 5)
    Channel6,
#endif
#if (DMA_CHANNELS > 6)
    Channel7
#endif
} TDMACh;

// Расброс по каналам (03x,04x,05x)
// 1: ADC, T2.3, T17.1, T17.U
// 2: SPI1.RX, U1.TX, I2C1.TX, T1.1, T2.U, T3.3 (R:ADC)
// 3: SPI1.TX, U1.RX, I2C1.RX, T1.2, T2.2, T3.4, T3.U, T6.U, DAC.1, T16.1, T16.U
// 4: SPI2.RX, U2.TX, I2C2.TX, T1.4, T1.TRIG, T1.COM, T2.4, T3.1, T3.TRIG
// 5: SPI2.TX, U2.RX, I2C2.RX, T1.3, T1.U, T2.1, T15.1, T15.U, T15.TRIG, T15.COM

// Расброс по каналам (07x)
// 4: [T7.U, DAC.2]
// 6: U3.RX, U4.RX (+ ремап)
// 7: U3.TX, U4.TX (+ ремап)

typedef struct
{
    DMA_Channel_TypeDef * DMA_Channel;
	IRQn_Type             NVIC_IRQn;
} TDMAChannel;

static const TDMAChannel Channels[] = {
    { DMA1_Channel1, DMA1_Channel1_IRQn },
    { DMA1_Channel2, DMA1_Channel2_3_IRQn },
    { DMA1_Channel3, DMA1_Channel2_3_IRQn },
    { DMA1_Channel4, DMA1_Channel4_5_6_7_IRQn },
    { DMA1_Channel5, DMA1_Channel4_5_6_7_IRQn },
#if (DMA_CHANNELS > 5)
    { DMA1_Channel6, DMA1_Channel4_5_6_7_IRQn },
#endif
#if (DMA_CHANNELS > 6)
    { DMA1_Channel7, DMA1_Channel4_5_6_7_IRQn },
#endif
};
static const int DMAChannelCount = sizeof(Channels) / sizeof(Channels[0]);
static bool       ChEnabled[DMAChannelCount];
static TDMASource ChSource[DMAChannelCount];
static bool       ChInterrupt[DMAChannelCount];

void DMA_Channel_IRQHandler(int Index)
{
	ChInterrupt[Index] = false;
	ChEnabled[Index] = false;
	//onComplete(Index);
}

void DMA1_Channel1_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_TCIF1)
	{
        DMA1->IFCR = DMA_IFCR_CTCIF1 | DMA_IFCR_CGIF1;
		if(ChInterrupt[0])
		{
			DMA1_Channel1->CCR &= ~DMA_CCR_EN;
			NVIC_DisableIRQ(DMA1_Channel1_IRQn);
			DMA_Channel_IRQHandler(0);
		}
	}
}

void DMA1_Channel2_3_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_TCIF2)
	{
        DMA1->IFCR = DMA_IFCR_CTCIF2 | DMA_IFCR_CGIF2;
		if(ChInterrupt[1])
		{
			DMA1_Channel2->CCR &= ~DMA_CCR_EN;
			if(!ChInterrupt[2]) NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
			DMA_Channel_IRQHandler(1);
		}
	}
	if(DMA1->ISR & DMA_ISR_TCIF3)
	{
        DMA1->IFCR = DMA_IFCR_CTCIF3 | DMA_IFCR_CGIF3;
		if(ChInterrupt[2])
		{
			DMA1_Channel3->CCR &= ~DMA_CCR_EN;
			if(!ChInterrupt[1]) NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
			DMA_Channel_IRQHandler(2);
		}
	}
}

void DMA1_Channel4_5_6_7_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_TCIF4)
	{
        DMA1->IFCR = DMA_IFCR_CTCIF4 | DMA_IFCR_CGIF4;
		if(ChInterrupt[3])
		{
			DMA1_Channel4->CCR &= ~DMA_CCR_EN;
			if(!ChInterrupt[4]) NVIC_DisableIRQ(DMA1_Channel4_5_6_7_IRQn);
			DMA_Channel_IRQHandler(3);
		}
	}
	if(DMA1->ISR & DMA_ISR_TCIF5)
	{
        DMA1->IFCR = DMA_IFCR_CTCIF5 | DMA_IFCR_CGIF5;
		if(ChInterrupt[4])
		{
			DMA1_Channel5->CCR &= ~DMA_CCR_EN;
			if(!ChInterrupt[3]) NVIC_DisableIRQ(DMA1_Channel4_5_6_7_IRQn);
			DMA_Channel_IRQHandler(4);
		}
	}    
}

int dma_GetChannel(TDMASource Source)
{
    switch(Source)
    {
    case DMA_ADC: return (SYSCFG->CFGR1 & SYSCFG_CFGR1_ADC_DMA_RMP) ? Channel2 : Channel1;
    case DMA_DAC1: return Channel3;
#ifdef AVAIL_DAC2
    case DMA_DAC2: return Channel4;
#endif
    case DMA_USART1_TX: return (SYSCFG->CFGR1 & SYSCFG_CFGR1_USART1TX_DMA_RMP) ? Channel4 : Channel2;
    case DMA_USART1_RX: return (SYSCFG->CFGR1 & SYSCFG_CFGR1_USART1RX_DMA_RMP) ? Channel5 : Channel3;
    case DMA_USART2_TX: return Channel4; // TODO: можно ремапить на ch6
    case DMA_USART2_RX: return Channel5; // TODO: можно ремапить на ch7
#ifdef AVAIL_USART3
    case DMA_USART3_TX: return Channel6; // TODO: можно ремапить
    case DMA_USART3_RX: return Channel7; // TODO: можно ремапить
#endif
    }
    
    return -1;
}

uint32_t dma_GetChannelPeripheral(TDMASource Source, TDMASize Size)
{
    switch(Source)
    {
    case DMA_ADC:       return (uint32_t)&ADC1->DR;
    case DMA_DAC1:      return (Size == DMA_SIZE_8) ? (uint32_t)&DAC->DHR8R1 : ((Size == DMA_SIZE_16) ? (uint32_t)&DAC->DHR12R1 : 0);
#ifdef AVAIL_DAC2
    case DMA_DAC2:      return (Size == DMA_SIZE_8) ? (uint32_t)&DAC->DHR8R2 : ((Size == DMA_SIZE_16) ? (uint32_t)&DAC->DHR12R2 : 0);
#endif
    case DMA_USART1_TX: return (uint32_t)&USART1->TDR;
    case DMA_USART1_RX: return (uint32_t)&USART1->RDR;
    case DMA_USART2_TX: return (uint32_t)&USART2->TDR;
    case DMA_USART2_RX: return (uint32_t)&USART2->RDR;
#ifdef AVAIL_USART3
    case DMA_USART3_TX: return (uint32_t)&USART3->TDR;
    case DMA_USART3_RX: return (uint32_t)&USART3->RDR;
#endif
    }
    
    return 0;
}

static bool Init(void)
{
    if(!drv_Require(&dma, "lowlevelsys")) return false;
    if(!drv_Require(&dma, "clock")) return false;
    
    // Включим тактирование DMA
	clock_EnablePeripheral(PR_DMA);
    
    memset(ChEnabled, 0, sizeof(ChEnabled));
    
    return true;
}

static void Uninit(void)
{
    // Выключим тактирование DMA
	clock_DisablePeripheral(PR_DMA);
}

// Остановить канал DMA
void dma_Stop(TDMASource Source)
{
    int C = dma_GetChannel(Source);
    if((C >= 0) && ChEnabled[C])
    {
        const TDMAChannel * DC = &Channels[C];
        DC->DMA_Channel->CCR = 0;
        DC->DMA_Channel->CNDTR = 0;
        DC->DMA_Channel->CMAR = 0;
        DC->DMA_Channel->CPAR = 0;
        
        ChEnabled[C] = false;
    }
}

//
bool dma_CheckCompleted(int Channel)
{
    if(Channel >= 0)
    {
        const TDMAChannel * DC = &Channels[Channel];
        
        return !(DC->DMA_Channel->CCR & DMA_CCR_EN);
    }
    else
        return false;
}

static void dma_ConfigureChannel(TDMASource Source, void * Buffer, TDMASize Size, int Count, bool Circular)
{
    int C = dma_GetChannel(Source);
    const TDMAChannel * DC = &Channels[C];
    
    {
        uint32_t CCR;
    
        DC->DMA_Channel->CCR = 0;
        DC->DMA_Channel->CNDTR = Count;
        DC->DMA_Channel->CPAR = dma_GetChannelPeripheral(Source, Size);
        DC->DMA_Channel->CMAR = (uint32_t)Buffer;
        
        CCR = DMA_CCR_MINC | DMA_CCR_EN;
        if(Circular) CCR |= DMA_CCR_CIRC;
        
		switch(Source)
		{
		case DMA_DAC1:	
#ifdef AVAIL_DAC2
		case DMA_DAC2:
#endif
		case DMA_USART1_TX:
		case DMA_USART2_TX:
#ifdef AVAIL_USART3
		case DMA_USART3_TX:
#endif
			CCR |= DMA_CCR_DIR | DMA_CCR_TCIE;
			NVIC_EnableIRQ(DC->NVIC_IRQn);
			ChInterrupt[C] = true;
			break;		
		default:
			ChInterrupt[C] = false;
			break;
		}
		
        switch(Size)
        {
        case DMA_SIZE_8: break;
        case DMA_SIZE_16: CCR |= DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0; break;
        case DMA_SIZE_32: CCR |= DMA_CCR_MSIZE_1 | DMA_CCR_PSIZE_1; break;
        }
        
        DC->DMA_Channel->CCR = CCR;
        
        ChSource[C] = Source;
        ChEnabled[C] = true;
    }
}

// Работа дма в обычном режиме (выплюнул и забыл)
void dma_Start(TDMASource Source, void * Buffer, TDMASize Size, int Count)
{
    int C = dma_GetChannel(Source);
    bool Avail = !ChEnabled[C] || (ChEnabled[C] && (dma_CheckCompleted(C)));
    if((C >= 0) && Avail)
        dma_ConfigureChannel(Source, Buffer, Size, Count, false);
}

// Работа дма в циклическом режиме
void dma_StartCircular(TDMASource Source, void * Buffer, TDMASize Size, int Count)
{
    int C = dma_GetChannel(Source);
    bool Avail = !ChEnabled[C] || (ChSource[C] == Source);
    if((C >= 0) && Avail)
        dma_ConfigureChannel(Source, Buffer, Size, Count, true);
}

// Получить номер полученного/отправляемого элемента
int  dma_GetIndex(TDMASource Source)
{
    int C = dma_GetChannel(Source);
    if((C >= 0) && ChEnabled[C] && (ChSource[C] == Source))
    {
        const TDMAChannel * DC = &Channels[C];
        
        return DC->DMA_Channel->CNDTR;
    }
    
    return 0;
}

const TDriver dma = { "dma", &Init, &Uninit, 0 };
