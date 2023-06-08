// ***********************************************************
// uart.h
// Драйвер UART v2.0.0
//
// Требуется: gpio, clock, dma, memory
//  teplofizik, 2016
// ***********************************************************

#include "uart.h"
#include "gpio.h"
#include "memory.h"
#include "dma.h"
#include "clock.h"
#include <stm32f0xx.h>
#include <string.h>

#define RX_BUFFER_SIZE   100

uint8_t RxSnapshot[RX_BUFFER_SIZE];
uint8_t RxIndexSnapshot;
uint8_t BkIndexSnapshot;

typedef struct
{
	USART_TypeDef     * USART;
	TPeripheral         Peripheral;
	
    IRQn_Type           IRQn;
    
	uint32_t            AlternateFunction;
	TPin                TX;
	TPin                RX;
    
    TDMASource          TXS;
    TDMASource          RXS;
} TUartPort;

typedef struct
{
    TUartBreakHandler             onBreak;
    TUartReceiveHandler           onReceive;
    TUartTransmittedHandler       onTransmitted;
    TUartTransmitCompletedHandler onTransmitCompleted;
    
    uint8_t * RxBuffer;
    uint8_t * BkBuffer;
    uint32_t  RxBufferIndex;
    
    uint8_t * TxBuffer;
    uint32_t  TxBufferSize;
    
    uint32_t Index;
    
    bool     Enabled;
    bool     TXE;
    bool     RXE;
    bool     Completed;
} TUartState;

// Таблица уартов
static const TUartPort Ports[] = {
    { USART1, PR_USART1, USART1_IRQn,   1, {PA,  9}, {PA, 10}, DMA_USART1_TX, DMA_USART1_RX }, // 0, {PB,  6}, {PB, 7}
    { USART2, PR_USART2, USART2_IRQn,   1, {PA,  2}, {PA,  3}, DMA_USART2_TX, DMA_USART2_RX },
#ifdef AVAIL_USART3 // todo
    { USART3, PR_USART3, USART3_4_IRQn, 4, {PB, 10}, {PB, 11}, DMA_USART3_TX, DMA_USART3_RX }
#endif
};

static const int PortCount = sizeof(Ports) / sizeof(Ports[0]);

// Драйвер загружен?
static bool UARTEnabled = false;

static const TPin TestPin = {PB, 8};

// Состояние портов
static TUartState UartState[PortCount];


// Принят Break
static void uart_onBreak(TUart Uart)
{
    if(UartState[Uart].onBreak) UartState[Uart].onBreak(UartState[Uart].Index);
}

// Принят байт
static void uart_onReceive(TUart Uart, uint8_t Data)
{
    if(UartState[Uart].onReceive) UartState[Uart].onReceive(UartState[Uart].Index, Data);
}

// Передача завершена
static void uart_onTransmitCompleted(TUart Uart)
{
    if(UartState[Uart].onTransmitCompleted) UartState[Uart].onTransmitCompleted(UartState[Uart].Index);
}

static void uart_MarkBreak(TUart Uart)
{
    int Index = RX_BUFFER_SIZE - dma_GetIndex(Ports[Uart].RXS);
    if(Index)
        Index--;
    else
        Index = RX_BUFFER_SIZE - 1;
	UartState[Uart].BkBuffer[Index] = 0x01;
    
    //memcpy(RxSnapshot, UartState[Uart].RxBuffer, RX_BUFFER_SIZE);
    //RxIndexSnapshot = UartState[Uart].RxBufferIndex;
    //BkIndexSnapshot = Index;
}

void USART_IRQHandler(TUart Uart)
{
    USART_TypeDef * USART = Ports[Uart].USART;
    uint32_t Status = USART->ISR;
    if(Status & USART_ISR_TC)
    {
        USART->CR1 &= ~(USART_CR1_TXEIE | USART_CR1_TCIE);  // Transmitter empty interrupt disable
        mem_Free(UartState[Uart].TxBuffer);
        UartState[Uart].TxBuffer = 0;
        UartState[Uart].TxBufferSize = 0;
        
        uart_onTransmitCompleted(Uart);
    }
	if(Status & USART_ISR_FE)
	{
        USART->ICR = USART_ICR_FECF;
		// mark byte as frame error
        gp_High(&TestPin);
		uart_MarkBreak(Uart);
        gp_Low(&TestPin);
	}
}

// Прерывание USART
void USART1_IRQHandler(void) { USART_IRQHandler(UART1); }
void USART2_IRQHandler(void) { USART_IRQHandler(UART2); }
#ifdef AVAIL_USART3 // todo
void USART3_4_IRQHandler(void) { USART_IRQHandler(UART3); }
#endif

// Установить скорость передачи
void uart_SetBaudrate(TUart Uart, uint32_t Baudrate)
{
    if(UARTEnabled)
    {
        if(Uart >= PortCount) return;
        
        Ports[Uart].USART->BRR = clock_GetPeripheralClock(Ports[Uart].Peripheral) / Baudrate;
    }
}

