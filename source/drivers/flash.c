// ***********************************************************
//	flash.c
//  ������ � ����������������� ������� v2.1.0
//
//  teplofizik, 2016
// ***********************************************************

#include "flash.h"
#include <stm32f0xx.h>
#include <string.h>

// ������ ��������: 0x400
// ������� � �������: 4 (������ 0x1000)

static const int PageCount = 64; // 64k

// ����� ��������
static int8_t GetPageIndex(void * Address)
{
    return ((uint32_t)Address >> 10);
}

// ������������� ������� �� Flash
static void flash_Unlock(void)
{
    if(FLASH->CR & FLASH_CR_LOCK)
    {
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
    }
}

// ���������� ������� �� Flash
static void flash_Lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

static bool flash_CheckEmpty(uint8_t Index)
{
    if(Index >= PageCount) return true;
    
    {
        uint8_t * S = (uint8_t *)(0x08000000 + (Index * 0x400));
        int i;
        
        for(i = 0; i < 0x400; i++) if(S[i] != 0xFF) return false;
    }
    
    return true;
}

// �������� ��������
static void flash_ClearPage(uint8_t Index)
{
    if(Index >= PageCount) return;
    if(flash_CheckEmpty(Index)) return;
    
    // ���� ����������
    flash_Unlock();
    while(FLASH->SR & FLASH_SR_BSY) {}
    
    // ��������� �������� ��������
    FLASH->CR |= FLASH_CR_PER;
    // ����� ��������
    FLASH->AR = 0x08000000 + (Index * 0x400);
    
    // ��������� ����������������
    FLASH->CR |= FLASH_CR_STRT;
    
    // �������� ����������
    while(!(FLASH->SR & FLASH_SR_EOP)) {}
        
    // �������� ���� ���������� ����������������
    FLASH->SR = FLASH_SR_EOP;
    FLASH->CR &= ~FLASH_CR_PER;
        
    flash_Lock();
}

// ������� ������ ����� �������
void flash_Erase(void * From, void * To)
{
    int FromPage = GetPageIndex(From);
    int ToPage = GetPageIndex(To);
    int i;
    
    for(i = FromPage; i <= ToPage; i++) flash_ClearPage(i);
}

// ������ ������ �� FLASH
void flash_Read(void * Dest, const void * Address, uint32_t Length)
{
    memcpy(Dest, Address, Length);
}
    
// �������� ������ �� FLASH
void flash_Write(const void * Src, void * Address, uint32_t Length)
{
    uint16_t       * D = (uint16_t *)Address;
    const uint16_t * S = (const uint16_t *)Src;
    int i;
    
    flash_Unlock();
    
    // ��������� ����������������
    FLASH->CR |= FLASH_CR_PG;
    
    for(i = 0; i < Length / 2; i++)
    {
        *D = *S;
        while(FLASH->SR & FLASH_SR_BSY) {}

        if(*D != *S) { /* error */ }
        D++; S++;
    }
    
    FLASH->CR &= ~FLASH_CR_PG;
    flash_Lock();
}


static bool Init(void)
{
    return true;
}

static void Uninit(void)
{
    
}

const TDriver flash = {"flash", &Init, &Uninit, 0 };
