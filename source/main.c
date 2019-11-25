// ***********************************************************
//  STM32F0 base project
//
//  teplofizik, 2013
// ***********************************************************

#include "drivers.h"
#include "main/test.h"

//uint32_t SystemCoreClock    = 8000000;

//void SystemInit(void)
//{
    
//}

int main(void)
{
    // Инициализация периферии и драйверов
    drivers_Init();
    app_Start(&test);
	// I2C
//	app_Start(&spitest);
//    app_Start(&i2cstest);
//    app_Start(&i2cmtest);
    app_Start(&pwmtest);
//    app_Start(&dactest);
//    app_Start(&uarttest);
//    app_Start(&thototest);
//    app_Start(&dmxtxtest);
    
    while(1)
    {
        drivers_Main();
    }
}
