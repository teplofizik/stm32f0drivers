// ***********************************************************
//	flash.c
//  Работа с энергонезависимой памятью v2.1.0
//
//  teplofizik, 2016
// ***********************************************************

#include "flash.h"
#include <stm32f0xx.h>
#include <string.h>

// Размер страницы: 0x400
// Страниц в секторе: 4 (размер 0x1000)

static const int PageCount = 64; // 64k

// Номер страницы
static int8_t GetPageIndex(void * Address)
{
    return ((uint32_t)Address >> 10);
}

// Разблокировка доступа ко Flash
static void flash_Unlock(void)
{
    if(FLASH->CR & FLASH_CR_LOCK)
    {
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
    }
}

// Блокировка доступа ко Flash
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

// Очистить страницу
static void flash_ClearPage(uint8_t Index)
{
    if(Index >= PageCount) return;
    if(flash_CheckEmpty(Index)) return;
    
    // Ждем готовности
    flash_Unlock();
    while(FLASH->SR & FLASH_SR_BSY) {}
    
    // Разрешить стирание страницы
    FLASH->CR |= FLASH_CR_PER;
    // Номер страницы
    FLASH->AR = 0x08000000 + (Index * 0x400);
    
    // Разрешить программирование
    FLASH->CR |= FLASH_CR_STRT;
    
    // Ожидание завершения
    while(!(FLASH->SR & FLASH_SR_EOP)) {}
        
    // Сбросить флаг завершения программирования
    FLASH->SR = FLASH_SR_EOP;
    FLASH->CR &= ~FLASH_CR_PER;
        
    flash_Lock();
}

// Очистка памяти перед записью
void flash_Erase(void * From, void * To)
{
    int FromPage = GetPageIndex(From);
    int ToPage = GetPageIndex(To);
    int i;
    
    for(i = FromPage; i <= ToPage; i++) flash_ClearPage(i);
}

// Читать память из FLASH
void flash_Read(void * Dest, const void * Address, uint32_t Length)
{
    memcpy(Dest, Address, Length);
}
    
// Записать память во FLASH
void flash_Write(const void * Src, void * Address, uint32_t Length)
{
    uint16_t       * D = (uint16_t *)Address;
    const uint16_t * S = (const uint16_t *)Src;
    int i;
    
    flash_Unlock();
    
    // Разрешить программирование
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
