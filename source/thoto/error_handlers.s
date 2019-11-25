; ***********************************************************
; **                Thoto OS error handler                 **
; ***********************************************************

 PRESERVE8
 AREA    |.text|, CODE, READONLY

                 EXPORT HardFault_Handler

                 IMPORT HandleProcessError
                 IMPORT ContextSave
                 IMPORT ContextSwitch
                 IMPORT ContextLoad
            
CONTEXT_HARDFAULT_HANDLER EQU 0x02
HARDFAULT_SOURCE          EQU 0x00

; Обработка ошибок
HardFault_Handler PROC
                ; Сохранить код возврата
                PUSH {R4-R7,LR}
                
                ; Обработать ошибку
                MOVS  R0, #HARDFAULT_SOURCE
                MRS   R1, PSP
                LDR   R1, [R1, #0x18]; PC сбойного процесса
                ; HandleProcessError(HARDFAULT_SOURCE, PC);
                BL    HandleProcessError
                
                POP  {R4-R7}
                
                ; Инициировать переключение контекста
                ; Сохраним контекст
                BL    ContextSave

                ; Переключим контекст, если надо
                MOVS  R0, #CONTEXT_HARDFAULT_HANDLER
                BL    ContextSwitch
                
                ; Восстановим контекст
                BL    ContextLoad
                
                ; Выход
                POP  {PC}
                ENDP
                
    END