// Настройка стоповых бит
static void uart_StopBits(TUart Uart, TUartStopBits StopBits)
{
    USART_TypeDef * USART;
    if(Uart >= PortCount) return;
    
    USART = Ports[Uart].USART;
    
    USART->CR2 &= ~USART_CR2_STOP;
    switch(StopBits)
    {
    case STOP_BITS_0_5: // 0b01
        break;
    case STOP_BITS_1:   // 0b00
        break;
    case STOP_BITS_1_5: // 0b11
        USART->CR2 |= USART_CR2_STOP_0 | USART_CR2_STOP_1;
        break;
    case STOP_BITS_2:   // 0b10
        USART->CR2 |= USART_CR2_STOP_1;
        break;
    }
}

// Установить индекс порта
void uart_SetPortIndex(TUart Uart, uint32_t Index)
{
    if(Uart >= PortCount) return;
    
    UartState[Uart].Index = Index;
}

// Инициализация модуля USART
void uart_Init(TUart Uart, uint32_t BaudRate, TUartStopBits StopBits)
{
    if(UARTEnabled && (Uart < PortCount))
    {
        USART_TypeDef * USART = Ports[Uart].USART;
        
        if(UartState[Uart].Enabled)
        {
            // Освободим занятую память
            if(UartState[Uart].RxBuffer)
                mem_Free(UartState[Uart].RxBuffer);
            if(UartState[Uart].BkBuffer)
                mem_Free(UartState[Uart].BkBuffer);
        }
        
        memset(&UartState[Uart], 0, sizeof(UartState[0]));
        
        // Включить тактирование модуля
		clock_EnablePeripheral(Ports[Uart].Peripheral);
        
        USART->CR1 = 0;
        
        uart_SetBaudrate(Uart, BaudRate);
        uart_StopBits(Uart, StopBits);
        
        NVIC_EnableIRQ(Ports[Uart].IRQn);
        NVIC_SetPriority(Ports[Uart].IRQn, 0);
        
		UartState[Uart].Index = Uart;
        UartState[Uart].RxBuffer = mem_Alloc(RX_BUFFER_SIZE);
        UartState[Uart].BkBuffer = mem_Alloc(RX_BUFFER_SIZE);
        UartState[Uart].RxBufferIndex = 0;
        UartState[Uart].TxBuffer = 0;
        UartState[Uart].TxBufferSize = 0;
        
        UartState[Uart].Completed = true;
        UartState[Uart].Enabled   = true;
        
        memset(UartState[Uart].RxBuffer, 0, RX_BUFFER_SIZE);
        memset(UartState[Uart].BkBuffer, 0, RX_BUFFER_SIZE);
    }
}

// Выключить уарт
void uart_Uninit(TUart Uart)
{    
    if(UARTEnabled && (Uart < PortCount))
    {
        USART_TypeDef * USART = Ports[Uart].USART;
        
        // Освободим память приемника
        if(UartState[Uart].Enabled)
        {
            // Освободим занятую память
            if(UartState[Uart].RxBuffer)
                mem_Free(UartState[Uart].RxBuffer);
            if(UartState[Uart].BkBuffer)
                mem_Free(UartState[Uart].BkBuffer);
        }
        
        // Завершим передачу
        if(UartState[Uart].TXE)
        {
            if(UartState[Uart].TxBuffer)
            {
                dma_Stop(Ports[Uart].TXS);
                mem_Free(UartState[Uart].TxBuffer);
            }
            gp_Input(&Ports[Uart].TX);
        }
        // Завершим прием
        if(UartState[Uart].RXE)
        {
            dma_Stop(Ports[Uart].RXS);
            gp_Input(&Ports[Uart].RX);
        }
        // Выключим уарт
        USART->CR1 = 0; 
        // Запретим прерывание
        NVIC_DisableIRQ(Ports[Uart].IRQn);
        
        // Отключим тактирование
		clock_DisablePeripheral(Ports[Uart].Peripheral);
        
        // Очистим память слота
        memset(&UartState[Uart], 0, sizeof(UartState));
    }
}

// Включение и выключение приемника
void uart_ReceiveEnable(TUart Uart, bool Enable)
{
    if(UARTEnabled && (Uart < PortCount))
    {
        USART_TypeDef * USART = Ports[Uart].USART;
       
        if(Enable)
        {
            USART->CR1 |=  USART_CR1_RE |    // Включить приемник
                           USART_CR1_RXNEIE |
                           USART_CR1_UE;
            
            USART->CR3 |= USART_CR3_DMAR;
            // Вход в альтернативную функцию
            gp_AlternateFunction(&Ports[Uart].RX, Ports[Uart].AlternateFunction);
            
            // Запуск DMA на циклический режим работы
            dma_StartCircular(Ports[Uart].RXS, UartState[Uart].RxBuffer, DMA_SIZE_8, RX_BUFFER_SIZE);
        }
        else
        {
            USART->CR1 &= ~USART_CR1_RE;     // Выключить приемник
            
            // Отключим модуль вовсе, если нет ни приемника, ни передатчика
            if(!(USART->CR1 & USART_CR1_TE))
                USART->CR1 &= ~USART_CR1_UE; 
            
            USART->CR3 &= ~USART_CR3_DMAR;
            // Вернем вход в дефолтный режим
            gp_Input(&Ports[Uart].RX);
            
            // Остановка DMA
            dma_Stop(Ports[Uart].RXS);
        }
        
        UartState[Uart].RXE = Enable; 
    }
}

