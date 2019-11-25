// ***********************************************************
//	dac.c
// 	ЦАП v2.0.0
//  ЦАП на два канала
//
//  Требования: gpio, clock
//  teplofizik, 2016
// ***********************************************************

#include "dac.h"
#include "gpio.h"
#include "clock.h"
#include <stm32f0xx.h>
#include <string.h>

static const TPin Outputs[] = {
	{PA, 4},
#ifdef AVAIL_DAC2
	{PA, 5}
#endif
};
static const int DACOutputCount = sizeof(Outputs) / sizeof(Outputs[0]);

static bool DACEnabled = false;
static bool Enabled[DACOutputCount];

// Включить канал ЦАП
void dac_Enable(TDACChannel Channel)
{
    if(DACEnabled && (Channel < DACOutputCount))
    {
        if(!Enabled[Channel])
        {
            Enabled[Channel] = true;
            dac_Set(Channel, 0);
            gp_Analog(&Outputs[Channel]);
            switch(Channel)
            {
            case DAC_OUT1: DAC->CR |= DAC_CR_EN1; break;
#ifdef AVAIL_DAC2
            case DAC_OUT2: DAC->CR |= DAC_CR_EN2; break;
#endif
            }
        }
    }
}

// Выключить канал ЦАП
void dac_Disable(TDACChannel Channel)
{
    if(DACEnabled && (Channel < DACOutputCount))
    {
        if(Enabled[Channel])
        {
            gp_Input(&Outputs[Channel]);
            switch(Channel)
            {
            case DAC_OUT1: DAC->CR &= ~DAC_CR_EN1; break;
#ifdef AVAIL_DAC2
            case DAC_OUT2: DAC->CR &= ~DAC_CR_EN2; break;
#endif
            }
            Enabled[Channel] = false;
        }
    }
}
    
// Задать значение выхода ЦАП 8 бит
void dac_Set(TDACChannel Channel, uint8_t Value)
{
    if(DACEnabled && (Channel < DACOutputCount))
    {
        if(Enabled[Channel])
        {
            switch(Channel)
            {
            case DAC_OUT1: DAC->DHR8R1 =  Value; break;
#ifdef AVAIL_DAC2
            case DAC_OUT2: DAC->DHR8R2 =  Value; break;
#endif
            }
        }
    }
}

// Задать значение выхода ЦАП 12 бит
void dac_Set12(TDACChannel Channel, uint16_t Value)
{
    if(DACEnabled && (Channel < DACOutputCount))
    {
        if(Enabled[Channel])
        {
            switch(Channel)
            {
            case DAC_OUT1: DAC->DHR12R1 = Value; break;
#ifdef AVAIL_DAC2
            case DAC_OUT2: DAC->DHR12R2 = Value; break;
#endif
            }
        }
    }
}

static bool Init(void)
{
    if(!drv_Require(&dac, "gpio")) return false;
    if(!drv_Require(&dac, "clock")) return false;
    
    clock_EnablePeripheral(PR_DAC);
    
    memset(Enabled, 0, sizeof(Enabled));
    
    DACEnabled = true;
    return true;
}

static void Uninit(void)
{
    int i;
    clock_DisablePeripheral(PR_DAC);
	
    for(i = 0; i < DACOutputCount; i++)
        if(Enabled[i]) 
            dac_Disable((TDACChannel)i);

    DACEnabled = false;
}

const TDriver dac = { "dac", Init, Uninit, 0 };
