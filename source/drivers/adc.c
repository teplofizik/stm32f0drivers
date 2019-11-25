// ***********************************************************
//	adc.c
// 	��� v2.1.0
//  ��� �� 16 ������� + ������ ���������, �������� ���������� � 
//  �����������
//
//  ����������: gpio, dma, clock
//  teplofizik, 2016
// ***********************************************************

// TODO: ��������� ������������� ������ ��� ��������� ������ �������

#include "adc.h"
#include "dma.h"
#include "clock.h"
#include <string.h>
#include <stm32f0xx.h>

#define ADC_CHANNEL_COUNT 19

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

// ���������� ���������� ������� (��������� ����������)
static const int ADCPhysicalInputs = sizeof(Inputs) / sizeof(Inputs[0]);

static bool ADCEnabled = false;
// ���������� ������
static bool     Enabled[ADC_CHANNEL_COUNT];
// ������ ��� ����������� ���������
static uint16_t ADCRes[ADC_CHANNEL_COUNT];
// ������ ������� ���������� �������
static uint8_t  Indexes[ADC_CHANNEL_COUNT];
// �������� ������� ������������ (�� ������� ���������� ����� � ������� ADCRes)
static uint8_t  RevTable[ADC_CHANNEL_COUNT];

static int ADCCount;

void adc_InitDMA(int Count)
{
    if(Count)
        dma_StartCircular(DMA_ADC, &ADCRes[0], DMA_SIZE_16, Count);
    else
        dma_Stop(DMA_ADC);
}

// ������������ ������� ���������
static void adc_Reconfigure(void)
{
    // �������� ���
    if(ADCCount > 0)
    {
        ADC1->CFGR1 &= ~ADC_CFGR1_CONT;
        dma_Stop(DMA_ADC);
    }
    
    //ADC1->CHSELR = ADC_CHSELR_CHSEL0 | ADC_CHSELR_CHSEL1 | ADC_CHSELR_CHSEL2;
    
    {
        uint32_t CHSELR = 0;
        int i;
        int ADCTIndex = 0;
        
        for(i = 0; i < ADC_CHANNEL_COUNT; i++)
        {
            if(Enabled[i])
            {
                Indexes[ADCTIndex] = i;
                ADCRes[ADCTIndex] = 0;
                RevTable[i] = ADCTIndex;
                
                CHSELR |= (1 << i);
                
                ADCTIndex++;
            }
        }
        
        ADCCount = ADCTIndex;
        
        ADC1->CHSELR = CHSELR;
    }
    
    // ������� ���������
    if(ADCCount > 0)
    {
        ADC1->CR |= ADC_CR_ADEN;
        ADC1->SMPR = 4; 
        ADC1->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG; // DMA en, circ
        ADC1->CFGR1 |= ADC_CFGR1_CONT;
        ADC1->CR |= ADC_CR_ADSTART;
    }
    else
    {
        ADC1->CR = 0;
    }
    // �������� DMA
    adc_InitDMA(ADCCount);
}

// ���
static bool Init(void)
{
    if(!drv_Require(&adc, "clock")) return false;
    if(!drv_Require(&adc, "gpio")) return false;
    if(!drv_Require(&adc, "dma")) return false;
    
    memset(ADCRes, 0, sizeof(ADCRes));
    memset(Indexes, 0, sizeof(Indexes));
    memset(Enabled, 0, sizeof(Enabled));
    memset(RevTable, 0, sizeof(RevTable));
    
    ADCCount = 0;
    ADCEnabled = true;
    
    // ������� � ������� ���
	clock_EnablePeripheral(PR_ADC);
	clock_ResetPeripheral(PR_ADC);
	
    // ��� ���������, ����� ����� ����
    
    return true;
}

// �������� �������� ���
static void Uninit(void)
{
    // ��������� ������������
	clock_DisablePeripheral(PR_ADC);
    
    // ����, ��� ������� �� ��������
    ADCEnabled = false;
    
    // ��������� DMA
    adc_InitDMA(0);
}

// �������� ����� ������ (TADCChannel) �� ������ ��� -1, ���� ��� ��� ������ ���
int adc_GetChannel(const TPin * Pin)
{
    int i;
    for(i = 0; i < ADC_CHANNEL_COUNT; i++)
    {
        if(gp_Equals(Pin, &Inputs[i]))
            return i;
    }
    
    return -1;
}
    
// �������� ����� ���������
void adc_Enable(TADCChannel Channel)
{
    if(ADCEnabled)
    {
        if(!Enabled[Channel])
        {
            if(Channel < ADCPhysicalInputs)
                gp_Analog(&Inputs[Channel]);
            
            Enabled[Channel] = true;
            
            switch(Channel)
            {
            case ADC_TEMP: // ������ �����������
                ADC->CCR |= ADC_CCR_TSEN;
                break;
            case ADC_VREF: // ������� ����������
                ADC->CCR |= ADC_CCR_VREFEN;
                break;
            default: break;
            }
            
            adc_Reconfigure();
        }
    }
}

// ��������� ����� ���������
void adc_Disable(TADCChannel Channel)
{
    if(ADCEnabled)
    {
        if(Enabled[Channel])
        {
            if(Channel < ADCPhysicalInputs)
                gp_Input(&Inputs[Channel]);
            
            Enabled[Channel] = false;
            
            switch(Channel)
            {
            case ADC_TEMP: // ������ �����������
                ADC->CCR &= ~ADC_CCR_TSEN;
                break;
            case ADC_VREF: // ������� ����������
                ADC->CCR &= ~ADC_CCR_VREFEN;
                break;
            default: break;
            }
            
            adc_Reconfigure();
        }
    }
}

// �������� ������ � ��������� ������
int adc_Get(TADCChannel Channel)
{
    if(ADCEnabled)
    {
        if(Enabled[Channel])
            return ADCRes[RevTable[Channel]];
    }
    return 0;
}

const TDriver adc = { "adc", Init, Uninit, 0 };
