// ***********************************************************
//	bsp.h
// 	Board support package
//
//  teplofizik, 2016
// ***********************************************************

#include "bsp.h"

const TDriver * BSP[] = {
	&led,
	&button,
	
    // ���������� ��
    &upgrade,
    0
};
