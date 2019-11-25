// ***********************************************************
// **                 Thoto OS error codes                  **
// ***********************************************************

#ifndef _THOTO_ERROR_CODES_H
#define _THOTO_ERROR_CODES_H

    // Коды ошибки
    typedef enum
    {
        // Операция выполнена успешно
        ERROR_SUCCESS                                   = 0x0000,

        // Процесс не найден
        ERROR_PROCESS_NOT_FOUND                         = 0x0201,

        // Слишком много процессов в таблице
        ERROR_CORE_TOO_MANY_PROCESSES                   = 0x1001
    } ERROR_CODES;

#endif
