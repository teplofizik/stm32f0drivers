// ******************************************************
// settings.h
//
// teplofizik
// ******************************************************
 
#include <stdint.h>
#include <stdbool.h>
#include "../drivers/driver.h"
#include "settings_blocks.h"
 
#ifndef SETTINGS_H_
#define SETTINGS_H_

    typedef struct
    { 
        bool RDMAvailable;
        bool DMXAvailable;
        
        // RDM Block
        TRDMUniqueBlock RDM;
        
        // DMX Block
        TDMXBlock       DMX;
    } TSettings;

    // —брос настроек
    void sett_Reset(void);
    
    extern const TDriver settings;

#endif
