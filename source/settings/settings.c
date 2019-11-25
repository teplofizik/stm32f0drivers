// ******************************************************
// settings.c
//
// teplofizik
// ******************************************************

#include "settings.h"
#include "settings_ll.h"
#include "../drivers.h"
#include <string.h>

bool SettingsEnabled = false;
TSettings Settings;

const TRDMUniqueBlock RDMDefault = {
    // RDM Address
    { 0x08, 0x9E, 0x00, 0x00, 0x00, 0x00 },
    // Y M D
    16, 12, 01,
    // Block
    0
};

const TDMXBlock DMXDefault = {
    // Personality, Start Address, SA Phy
    1, 1, 1,
    // Flags
    0,
    // Source: DMX
    0, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Manual
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Auto
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Mapping
    // Label
    "Dimmer"
};

// Сброс настроек
void sett_Reset(void)
{
    if(SettingsEnabled)
    {
        
    }
}

static void OnTimer(void)
{
    
}

static bool sett_LoadBlock(TBlockID ID, void * Block, const void * Default, int Length, bool * LoadedFlag)
{
    // Загрузить данные из блока
    if(!sb_Load(ID, Block, Length))
    {
        if(Default)
            // Нет настроек среди сохраненных
            memcpy(Block, Default, Length);
        else
            return false;
    }
    
    *LoadedFlag = true;
    return true;
}

static bool sett_Init(void)
{
    memset(&Settings, 0, sizeof(Settings));
    
    SettingsEnabled = true;
    timer_AddFunction(10, &OnTimer);
    
    // Загрузка настроек (из сохраненных или дефолтных
    sett_LoadBlock(SETT_RDM_UNIQUE, &Settings.RDM, &RDMDefault, sizeof(Settings.RDM), &Settings.RDMAvailable);
    sett_LoadBlock(SETT_DMX, &Settings.DMX, &DMXDefault, sizeof(Settings.DMX), &Settings.DMXAvailable);
    
    return true;
}

static void sett_Uninit(void)
{
    SettingsEnabled = false;
    timer_Remove(&OnTimer);
}

const TDriver settings = { "settings", &sett_Init, &sett_Uninit, 0 };
