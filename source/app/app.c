// ***********************************************************
//	app.c
// 	Базовый код для работы с приложениями
//
//  teplofizik, 2016
// ***********************************************************

#include "app.h"
#include "../drivers.h"
#include <string.h>
#include <stdio.h>

const TApp * Apps[MAX_APP_COUNT];

// Инициализация подсистемы приложений
void app_Init(void)
{
    memset(Apps, 0, sizeof(Apps));
}

// Удалить из списка
static void app_Remove(const TApp * App)
{
    int i;
    if(App->Uninit) App->Uninit();
    
    for(i = 0; i < MAX_APP_COUNT; i++)
    {
        if(Apps[i] == App)
        {
            Apps[i] = 0;
            break;
        }
    }
}

// Получить указатель на указатель на приложение
const TApp ** app_Get(const TApp * App)
{
    int i;
    for(i = 0; i < MAX_APP_COUNT; i++)
    {
        if(Apps[i] == App)
            return &Apps[i];
    }
    
    return 0;
}

// Главный цикл для приложений
void app_Main(void)
{
    int i;
    for(i = 0; i < MAX_APP_COUNT; i++)
    {
        const TApp * A = Apps[i];
        if(A && A->Main)
        {
            if(!A->Main())
            {
                // Application stopped
                app_Remove(A);
            }
        }
    }
}

// Загрузить приложение
bool app_Start(const TApp * App)
{
    const char ** D = App->Required;
    
    if(app_Get(App)) 
        // ERROR_ALREADY_RUNNING
        return false;
    
    // Проверка драйверов, если нужна
    if(D)
    {
        while(*D)
        {
            if(!drv_IsLoaded(*D))
            {
                // ERROR_REQUIRED_DRIVER_NOT_AVAILABLE
                return false;
            }
            D++;
        }
    }
    
    if(App->Init())
    {
        int i;
        for(i = 0; i < MAX_APP_COUNT; i++)
        {
            if(!Apps[i])
            {
                Apps[i] = App;
                // ERROR_OK
                return true;
            }
        }
    }
    
    // ERROR_NO_FREE_SLOT
    return false;
}

// Остановить приложение
void app_Stop(const TApp * App)
{
    int i;
    for(i = 0; i < MAX_APP_COUNT; i++)
    {
        if(Apps[i] == App)
        {
            if(App->Uninit) App->Uninit();
            Apps[i] = 0;
            break;
        }
    }
}
