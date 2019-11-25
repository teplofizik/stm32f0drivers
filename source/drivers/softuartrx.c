// ***********************************************************
//	softuartrx.c
//  ������� �����, ��������������
// 
//  ����������: gpio, timer
//  teplofizik, 2017
// ***********************************************************

#include "softuartrx.h"
#include "gpio.h"
#include "timer.h"
#include <stm32f0xx.h>

// ����� �������
static const TTimer Timer = T15;
static const int Frequency = 9600;

typedef struct
{
	bool           ReceiveState;
	uint8_t        BitIndex;
	uint16_t       TempBuffer;
	
	// ������� ��������
	int            CharTimeout;
} TSURXState;

const TPin SUPins[] = {
 { PB, 0 },
 { PB, 1 },
 { PB, 2 },
 { PB, 3 },
};

const int SUCount = sizeof(SUPins) / sizeof(SUPins[0]);

TSURXState SURxStates[SUCount];
TSuCharHandler SuHandler;

TPin Debug = {PA, 5};

static void OnChar(int Index, uint8_t Char)
{
	if(SuHandler) SuHandler(Index, Char);
}

// �������� ���������� ����������
void TIM15_IRQHandler(void)  
{
	int i;
    TIM15->SR &= ~TIM_SR_UIF;
	
	//gp_High(&Debug);
	// ��������� ��� �� ������������ � TempBuffer.
	// ����� blen ���� ������, 8 ���� (0-7)
	// � �������� ���, 8 ���. ��� ���, ����� ��������.
	// ���� ���� �������� ��������, �� ��������������� ��� ������ ���� 8, � �������� �����.
	for(i = 0; i < SUCount; i++)
	{
		TSURXState * S = &SURxStates[i];
		if(S->ReceiveState)
		{
			bool PS = gp_Get(&SUPins[i]);
			
			if(!PS) S->TempBuffer &= ~(1 << S->BitIndex);
			S->BitIndex++;
			if(S->BitIndex == 9)
			{
				S->ReceiveState = false;
				// �������� ��������� ���� - 8 ��� ����� ���� 0 (�������� �������� ���)
				if((S->TempBuffer & 0xFF00) == 0xFF00)
				{
					// �� �����
					OnChar(i, S->TempBuffer & 0xFF);
				}
				else
				{
					// Break
				}
			}
		}
		else
		{
			// ������� ��������� ���, �������� ����
			if(!gp_Get(&SUPins[i]))
			{
				S->ReceiveState = true;
				S->BitIndex = 0;
				S->TempBuffer = 0xFFFF;
			}
		}
	}
	//gp_Low(&Debug);
}

// ���������� ���������� ��������� �����
void surx_SetHandler(TSuCharHandler Handler)
{
	SuHandler = Handler;
}

bool Init(void)
{
	int i;
    if(!drv_Require(&timer, "gpio")) return false;
    if(!drv_Require(&timer, "timer")) return false;
	
	for(i = 0; i < SUCount; i++)
	{
		SURxStates[i].ReceiveState = false;
		SURxStates[i].BitIndex   = 0;
		SURxStates[i].TempBuffer = 0xFFFF;
		
		gp_Input(&SUPins[i]);
		gp_PullUp(&SUPins[i]);
	}
	SuHandler = 0;
	
	gp_Output(&Debug);
	gp_Low(&Debug);
	
	NVIC_EnableIRQ(TIM15_IRQn);
	
	tim_Init(Timer, 200, Frequency);
	tim_InterruptEnable(Timer, true, TIM_DIER_UIE);
	tim_Start(Timer);
	return true;
}	


void Uninit(void)
{
	int i;
	
	for(i = 0; i < SUCount; i++)
		gp_NoPull(&SUPins[i]);
	
	gp_Input(&Debug);
	
	tim_Stop(Timer);
	tim_Uninit(Timer);
}

const TDriver surx = { "surx", Init, Uninit, 0 };

