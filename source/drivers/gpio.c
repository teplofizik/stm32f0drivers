// ***********************************************************
//	gpio.c
//  Порты ввода-вывода v2.0.0
//
//  teplofizik, 2016
// ***********************************************************

#include "gpio.h"
#include <stm32f0xx.h>

static GPIO_TypeDef * const Ports[6] = { 
	GPIOA,
	GPIOB, 
	GPIOC, 
	GPIOD, 
#ifdef AVAIL_GPIOE
	GPIOE, 
#endif
	GPIOF };

// Инициализация
static bool Init(void)
{
    // Тактирование во все порты!
    RCC->AHBENR |= 0 |
                    RCC_AHBENR_GPIOAEN |
                    RCC_AHBENR_GPIOBEN |
                    RCC_AHBENR_GPIOCEN |
                    RCC_AHBENR_GPIODEN |
#ifdef AVAIL_GPIOE
                    RCC_AHBENR_GPIOEEN |
#endif
                    RCC_AHBENR_GPIOFEN |
                    0;
    
    return true;
}

// Выгрузка
static void Uninit(void)
{
    // Тактирование во все порты!
    RCC->AHBENR &= ~(0 |
                     RCC_AHBENR_GPIOAEN |
                     RCC_AHBENR_GPIOBEN |
                     RCC_AHBENR_GPIOCEN |
                     RCC_AHBENR_GPIODEN |
#ifdef AVAIL_GPIOE
                    RCC_AHBENR_GPIOEEN |
#endif
                     RCC_AHBENR_GPIOFEN |
                     0);
}

// Настроить вывод на выход
void gp_Output(const TPin * Pin)
{
    uint32_t MODER = Ports[Pin->Port]->MODER;

    // Режим выхода
    // GP Output = 1
    MODER &= ~(0x03UL << (Pin->Pin * 2));
    MODER |=  (0x01UL << (Pin->Pin * 2));
    
    Ports[Pin->Port]->MODER = MODER;
}

// Настроить вывод на вход
void gp_Input(const TPin * Pin)
{
    // Очистим поле
    // GP Input = 0
    Ports[Pin->Port]->MODER &= ~(0x03UL << (Pin->Pin * 2));
}

// Установить для вывода аналоговый режим
void gp_Analog(const TPin * Pin)
{
    // GP Analog = 3
    Ports[Pin->Port]->MODER |= (0x03UL << (Pin->Pin * 2));
}

// Высокий уровень на ножке
void gp_High(const TPin * Pin)
{
    Ports[Pin->Port]->BSRR = (1UL << (Pin->Pin & 0x0F));
}

// Низкий уровень на ножке
void gp_Low(const TPin * Pin)
{
    Ports[Pin->Port]->BSRR = (1UL << ((Pin->Pin & 0x0F) + 16));
}

// Значение на ножке
bool gp_Get(const TPin * Pin)
{
    return ((Ports[Pin->Port]->IDR & (1 << Pin->Pin)) != 0);
}

// Установить значение на ножке
void gp_Set(const TPin * Pin, bool Value)
{
    if(Value)
        gp_High(Pin);
    else
        gp_Low(Pin);
}

// Включить Open Drain (иначе двухтактный)
void gp_OpenDrain(const TPin * Pin, bool Enable)
{
    if(Enable)
        Ports[Pin->Port]->OTYPER |= (1 << Pin->Pin);
    else
        Ports[Pin->Port]->OTYPER &= ~(1 << Pin->Pin);
}

// Максимальная скорость работы вывода: 50 МГц
void gp_FastSpeed(const TPin * Pin)
{
    uint32_t OSPEEDR = Ports[Pin->Port]->OSPEEDR;
    
    OSPEEDR &= ~(3 << (Pin->Pin * 2));
    OSPEEDR |=  (2 << (Pin->Pin * 2)); // 50 MHz
    
    Ports[Pin->Port]->OSPEEDR = OSPEEDR;
}

// Выключить подтяжку
void gp_NoPull(const TPin * Pin)
{
    uint32_t PUPDR = Ports[Pin->Port]->PUPDR;
    
    PUPDR &= ~(0x03UL << (Pin->Pin * 2));
    
    Ports[Pin->Port]->PUPDR = PUPDR;
}

// Включить подтяжку к питанию
void gp_PullUp(const TPin * Pin)
{
    uint32_t PUPDR = Ports[Pin->Port]->PUPDR;
    
    // 1 - подтяжка вверх, 2 - вниз.
    PUPDR &= ~(0x03UL << (Pin->Pin * 2));
    PUPDR |=  (0x01UL << (Pin->Pin * 2));
    
    Ports[Pin->Port]->PUPDR = PUPDR;
}

// Включить подтяжку к земле
void gp_PullDown(const TPin * Pin)
{
    uint32_t PUPDR = Ports[Pin->Port]->PUPDR;
    
    // 1 - подтяжка вверх, 2 - вниз.
    PUPDR &= ~(0x03UL << (Pin->Pin * 2));
    PUPDR |=  (0x02UL << (Pin->Pin * 2));
    
    Ports[Pin->Port]->PUPDR = PUPDR;
}

// Установить для вывода альтернативную функцию (0-3)
void gp_AlternateFunction(const TPin * Pin, uint8_t Function)
{
    uint32_t MODER = Ports[Pin->Port]->MODER;
    
    Function &= 0x0F;
    
    // Only for PA, PB
    if(Pin->Port < 2)
    {
        // Сначала выбираем альтернативную функцию, и только потом на неё переключаемся
        if(Pin->Pin < 8)
        {
            uint8_t P = Pin->Pin & 0x07;
            
            // Очистим, потом запишем
            Ports[Pin->Port]->AFR[0] &= ~(0x0F << (P * 4));
            Ports[Pin->Port]->AFR[0] |= ((uint32_t)Function << (P * 4));
        }
        else
        {
            uint8_t P = Pin->Pin & 0x07;
            
            // Очистим, потом запишем
            Ports[Pin->Port]->AFR[1] &= ~(0x0F << (P * 4));
            Ports[Pin->Port]->AFR[1] |= ((uint32_t)Function << (P * 4));
        }
    }

    // AF = 2
    MODER &= ~(0x03UL << (Pin->Pin * 2));
    MODER |=  (0x02UL << (Pin->Pin * 2));
    
    Ports[Pin->Port]->MODER = MODER;
}

// Одинаковые ли выводы
bool gp_Equals(const TPin * A, const TPin * B)
{
    return (A->Port == B->Port) && (A->Pin == B->Pin);
}

const TDriver gpio = { "gpio", Init, Uninit, 0 };
