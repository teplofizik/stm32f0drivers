// ******************************************************
// settings_ll.h
//
// teplofizik
// ******************************************************
 
#include <stdint.h>
#include <stdbool.h>
 
#ifndef SETTINGS_LL_H_
#define SETTINGS_LL_H_

    // Количество страниц, доступное для записи
    int sll_PageCount(void);

    // Стереть страницу
    void sll_Erase(int Page);

    // Прочитать блок из памяти
    void sll_Read(int Page, int Offset, uint8_t * Data, int Length);

    // Записать блок в память
    void sll_Write(int Page, int Offset, const uint8_t * Data, int Length);
            

#endif
