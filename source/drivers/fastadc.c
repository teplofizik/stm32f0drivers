// ***********************************************************
//	fastadc.c
// 	Быстрый АЦП на один канал v2.1.0
//
//  Требования: gpio, dma, clock
//  teplofizik, 2016
// ***********************************************************

// TODO: бесшовное перекидывание данных при изменении набора каналов

#include "fastadc.h"
#include "dma.h"
#include "clock.h"
#include "../config.h"
#include <string.h>
#include <stm32f0xx.h>

static const TPin Inputs[] = {
    {PA, 0}, // ADC_PA0
    {PA, 1}, // ADC_PA1
    {PA, 2}, // ADC_PA2
    {PA, 3}, // ADC_PA3
    {PA, 4}, // ADC_PA4
    {PA, 5}, // ADC_PA5
    {PA, 6}, // ADC_PA6
    {PA, 7}, // ADC_PA7
    
    {PB, 0}, // ADC_PB0
    {PB, 1}, // ADC_PB1
    
    {PC, 0}, // ADC_PC0
    {PC, 1}, // ADC_PC1
    {PC, 2}, // ADC_PC2
    {PC, 3}, // ADC_PC3
    {PC, 4}, // ADC_PC4
    {PC, 5}, // ADC_PC5
};

// Количество физических выводов (остальные внутренние)
static const int ADCPhysicalInputs = sizeof(Inputs) / sizeof(Inputs[0]);

static bool ADCEnabled = false;
// Включенные каналы
static bool     Enabled;
// Массив для результатов измерений
uint16_t ADCRes[MAX_ADC_SAMPLES];
// Номера каналов измеренных величин
static uint8_t  ADCIndex;

static int ADCCount;

void adc_InitDMA(int Count)
{
    if(Count)
        dma_StartCircular(DMA_ADC, &ADCRes[0], DMA_SIZE_16, Count);
    else
        dma_Stop(DMA_ADC);
}

// Перенастроим систему измерения
static void adc_Reconfigure(void)
{
    // Выключим АЦП
    if(Enabled)
    {
        ADC1->CFGR1 &= ~ADC_CFGR1_CONT;
        dma_Stop(DMA_ADC);
    }
    
	ADC1->CHSELR = (1 << ADCIndex);
	
    // Включим измерение
    if((ADCCount > 0) && Enabled)
    {
        ADC1->CR |= ADC_CR_ADEN;
        ADC1->SMPR = 0; 
        ADC1->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG; // DMA en, circ
        ADC1->CFGR1 |= ADC_CFGR1_CONT;
        ADC1->CR |= ADC_CR_ADSTART;
    }
    else
    {
        ADC1->CR = 0;
    }
    // Настроим DMA
    adc_InitDMA(ADCCount);
}

// АЦП
static bool Init(void)
{
    if(!drv_Require(&fastadc, "clock")) return false;
    if(!drv_Require(&fastadc, "gpio")) return false;
    if(!drv_Require(&fastadc, "dma")) return false;
    
    memset(ADCRes, 0, sizeof(ADCRes));
	ADCIndex = 0;
	Enabled = false;
	ADCCount = 0;
    
    ADCEnabled = true;
    
    // Включим и сбросим АЦП
	clock_EnablePeripheral(PR_ADC);
	clock_ResetPeripheral(PR_ADC);
	
    // АЦП включится, когда будет надо
	//ADC1->CFGR1 = (ADC1->CFGR1 & ~ADC_CFGR1_RES) | ADC_CFGR1_RES_0;
	ADC1->CFGR2 = 0x40000000; // PCLK/2 (Synchronous clock mode)
    
    return true;
}

// Выгрузка драйвера АЦП
static void Uninit(void)
{
    // Выключить тактирование
	clock_DisablePeripheral(PR_ADC);
    
    // Флаг, что драйвер не загружен
    ADCEnabled = false;
    
    // Выключить DMA
    adc_InitDMA(0);
}


void fadc_SetHandler(TEventHandler Handler)
{
	dma_SetHandler(DMA_ADC, Handler);	
}

// Включить канал измерения
void fadc_Enable(TFADCChannel Channel, int Count)
{
    if(ADCEnabled)
    {
        if(!Enabled)
        {
			if(Count > MAX_ADC_SAMPLES)
				Count = MAX_ADC_SAMPLES;
			
            if(Channel < ADCPhysicalInputs)
                gp_Analog(&Inputs[Channel]);
            
            Enabled = true;
			ADCCount = Count;
            ADCIndex = Channel;
			
            switch(Channel)
            {
            case FADC_TEMP: // Датчик температуры
                ADC->CCR |= ADC_CCR_TSEN;
                break;
            case FADC_VREF: // Опорное напряжение
                ADC->CCR |= ADC_CCR_VREFEN;
                break;
            default: break;
            }
            
            adc_Reconfigure();
        }
    }
}

// Выключить канал измерения
void fadc_Disable(void)
{
    if(ADCEnabled)
    {
        if(Enabled)
        {
            if(ADCIndex < ADCPhysicalInputs)
                gp_Input(&Inputs[ADCIndex]);
            
            Enabled = false;
            
            switch(ADCIndex)
            {
            case FADC_TEMP: // Датчик температуры
                ADC->CCR &= ~ADC_CCR_TSEN;
                break;
            case FADC_VREF: // Опорное напряжение
                ADC->CCR &= ~ADC_CCR_VREFEN;
                break;
            default: break;
            }
            
            adc_Reconfigure();
        }
    }
}

// Вычитать данные с заданного канала
uint16_t * fadc_Get(void)
{
    if(ADCEnabled)
    {
        if(Enabled)
            return &ADCRes[0];
    }
    return 0;
}

const TDriver fastadc = { "fastadc", Init, Uninit, 0 };
