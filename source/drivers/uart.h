// ***********************************************************
// uart.h
// Драйвер UART v2.0.0
//
// Требуется: gpio, clock, dma, memory
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _UART_H
#define _UART_H

    typedef enum
    {
        UART1,  // PA9,  PA10
        UART2,  // PA2,  PA3
#ifdef AVAIL_USART3
        UART3   // PB10, PB11
#endif
    } TUart;

    typedef enum
    {
        UART_EVENT_ONBREAK,
        UART_EVENT_ONRECEIVE,
        UART_EVENT_ONTRANSMITCOMPLETED
    } TUartEvent;
    
    typedef enum
    {
        STOP_BITS_0_5,
        STOP_BITS_1,
        STOP_BITS_1_5,
        STOP_BITS_2
    } TUartStopBits;
    
    typedef void (* TUartBreakHandler)(uint32_t Uart);
    typedef void (* TUartReceiveHandler)(uint32_t Uart, uint8_t Data);
    typedef void (* TUartTransmittedHandler)(uint32_t Uart);
    typedef void (* TUartTransmitCompletedHandler)(uint32_t Uart);
    
	// Инициализация модуля USART
	void uart_Init(TUart Uart, uint32_t BaudRate, TUartStopBits StopBits);

    // Выключить уарт
    void uart_Uninit(TUart Uart);
    
    // Разрешить прием
    void uart_ReceiveEnable(TUart Uart, bool Enable);
    
    // Разрешить передачу
    void uart_TransmitEnable(TUart Uart, bool Enable);
    
    // Отправить
    void uart_Send(TUart Uart, const uint8_t * Data, int Length);
    
    // Установить скорость передачи
    void uart_SetBaudrate(TUart Uart, uint32_t Baudrate);
    
    // Установить индекс порта
    void uart_SetPortIndex(TUart Uart, uint32_t Index);
    
    // Установить обработчик
    void uart_SetHandler(TUart Uart, TUartEvent Event, void * Handler);
    
    extern const TDriver uart;
    
#endif
