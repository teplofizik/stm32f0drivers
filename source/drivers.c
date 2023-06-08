// ***********************************************************
//	drivers.h
// 	Инициализация всей необходимой периферии v2.0.0
//
//  teplofizik, 2016
// ***********************************************************

#include "drivers.h"

static const TDriver * Drivers[] = {
	&clock,        // "clock"
    &lowlevelsys,  // "lowlevelsys"
    &memory,       // "memory"
	&flash,        // "flash"
	&gpio,         // "gpio"
	&exti,         // "exti",
	&timer,        // "timer"
    &systimer,     // "systimer"
    &activity,     // "activity"
    &dma,          // "dma"
    &adc,          // "adc"
    &dac,          // "dac"
    &uart,         // "uart"
	&i2c,          // "i2c"
	&i2cs,         // "i2cs"
	&i2cm,         // "i2cm"
	&ledpwm,       // "ledpwm"
	&ledfastpwm,   // "ledfastpwm"
	&touch,        // "touch"
	&spi,          // "spi"
	&surx,         // "surx"
    0
};

// Загрузить драйвера по списку
void drivers_Load(const TDriver ** List)
{
    const TDriver ** D = List;
    while(*D)
    {
        drv_Load(*D);
        D++;
    }
}

// Инициализация драйверов
void drivers_Init(void)
{
    drv_Init();
#ifdef THOTO
	drv_Load(&thoto);
#endif
    drivers_Load(&Drivers[0]);
    drivers_Load(&BSP[0]);
    
    // Разрешим прерывания, иначе таймер работать не будет
    irq_enable();
    
    app_Init();
	
}

void drivers_Main(void)
{
    drv_Main();
    app_Main();
}

// Программная задержка
void delay(uint32_t Value)
{
    volatile uint32_t Cycles = Value;

    while(Cycles--);
}
