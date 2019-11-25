// ***********************************************************
//	gpio.c
//  ����� �����-������ v2.0.0
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

// �������������
static bool Init(void)
{
    // ������������ �� ��� �����!
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

// ��������
static void Uninit(void)
{
    // ������������ �� ��� �����!
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

// ��������� ����� �� �����
void gp_Output(const TPin * Pin)
{
    uint32_t MODER = Ports[Pin->Port]->MODER;

    // ����� ������
    // GP Output = 1
    MODER &= ~(0x03UL << (Pin->Pin * 2));
    MODER |=  (0x01UL << (Pin->Pin * 2));
    
    Ports[Pin->Port]->MODER = MODER;
}

// ��������� ����� �� ����
void gp_Input(const TPin * Pin)
{
    // ������� ����
    // GP Input = 0
    Ports[Pin->Port]->MODER &= ~(0x03UL << (Pin->Pin * 2));
}

// ���������� ��� ������ ���������� �����
void gp_Analog(const TPin * Pin)
{
    // GP Analog = 3
    Ports[Pin->Port]->MODER |= (0x03UL << (Pin->Pin * 2));
}

// ������� ������� �� �����
void gp_High(const TPin * Pin)
{
    Ports[Pin->Port]->BSRR = (1UL << (Pin->Pin & 0x0F));
}

// ������ ������� �� �����
void gp_Low(const TPin * Pin)
{
    Ports[Pin->Port]->BSRR = (1UL << ((Pin->Pin & 0x0F) + 16));
}

// �������� �� �����
bool gp_Get(const TPin * Pin)
{
    return ((Ports[Pin->Port]->IDR & (1 << Pin->Pin)) != 0);
}

// ���������� �������� �� �����
void gp_Set(const TPin * Pin, bool Value)
{
    if(Value)
        gp_High(Pin);
    else
        gp_Low(Pin);
}

// �������� Open Drain (����� �����������)
void gp_OpenDrain(const TPin * Pin, bool Enable)
{
    if(Enable)
        Ports[Pin->Port]->OTYPER |= (1 << Pin->Pin);
    else
        Ports[Pin->Port]->OTYPER &= ~(1 << Pin->Pin);
}

// ������������ �������� ������ ������: 50 ���
void gp_FastSpeed(const TPin * Pin)
{
    uint32_t OSPEEDR = Ports[Pin->Port]->OSPEEDR;
    
    OSPEEDR &= ~(3 << (Pin->Pin * 2));
    OSPEEDR |=  (2 << (Pin->Pin * 2)); // 50 MHz
    
    Ports[Pin->Port]->OSPEEDR = OSPEEDR;
}

// ��������� ��������
void gp_NoPull(const TPin * Pin)
{
    uint32_t PUPDR = Ports[Pin->Port]->PUPDR;
    
    PUPDR &= ~(0x03UL << (Pin->Pin * 2));
    
    Ports[Pin->Port]->PUPDR = PUPDR;
}

// �������� �������� � �������
void gp_PullUp(const TPin * Pin)
{
    uint32_t PUPDR = Ports[Pin->Port]->PUPDR;
    
    // 1 - �������� �����, 2 - ����.
    PUPDR &= ~(0x03UL << (Pin->Pin * 2));
    PUPDR |=  (0x01UL << (Pin->Pin * 2));
    
    Ports[Pin->Port]->PUPDR = PUPDR;
}

// �������� �������� � �����
void gp_PullDown(const TPin * Pin)
{
    uint32_t PUPDR = Ports[Pin->Port]->PUPDR;
    
    // 1 - �������� �����, 2 - ����.
    PUPDR &= ~(0x03UL << (Pin->Pin * 2));
    PUPDR |=  (0x02UL << (Pin->Pin * 2));
    
    Ports[Pin->Port]->PUPDR = PUPDR;
}

// ���������� ��� ������ �������������� ������� (0-3)
void gp_AlternateFunction(const TPin * Pin, uint8_t Function)
{
    uint32_t MODER = Ports[Pin->Port]->MODER;
    
    Function &= 0x0F;
    
    // Only for PA, PB
    if(Pin->Port < 2)
    {
        // ������� �������� �������������� �������, � ������ ����� �� �� �������������
        if(Pin->Pin < 8)
        {
            uint8_t P = Pin->Pin & 0x07;
            
            // �������, ����� �������
            Ports[Pin->Port]->AFR[0] &= ~(0x0F << (P * 4));
            Ports[Pin->Port]->AFR[0] |= ((uint32_t)Function << (P * 4));
        }
        else
        {
            uint8_t P = Pin->Pin & 0x07;
            
            // �������, ����� �������
            Ports[Pin->Port]->AFR[1] &= ~(0x0F << (P * 4));
            Ports[Pin->Port]->AFR[1] |= ((uint32_t)Function << (P * 4));
        }
    }

    // AF = 2
    MODER &= ~(0x03UL << (Pin->Pin * 2));
    MODER |=  (0x02UL << (Pin->Pin * 2));
    
    Ports[Pin->Port]->MODER = MODER;
}

// ���������� �� ������
bool gp_Equals(const TPin * A, const TPin * B)
{
    return (A->Port == B->Port) && (A->Pin == B->Pin);
}

const TDriver gpio = { "gpio", Init, Uninit, 0 };
