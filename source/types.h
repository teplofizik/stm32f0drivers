//
// types.h
//
// Basic types
//

#include <stdint.h>
#include <stdbool.h>

#ifndef _TYPES_H
#define _TYPES_H

	typedef void (* TEventHandler)(void);
	
    // Блокировка
    typedef bool (* TLockHandler)(void);
    
    // Установка/разрыв соединения
	typedef void (* TLinkHandler)(bool Connected);
	
	// Serial types
    typedef void (* TCharHandler)(char Ch);
    typedef void (* TSerialHandler)(const char * Ch);

#endif
