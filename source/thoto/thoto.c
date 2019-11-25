// ***********************************************************
// **                    Thoto OS API                       **
// ***********************************************************

#include "thoto.h"
#include "internal.h"
#include "process.h"
#include "event.h"

bool thoto_Init(void)
{
    // Иницилизация таблиц процессов
    InitProcessTable();
    
    // Иницилизация таблицы событий
    InitEventTable();
    
    // Разделение указателей стека
    EnableProcessSP();
    
    // Процесс основного потка подхватывается автоматически при первом же прерывании по сустику.
	return true;
}

void thoto_Uninit(void)
{
	
}

const TDriver thoto = {"thoto", &thoto_Init, &thoto_Uninit, 0 };
