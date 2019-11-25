// ***********************************************************
//	uuid.c
// 	Unique ID
// 
//  teplofizik, 2017
// ***********************************************************

#include "uuid.h"
#include <stdio.h>
#include <string.h>

#define STM32_UUID ((uint32_t *)0x1FFFF7AC)
    
// Get unique ID as string (25 bytes)
void uid_Get(char * Dest)
{
    uint32_t idPart1 = STM32_UUID[0];
    uint32_t idPart2 = STM32_UUID[1];
    uint32_t idPart3 = STM32_UUID[2];
    
    sprintf(Dest, "%08X-%08X-%08X", idPart1, idPart2, idPart3);
}

// Get unique ID as bytes (12 bytes)
void uid_GetRaw(uint8_t * Dest)
{
	memcpy(Dest, STM32_UUID, 12);
}
