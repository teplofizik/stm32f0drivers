// ***********************************************************
//	activity.h
// 	Монитор активности v2.0.0
//
//  Требования: systimer
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _ACTIVITY_H
#define _ACTIVITY_H

    // Количество источников, за активностью которых наблюдаем
    #define ACT_TIMER_COUNT 10
    // Время индикации активности, мс
    #define ACTIVITY_TIME   300

    typedef enum { ACT_TEST } TActivity;

    // Сообщить об активности
    void act_Activate(TActivity Act);
    
    // Проверить - была ли недавно активность
    bool act_Get(TActivity Act);
    
    extern const TDriver activity;
    
#endif
