// ***********************************************************
//	activity.h
// 	������� ���������� v2.0.0
//
//  ����������: systimer
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

// �������� ��������
static bool Init(void)
{
    int i;
    
    if(!drv_Require(&activity, "systimer")) return false;
    
    for(i = 0; i < Count; i++) Timeouts[i] = 0;
    
    timer_AddFunction(100, &OnTimer);
    ActivityEnabled = true;
    
    return true;
}

// �������� ��������
static void Uninit(void)
{
    timer_Remove(&OnTimer);
    ActivityEnabled = false;
}

// �������� �� ����������
void act_Activate(TActivity Act)
{
    if(ActivityEnabled)
    {
        if(Act >= Count) return;
            
        Timeouts[Act] = (ACTIVITY_TIME / 10);
    }
}

// ��������� - ���� �� ������� ����������
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
