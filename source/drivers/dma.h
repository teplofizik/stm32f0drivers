// ***********************************************************
//	dma.h
// 	Прямой доступ к памяти v2.0.0
// 
//  Требования: lowlevelsys, clock
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _DMA_H
#define _DMA_H

    typedef enum
    {
        DMA_SIZE_8,
        DMA_SIZE_16,
        DMA_SIZE_32
    } TDMASize;

    typedef enum
    {
        DMA_ADC,       // 16 bit
        DMA_DAC1,      // 8,16 bit; right-aligned
#ifdef AVAIL_DAC2
        DMA_DAC2,      // 8,16 bit; right-aligned
#endif
        DMA_USART1_TX, // 8 bit
        DMA_USART1_RX, // 8 bit
        DMA_USART2_TX, // 8 bit
        DMA_USART2_RX, // 8 bit
#ifdef AVAIL_USART3
        DMA_USART3_TX, // 8 bit
        DMA_USART3_RX  // 8 bit
#endif
    } TDMASource;

    // Работа дма в обычном режиме (выплюнул и забыл)
    void dma_Start(TDMASource Source, void * Buffer, TDMASize Size, int Count);
    
    // Работа дма в циклическом режиме
    void dma_StartCircular(TDMASource Source, void * Buffer, TDMASize Size, int Count);
    
    // Остановить канал DMA
    void dma_Stop(TDMASource Source);
    
    // Получить номер полученного/отправляемого элемента
    int  dma_GetIndex(TDMASource Source);
    
    extern const TDriver dma;
    
#endif
