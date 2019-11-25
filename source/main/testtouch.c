// ***********************************************************
//	buttons.h
//  Опрос кнопок
// 
//  teplofizik, 2017
// ***********************************************************

#include "buttons.h"
#include "../drivers.h"

// Куда подключены измерительные конденсаторы
static const TPin Caps[] = {
	{PA, 2},
	{PA, 6}
};

// Куда подключены кнопки
static const TPin Buttons[] = {
	{PA, 1},
	{PA, 0},
	{PA, 5},
	{PA, 4},
	{PA, 3}
};

static const int CapsCount = sizeof(Caps) / sizeof(Caps[0]);
static const int ButtonsCount = sizeof(Buttons) / sizeof(Buttons[0]);
static bool OldState[ButtonsCount];

static TButtonHandler ButtonHandler = 0;

static void ButtonTimer(void)
{
	int i;
	for(i = 0; i < ButtonsCount; i++)
	{
		bool State = touch_IsActivated(&Buttons[i]);
		if(State != OldState[i])
		{
			OldState[i] = State;
			
			if(State)
				if(ButtonHandler) ButtonHandler(i);
		}
	}
}

void but_SetHandler(TButtonHandler Handler)
{
	ButtonHandler = Handler;
}

// Инициализация кнопок
void but_Init(void)
{
	int i;
	
	for(i = 0; i < CapsCount; i++) touch_ConfigCap(&Caps[i]);
	for(i = 0; i < ButtonsCount; i++) 
	{
		touch_ConfigIn(&Buttons[i], 0);
		OldState[i] = false;
	}
	
	timer_AddFunction(10, ButtonTimer);
	
}
