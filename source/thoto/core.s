; ***********************************************************
; **                   Thoto OS Scheduler                  **
; ***********************************************************

; Диспетчер сохраняет и загружает контекст процесса, не более того.
; Весь остальной код написан на Си.

 PRESERVE8
 AREA    |.text|, CODE, READONLY

                 IMPORT SysTick_User
                 IMPORT ContextSwitch
                 
                 IMPORT FunctionCount
                 IMPORT FunctionTable
                 
                 IMPORT exitprocess
                 
                 IMPORT _GetCurrentProcess
                 IMPORT _SetCurrentProcess
                 IMPORT _GetProcess
                 
; Инициатор смены контекста
CONTEXT_SYSTICK_HANDLER   EQU 0x00
CONTEXT_SVC_HANDLER       EQU 0x01
CONTEXT_HARDFAULT_HANDLER EQU 0x02

SP_OFFSET               EQU 0x00

; Сохранение контекста
; Аргументы: нет
; Результаты: нет
ContextSave     PROC
                EXPORT ContextSave
                PUSH {LR}
                
                ; Получить номер текущего слота
                BL    _GetCurrentProcess
                ; Получить указатель на запись в таблице процессов
                BL    _GetProcess
                
                ; Сохранить её в R1
                MOV   R1, R0
                
                ; Прочитать указатель стека процесса
                MRS   R0, PSP
                
                ; Записать указатель стека в таблицу
                STR   R0, [R1, #SP_OFFSET]
                
                ; Передвинем указатель на список регистров
                ADDS  R1, #4
                
                ; Записать старшие регистры в таблицу
                ;STMIA R1, {R4-R11}
				STR   R4, [R1,  #0]
				STR   R5, [R1,  #4]
				STR   R6, [R1,  #8]
				STR   R7, [R1,  #12]
				MOV   R4, R8
				STR   R4, [R1,  #16]
				MOV   R4, R9
				STR   R4, [R1,  #20]
				MOV   R4, R10
				STR   R4, [R1, #24]
				MOV   R4, R11
				STR   R4, [R1, #28]
                
                POP  {PC} 
                ENDP

; Сохранение контекста
; Аргументы: нет
; Результаты: PSP, R4-R11 восстановлены
ContextLoad     PROC
                EXPORT ContextLoad
                PUSH {LR}
                
                ; Получить номер текущего слота
                BL    _GetCurrentProcess
                ; Получить указатель на запись в таблице процессов
                BL    _GetProcess
                
                ; Сохранить её в R1
                MOV   R1, R0
                
                ; Прочитать указатель стека в R0
                LDR   R0, [R1, #SP_OFFSET]
                
                ; Записать указатель стека процесса
                MSR   PSP, R0
                
                ; Передвинем указатель на список регистров
                ADDS  R1, #4
                
                ; Прочитать старщие регистры из таблицы
                ;LDMIA R1, {R4-R11}
				LDR   R4, [R1, #0]
				LDR   R5, [R1, #4]
				LDR   R6, [R1, #8]
				LDR   R7, [R1, #12]
				LDR   R3, [R1, #16]
				MOV   R8, R3
				LDR   R3, [R1, #20]
				MOV   R9, R3
				LDR   R3, [R1, #24]
				MOV   R10, R3
				LDR   R3, [R1, #28]
				MOV   R11, R3
				
                
                POP  {PC} 
                ENDP

; Обработчик прерывания системного таймера
SysTick_Handler PROC    
                EXPORT SysTick_Handler
                 
                ; Запретим прерывания
                CPSID i
                 
                ; Сохраним старшие регистры (мало ли придётся сохранять)
                PUSH {R4-R7,LR}
                 
                ; Обработать задержки и т.д.
                BL SysTick_User
                 
                ; Обработано... Восстановим регистры
                POP  {R4-R6}
                ; R7->R0, LR -> R7
                ;POP  {R0,R7}
                POP {R0}
				
                ; Сохраним контекст
                BL    ContextSave

                ; Переключим контекст, если надо
                MOVS  R0, #CONTEXT_SYSTICK_HANDLER
                BL    ContextSwitch
                
                ; Восстановим контекст
                BL    ContextLoad
                
                ; Разрешим прерывания
                CPSIE i
                
                ;BX    R7
				POP   {PC}
                ENDP

SVC_F_RESULT         EQU 0x00000001 ; Запись результата функции в R0 процесса
SVC_F_FORCE_SWITCH   EQU 0x00000002 ; Сменить контекст

; Обработчик прерывания системного таймера
SVC_Handler     PROC    
                EXPORT SVC_Handler 
                
                ; Запретим прерывания
                CPSID i
                
                ; Сохраним старшие регистры (мало ли придётся сохранять)
                PUSH {R4-R7,LR}
                
                ; Получение номера прерывания (теперь через PSP)
                MRS   R7, PSP
                LDR   R7, [R7, #0x18] ; PC
				SUBS  R7, #2
                LDRB  R7, [R7]  ; Младший байт инструкции (номер)

                ; Проверим номер на допустимость (а то улетим куда)
                LDR   R6, =FunctionCount
                LDR   R6, [R6]
            
                ; Если много - выходим.
                CMP   R7, R6
                
				BLT   FunctionOk
				
                POP   {R4-R7}
                B     _svch_exit
                
FunctionOk
                ; Сохраним номер для проверки дальнейшей
                PUSH {R7}
                
                ; Умножим номер на 8, получим смещение.
                LSLS  R7, #3
                
                ; И прибавим к началу таблицы
                LDR   R6, =FunctionTable
                ADD   R6, R7
                
                ; Загрузим адрес функции
                LDR   R7, [R6, #0x00] ; Handler
                
                ; Вызов функции
                BLX   R7
                
                ; Вытащим номер обратно в R1
                POP  {R1}
                
                ; Надо ли записывать в R0?
                LDR   R2, [R6, #0x04] ; Flags
				MOVS  R3, #SVC_F_RESULT
                TST   R2, R3
                
                BEQ   NoResult
                ; Скопипастим R0 обратно в стек
                MRS   R7,  PSP
                STR   R0,  [R7]
                
NoResult
                ; Вернём старшие регистры
                POP {R4-R6}
                
                ; Надо ли переключать процесс?
				MOVS  R3, #SVC_F_FORCE_SWITCH
                TST   R2, R3
                ; Если не надо - выходим
                BEQ   _svch_exit
                
                ; Сохраним контекст
                BL    ContextSave

                ; Переключим контекст, если надо
                MOVS  R0, #CONTEXT_SVC_HANDLER
                BL    ContextSwitch
                
                ; Восстановим контекст
                BL    ContextLoad
                
_svch_exit
                ; R7->R0, LR -> R7
                ;POP  {R0, R7}
                POP {R0}

                ; Разрешим прерывания
                CPSIE i
                
                ;BX    R7
				POP {PC}
                ENDP
                
; Процедура-заглушка для процесса
; Аргументы: 1
;  R0: код выхода
; Результат: нет
ProcessStub     PROC
                EXPORT ProcessStub
                
                ; Корректное завершение процесса с кодом возврата
                BL exitprocess
                
                B     .
                ENDP
 END
 