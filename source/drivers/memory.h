// ***********************************************************
//	memory.h
// 	Работа с памятью v2.0.0
// 
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

#ifndef _MEMORY_H
#define _MEMORY_H

    #define MEMORY_RECORD_COUNT 10

	// Выделить блок памяти
	void * mem_Alloc(uint32_t Size);
	
    // Освободить выделенный буфер
    void mem_Free(void * Memory);
	
    // Количество памяти, занятых в стеке
    uint32_t mem_StackUsage(void);
    
    // Количество памяти, выделенное под стек
    uint32_t mem_StackSize(void);
    
    // Количество памяти, занятое в куче
    uint32_t mem_HeapUsage(void);
    
    // Количество памяти, выделенное под кучу
    uint32_t mem_HeapSize(void);
    
    extern const TDriver memory;
    
#endif