// Включение и выключение передатчика
void uart_TransmitEnable(TUart Uart, bool Enable)
{
    if(UARTEnabled && (Uart < PortCount))
    {
        USART_TypeDef * USART = Ports[Uart].USART;
        
        if(Enable)
        {
            USART->ICR = USART_ICR_TCCF;
            USART->CR1 |= USART_CR1_TE |      // Включить передатчик
                          USART_CR1_UE;
            USART->CR3 |= USART_CR3_DMAT;
            
            // Выход в альтернативную функцию
            gp_AlternateFunction(&Ports[Uart].TX, Ports[Uart].AlternateFunction);
        }
        else
        {
            USART->CR1 &= ~(USART_CR1_TCIE |   // Выключить передатчик
                            USART_CR1_TE);
            // Отключим модуль вовсе, если нет ни приемника, ни передатчика
            if(!(USART->CR1 & USART_CR1_RE))
                USART->CR1 &= ~USART_CR1_UE; 
            
            USART->CR3 &= ~USART_CR3_DMAT;
            // Вернем выход в дефолтный режим
            gp_Input(&Ports[Uart].TX);
        }
        
        UartState[Uart].TXE = Enable; 
    }
}

// Отправить
void uart_Send(TUart Uart, const uint8_t * Data, int Length)
{
    if(UARTEnabled && (Uart < PortCount))
    {
        TUartState * S = &UartState[Uart];
        if(S->TXE && !S->TxBuffer)
        {
            USART_TypeDef * USART = Ports[Uart].USART;
            
            S->TxBuffer = mem_Alloc(Length);
            S->TxBufferSize = Length;
			memcpy(S->TxBuffer, Data, Length);
			
            USART->ICR = USART_ICR_TCCF;
            USART->CR1 |= USART_CR1_TCIE;
            dma_Start(Ports[Uart].TXS, S->TxBuffer, DMA_SIZE_8, Length);
        }
    }
}

// Установить обработчик события
void uart_SetHandler(TUart Uart, TUartEvent Event, void * Handler)
{
    if(UARTEnabled && (Uart < PortCount))
    {
        switch(Event)
        {
        case UART_EVENT_ONBREAK:             UartState[Uart].onBreak = (TUartBreakHandler)Handler; break;
        case UART_EVENT_ONRECEIVE:           UartState[Uart].onReceive = (TUartReceiveHandler)Handler; break;
        case UART_EVENT_ONTRANSMITCOMPLETED: UartState[Uart].onTransmitCompleted = (TUartTransmitCompletedHandler)Handler; break;
        }
    }
}

// ==========================================================

static bool Init(void)
{
    if(!drv_Require(&uart, "gpio")) return false;
    if(!drv_Require(&uart, "clock")) return false;
    if(!drv_Require(&uart, "memory")) return false;
    if(!drv_Require(&uart, "dma")) return false;

    memset(UartState, 0, sizeof(UartState));
    
    gp_Output(&TestPin);
    gp_Low(&TestPin);
    
    UARTEnabled = true;
    return true;
}

static void Uninit(void)
{
    UARTEnabled = false;
    gp_Input(&TestPin);
}

static void uart_ProcessDataPart(int Index, int From, int Length)
{
    TUart Uart = (TUart)Index;
    TUartState * S = &UartState[Index];
    const uint8_t * Buffer = &S->RxBuffer[From];
    int i;
    
    for(i = 0; i < Length; i++)
    {
        // Помечается символ после брейка
        if(UartState[Uart].BkBuffer[From])
        {
            UartState[Uart].BkBuffer[From] = 0;
            uart_onBreak(Uart);
        }
        
        uart_onReceive(Uart, Buffer[i]);
    }
}

static bool Main(void)
{
    int i;
    for(i = 0; i < PortCount; i++)
    {
        if(UartState[i].RXE)
        {
            TUartState * S = &UartState[i];
            int ToRx = RX_BUFFER_SIZE - dma_GetIndex(Ports[i].RXS);
    
            if(ToRx != S->RxBufferIndex)
            {
                if(ToRx > S->RxBufferIndex)
                    uart_ProcessDataPart(i, S->RxBufferIndex, ToRx - S->RxBufferIndex);
                else
                {
                    uart_ProcessDataPart(i, S->RxBufferIndex, RX_BUFFER_SIZE - S->RxBufferIndex);
                    uart_ProcessDataPart(i, 0, ToRx);
                }
                
                S->RxBufferIndex = ToRx;
            }
        }
    }
    return true;
}

const TDriver uart = { "uart", &Init, &Uninit, &Main };
