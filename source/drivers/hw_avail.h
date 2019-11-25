// ***********************************************************
//	hw_avail.h
// 	Список поддерживаемой периферии
// 
//  teplofizik, 2016
// ***********************************************************

#ifndef HW_AVAIL
#define HW_AVAIL

// #define AVAIL_DMA2
// #define AVAIL_DAC2
// #define AVAIL_GPIOE
// #define AVAIL_USART3

#if defined(STM32F071x8) || defined(STM32F071xB)

	#define DMA_CHANNELS                 7

	#define AVAIL_DAC2
	#define AVAIL_GPIOE
	#define AVAIL_USART3

#endif

#if defined(STM32F051x8) || defined(STM32F051xB)

	#define DMA_CHANNELS                 5
	
#endif

#endif
