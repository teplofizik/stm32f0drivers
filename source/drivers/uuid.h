// ***********************************************************
//	uuid.h
// 	Unique ID
// 
//  teplofizik, 2017
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _UUID_H
#define _UUID_H

	// Get unique ID as string (25 bytes)
	void uid_Get(char * Dest);

	// Get unique ID as bytes (12 bytes)
	void uid_GetRaw(uint8_t * Dest);

#endif
