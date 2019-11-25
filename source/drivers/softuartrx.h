// ***********************************************************
//	softuartrx.h
//  Приёмник уарта, многоканальный
//  
//  Требования: gpio, timer
//  teplofizik, 2017
// ***********************************************************

#include "driver.h"

#ifndef _SOFTUARTRX_H
#define _SOFTUARTRX_H

    typedef void (* TSuCharHandler)(int Index, char Ch);

	// Установить обработчик принятого байта
	void surx_SetHandler(TSuCharHandler Handler);

	extern const TDriver surx;

#endif
