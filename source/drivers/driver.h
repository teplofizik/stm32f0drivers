// ***********************************************************
//	driver.h
// 	������� ��� ��� ������ � ����������
//
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

// ������ ��������������� ������
#include "hw_avail.h"
// ����������� ����
#include "../types.h"
	
#ifndef _DRIVER_H
#define _DRIVER_H

    #define MAX_DRIVER_COUNT 20

    typedef struct
    {
        const char * Name;
        
        // ������������� - ���� false, �� �� ����������� (�� ��������� � ������)
        bool (* Init)(void);
        // �������� ��������
        void (* Uninit)(void);
        // �������� ���� - ���� false, �� ��������� ��-�� ������.
        bool (* Main)(void);
    } TDriver;

	// ������������� ���������� ���������
    void drv_Init(void);
    
	// �������� ���� ��� ���������
    void drv_Main(void);

    // ��������� �������
    void drv_Load(const TDriver * Drv);

    // ������� ��������?
    bool drv_IsLoaded(const char * name);
        
    // ���������� ������� ��������
    // ���� �� �������� - false
    bool drv_Require(const TDriver * Drv, const char * name);
    
    // ������ � ���
    void drv_DebugLog(const TDriver * Drv, const char * Text);
    
#endif
