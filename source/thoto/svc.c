// ***********************************************************
// **                Thoto OS Supervisor                    **
// ***********************************************************

#include "svc.h"

// Таблица процессов
SVCFUNC FunctionTable[] =
{
    { &svc_delay, SVC_F_FORCE_SWITCH },
    { &svc_createprocess, SVC_F_FORCE_SWITCH | SVC_F_RESULT },
    { &svc_exitprocess, SVC_F_FORCE_SWITCH },
    { &svc_currentprocess, SVC_F_RESULT },
    { &svc_wait, SVC_F_FORCE_SWITCH | SVC_F_RESULT },
    { &svc_event, 0 },
    { &svc_asyncevent, 0 },
    { &svc_asyncreturn, SVC_F_FORCE_SWITCH },
    { &svc_pcalloc, SVC_F_RESULT },
    { &svc_pfree, 0 },
};

const uint32_t FunctionCount = SVC_LASTITEM;

