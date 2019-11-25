// ***********************************************************
//	activity.h
// 	Монитор активности v2.0.0
//
//  Требования: systimer
//  teplofizik, 2016
// ***********************************************************

#include "activity.h"
#include "systick.h"

static int ActivityEnabled = false;
static int Timeouts[ACT_TIMER_COUNT];
static const int Count = sizeof(Timeouts)/sizeof(Timeouts[0]);

static void OnTimer(void)
{
    int i;
    for(i = 0; i < Count; i++) if(Timeouts[i]) Timeouts[i]--;
}

// Загрузка драйвера
static bool Init(void)
{
    int i;
    
    if(!drv_Require(&activity, "systimer")) return false;
    
    for(i = 0; i < Count; i++) Timeouts[i] = 0;
    
    timer_AddFunction(100, &OnTimer);
    ActivityEnabled = true;
    
    return true;
}

// Выгрузка драйвера
static void Uninit(void)
{
    timer_Remove(&OnTimer);
    ActivityEnabled = false;
}

// Сообщить об активности
void act_Activate(TActivity Act)
{
    if(ActivityEnabled)
    {
        if(Act >= Count) return;
            
        Timeouts[Act] = (ACTIVITY_TIME / 10);
    }
}

// Проверить - была ли недавно активность
bool act_Get(TActivity Act)
{
    if(ActivityEnabled)
    {
        if(Act < Count) 
            return Timeouts[Act] > 0;
    }
    
    return false;
}

const TDriver activity = { "activity", &Init, &Uninit, 0 };
