// ***********************************************************
//	memory.h
// 	������ � ������� v2.0.0
// 
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _MEMORY_H
#define _MEMORY_H

    #define MEMORY_RECORD_COUNT 10

	// �������� ���� ������
	void * mem_Alloc(uint32_t Size);
	
    // ���������� ���������� �����
    void mem_Free(void * Memory);
	
    // ���������� ������, ������� � �����
    uint32_t mem_StackUsage(void);
    
    // ���������� ������, ���������� ��� ����
    uint32_t mem_StackSize(void);
    
    // ���������� ������, ������� � ����
    uint32_t mem_HeapUsage(void);
    
    // ���������� ������, ���������� ��� ����
    uint32_t mem_HeapSize(void);
    
    extern const TDriver memory;
    
#endif
