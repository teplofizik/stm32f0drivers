// ***********************************************************
//	app.h
// 	������� ��� ��� ������ � ������������
//
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _APP_H
#define _APP_H

    #define MAX_APP_COUNT 5

    typedef struct
    {
        // �������� ����������
        const char  * Name;
        
        // ������ ����������� ���������
        const char ** Required;
        
        // ������ ���������� - ���� false, �� �� ����������� (�� ��������� � ������)
        bool (* Init)(void);
        // �������� ����������
        void (* Uninit)(void);
        // �������� ���� - ���� false, �� ��������� ��-�� ������.
        bool (* Main)(void);
    } TApp;

	// ������������� ���������� ����������
    void app_Init(void);
    
	// �������� ���� ��� ��������
    void app_Main(void);

    // ��������� ����������
    bool app_Start(const TApp * App);

    // ���������� ����������
    void app_Stop(const TApp * App);
    
#endif
