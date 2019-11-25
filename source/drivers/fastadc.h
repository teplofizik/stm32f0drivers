// ***********************************************************
//	fastadc.c
// 	������� ��� �� ���� ����� v2.1.0
//
//  ����������: gpio, dma, clock
//  teplofizik, 2018
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"
#include "gpio.h"

#ifndef _FASTADC_H
#define _FASTADC_H

    typedef enum
    {
        FADC_PA0,   //  0: PA0
        FADC_PA1,   //  1: PA1
        FADC_PA2,   //  2: PA2
        FADC_PA3,   //  3: PA3
        FADC_PA4,   //  4: PA4
        FADC_PA5,   //  5: PA5
        FADC_PA6,   //  6: PA6
        FADC_PA7,   //  7: PA7
        
        FADC_PB0,   //  8: PB0
        FADC_PB1,   //  9: PB1
        
        FADC_PC0,   // 10: PC0
        FADC_PC1,   // 11: PC1
        FADC_PC2,   // 12: PC2
        FADC_PC3,   // 13: PC3
        FADC_PC4,   // 14: PC4
        FADC_PC5,   // 15: PC5
        
        FADC_TEMP,  // �����������
        FADC_VREF,  // ������� ����������
        FADC_VBAT2  // �������� ���������� � ��������� (Vbat/2)
    } TFADCChannel;

    // �������� ����� ���������
    void fadc_Enable(TFADCChannel Channel, int Count);
    
    // ��������� ����� ���������
    void fadc_Disable(void);
    
    // �������� ��������� ���������
	uint16_t * fadc_Get(void);
    
	void fadc_SetHandler(TEventHandler Handler);
	
    extern const TDriver fastadc;
    
#endif
