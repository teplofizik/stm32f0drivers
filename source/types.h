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
	
    // ����������
    typedef bool (* TLockHandler)(void);
    
    // ���������/������ ����������
	typedef void (* TLinkHandler)(bool Connected);
	
	// Serial types
    typedef void (* TCharHandler)(char Ch);
    typedef void (* TSerialHandler)(const char * Ch);

#endif
