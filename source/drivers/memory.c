// ***********************************************************
//	memory.h
// 	Работа с памятью v2.0.0
// 
//  teplofizik, 2016
// ***********************************************************

#include "memory.h"
#include <stdlib.h>
#include <string.h>
#include <stm32f0xx.h>

typedef struct
{
    void * Pointer;
    uint32_t Size;
} THeapRecord;

// Стек
extern uint32_t Stack_Mem;
extern uint32_t __initial_sp;

// Куча
extern uint32_t __heap_base;
extern uint32_t __heap_limit;

static bool MemEnabled = false;
static int  MemAllocated = 0;

// Записи о выделенных блоках
static THeapRecord Blocks[MEMORY_RECORD_COUNT];

// Получить запись о блоке
static THeapRecord * mem_GetBlockRecord(void * Pointer)
{
    int i;
    for(i = 0; i < MEMORY_RECORD_COUNT; i++)
    {
        if(Blocks[i].Pointer == Pointer)
            return &Blocks[i];
    }
    return 0;
}

// Добавить блок в таблицу
static void mem_AddBlock(void * Pointer, uint32_t Size)
{
    int i;
    for(i = 0; i < MEMORY_RECORD_COUNT; i++)
    {
        if(!Blocks[i].Pointer)
        {
            THeapRecord * R = &Blocks[i];
            R->Pointer = Pointer;
            R->Size = Size;
            return;
        }
    }
    drv_DebugLog(&memory, "No enough heap block info slots in driver '%s'");
}

// Выделить блок памяти
void * mem_Alloc(uint32_t Size)
{
    if(MemEnabled)
    {
        void * mem = malloc(Size);
        
        // add to table
        if(mem) 
        {
            MemAllocated += Size;
            mem_AddBlock(mem, Size);
        }
        
        return mem;
    }
    return 0;
}
	
// Освободить выделенный буфер
void mem_Free(void * Memory)
{
    if(MemEnabled)
    {
		if(Memory)
		{
			THeapRecord * R = mem_GetBlockRecord(Memory);
			// удалим из таблицы
			if(R)
			{
				MemAllocated -= R->Size;
				
				memset(R, 0, sizeof(*R));
			}
			
			free(Memory);
		}
    }
}
	
// Количество памяти, занятых в стеке
uint32_t mem_StackUsage(void)
{
    return (uint32_t)&__initial_sp - __get_MSP();
}

// Количество памяти, выделенное под стек
uint32_t mem_StackSize(void)
{
    return (uint32_t)&__initial_sp - (uint32_t)&Stack_Mem;
}
    
// Количество памяти, занятое в куче
uint32_t mem_HeapUsage(void)
{
    return (MemEnabled) ? MemAllocated : 0;
}

// Количество памяти, выделенное под кучу
uint32_t mem_HeapSize(void)
{
    return (uint32_t)&__heap_limit - (uint32_t)&__heap_base;
}

static bool Init(void)
{
    MemEnabled = true;
    memset(Blocks, 0, sizeof(Blocks));
    
    return true;
}

static void Uninit(void)
{
    MemEnabled = false;
}

const TDriver memory = { "memory", &Init, &Uninit, 0 };
