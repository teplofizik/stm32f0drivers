// ***********************************************************
//	upgrade.c
// 	Обновление ПО
//
//  teplofizik, 2014
// ***********************************************************

#include "upgrade.h"
#include "../drivers.h"
#include "../bsp.h"
#include <string.h>

static bool         UpgradeEnabled = false;
static TLockHandler LockHandler;
// прошивка
static uint8_t *    NeedDMXProgram = (uint8_t *)0;
static bool         NeedErase = false;

// Проверка
static bool         Checksum = false;
static uint32_t     TotalLength = 0;

// Прошить основное ПО?
static bool         NeedUpgrade = false;

// TODO: ADDRESS SELECT
uint8_t *           TempFirmware = (uint8_t *)0x08008000;
const uint32_t      TempFirmwareSize = 0x8000;

void rupg_Reprogram(uint32_t Length);

// Стирание временного хранилища под прошивку
void upg_Erase(void)
{
    if(UpgradeEnabled)
        NeedErase = true;
}

// Записать кусок прошивки (32 б)
void upg_Write(uint32_t Offset, uint8_t * Data, uint32_t Length)
{
    if(UpgradeEnabled)
    {
        uint8_t FirmwareBuffer[16];
        if(Length <= sizeof(FirmwareBuffer))
        {
            uint8_t * NeedProgram;
            uint32_t ProgramLength = 0;
            
            memcpy(&FirmwareBuffer[0], Data, Length);
            
            NeedProgram = &TempFirmware[Offset];
            ProgramLength = Length;
            
            __disable_irq();
            flash_Write(&FirmwareBuffer[0], NeedProgram, ProgramLength);
            __enable_irq();
            
            NeedProgram = 0;
            ProgramLength = 0;
        }
    }
}

// Записать кусок прошивки с потока DMX
void upg_DMXWrite(uint32_t Offset)
{
    if(UpgradeEnabled)
        NeedDMXProgram = &TempFirmware[Offset];
}

// Проверить контрольную сумму
bool upg_Checksum(uint32_t Length, uint32_t Sum)
{
    if(UpgradeEnabled)
    {
        uint16_t CS = 0;
        int i;

        TotalLength = Length;
        
        for(i = 0; i < TotalLength; i++) CS += TempFirmware[i];
        if(CS == Sum) Checksum = true;

        return Sum == CS;
    }
    else
        return false;
}

// Обновить основное ПО
void upg_Upgrade(void)
{
    if(UpgradeEnabled)
        NeedUpgrade = true;
}

// Занято?
bool upg_Busy(void)
{
    return UpgradeEnabled && NeedUpgrade;
}

// Инициализация
bool upg_Init(void)
{
    UpgradeEnabled = true;
    
    NeedUpgrade = false;
    NeedErase = false;
    
    return true;
}

void upg_Uninit(void)
{
    UpgradeEnabled = false;
    
    NeedUpgrade = false;
    NeedErase = false;
}

void upg_OnDMX(const uint8_t * DMX)
{
    if(UpgradeEnabled)
    {
        if(!NeedDMXProgram) return;
        
        __disable_irq();
        flash_Write(DMX, NeedDMXProgram, 512);
        __enable_irq();
        NeedDMXProgram = (uint8_t *)0;
    }
}

void upg_SetLockCallback(TLockHandler Handler)
{
    if(UpgradeEnabled)
        LockHandler = Handler;
}

bool upg_Main(void)
{
    if(LockHandler && LockHandler()) return true;
    
    if(NeedErase)
    {
        flash_Erase(TempFirmware, &TempFirmware[TempFirmwareSize]);
        NeedErase = false;
    }
    if(NeedUpgrade)
    {
        if(Checksum && TotalLength) rupg_Reprogram(TotalLength);
        NeedUpgrade = false;
    }
    
    return true;
}

// Обновление ПО
const TDriver upgrade = { "upgrade", &upg_Init, &upg_Uninit, &upg_Main };
