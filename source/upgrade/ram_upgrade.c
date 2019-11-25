//
// Main firmware upgrade
//
#include <stm32f0xx.h>
#include <stdbool.h>

// ����� ��������
static int8_t GetPageIndex(uint32_t Address)
{
    return ((uint32_t)Address >> 10) & 0xFF;
}

// ����������� ��������
static void delay(uint32_t Value)
{
    volatile uint32_t Cycles = Value;
    while(Cycles--);
}

// ������������� ������� �� Flash
static void rupg_Unlock(void)
{
    if(FLASH->CR & FLASH_CR_LOCK)
    {
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
    }
}

// ���������� ������� �� Flash
static void rupg_Lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

static bool rupg_ErasePage(uint32_t Address)
{
    // ����� ��������
    FLASH->AR = Address;
    
    // ��������� ����������������
    FLASH->CR |= FLASH_CR_STRT;
    
    // �������� ����������
    while(!(FLASH->SR & FLASH_SR_EOP)) {}
        
    // �������� ���� ���������� ����������������
    FLASH->SR = FLASH_SR_EOP;
        
    // ��������:
    {
        uint8_t * S = (uint8_t *)(Address & 0xFFFFFC00);
        int i;
        
        for(i = 0; i < 0x400; i++) if(S[i]!= 0xFF) return false;
    }
    
    return true;
}

static void rupg_EraseMain(int To)
{
    int i;
    // ��������� �������� ��������
    FLASH->CR = FLASH_CR_PER;
    
    for(i = 0; i <= To; i++)
    {
        int Try = 2;
        
        while(Try--)
        {
            if(rupg_ErasePage(0x08000200 + (i * 0x400))) break;
        }
        
        delay(5000);
    }
    FLASH->CR = 0;
}

static void rupg_Copy(int Length)
{
    int i;
    uint16_t * D = (uint16_t *)0x08000000;
    uint16_t * S = (uint16_t *)0x08008000;
    Length = ((Length + 1) / 2);
    
    // ��������� ����������������
    FLASH->CR = FLASH_CR_PG;
    
    for(i = 0; i < Length; i++)
    {
        *D = *S;
        while(FLASH->SR & FLASH_SR_BSY) {}
        
        D++; S++;
    }
    
    // ��������� ����������������
    FLASH->CR = 0;
}

void rupg_Reprogram(uint32_t Length)
{
    // ������� ���������� �����
    __disable_irq();
    
    // �����
    rupg_Unlock();
    
    // ������ �������� ��������
    rupg_EraseMain(GetPageIndex(Length));
    
    // ��������� ����� �������� �� ����� �������
    rupg_Copy(Length);
    
    rupg_Lock();
    NVIC_SystemReset();
}


