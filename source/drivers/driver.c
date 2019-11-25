// ***********************************************************
//	driver.c
// 	Базовый код для работы с драйверами
//
//  teplofizik, 2016
// ***********************************************************

#include "driver.h"
#include <string.h>
#include <stdio.h>

const TDriver * Drivers[MAX_DRIVER_COUNT];

// Строка в лог
void drv_DebugLog(const TDriver * Drv, const char * Text)
{
    char Temp[100];
    sprintf(Temp, Text, Drv->Name);
}

// Инициализация подсистемы драйверов
void drv_Init(void)
{
    memset(Drivers, 0, sizeof(Drivers));
}

// Драйвер загружен?
bool drv_IsLoaded(const char * name)
{
    int i;
    for(i = 0; i < MAX_DRIVER_COUNT; i++)
    {
        if(Drivers[i])
        
        if(!strcmp(Drivers[i]->Name, name)) return true;
    }
    
    return false;
}

// Требование наличия драйвера
// Если не загружен - false
bool drv_Require(const TDriver * Drv, const char * name)
{
    int i;
    for(i = 0; i < MAX_DRIVER_COUNT; i++)
    {
        if(Drivers[i])
        
        if(!strcmp(Drivers[i]->Name, name)) return true;
    }
    
    {
        char temp[100];
        sprintf(temp, "Driver '%%s' could not be loaded: required driver '%s' not loaded", name);
        drv_DebugLog(Drv, temp);
    }
    return false;
}

// Удалить из списка
static void drv_Remove(const TDriver * Drv)
{
    int i;
    if(Drv->Uninit) Drv->Uninit();
    
    for(i = 0; i < MAX_DRIVER_COUNT; i++)
    {
        if(Drivers[i] == Drv)
        {
            Drivers[i] = 0;
            break;
        }
    }
}

// Основной цикл для драйверов
void drv_Main(void)
{
    int i;
    for(i = 0; i < MAX_DRIVER_COUNT; i++)
    {
        const TDriver * D = Drivers[i];
        if(D && D->Main)
        {
            if(!D->Main())
            {
                drv_DebugLog(D, "Driver '%s' unloaded due to error in Driver->Main\n");
                drv_Remove(D);
            }
        }
    }
}

// Загрузить драйвер
void drv_Load(const TDriver * Drv)
{
    if(Drv->Init())
    {
        int i;
        for(i = 0; i < MAX_DRIVER_COUNT; i++)
        {
            if(!Drivers[i])
            {
                Drivers[i] = Drv;
                break;
            }
        }
    }
    else
        drv_DebugLog(Drv, "Driver '%s' not loaded due to error in Driver->Init");
}
