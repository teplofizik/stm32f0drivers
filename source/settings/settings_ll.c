// ******************************************************
// settings_ll.c
//
// teplofizik
// ******************************************************
 
#include "settings_ll.h"

// TODO: ƒобавить драйверы (sd/eeprom/flash, с приоритетами)
//       ѕровер€ем наличие пам€ти дл€ хранени€ данных - и работаем с ней (грузимс€, гул€ем по страницам).
//       Ќа остальные доступные устройства кладетс€ одна копи€. все левые страницы очищаютс€.

//  оличество страниц, доступное дл€ записи
int sll_PageCount(void)
{
    return 2;
}
    
// —тереть страницу
void sll_Erase(int Page)
{
    
}

// ѕрочитать блок из пам€ти
void sll_Read(int Page, int Offset, uint8_t * Data, int Length)
{
    
}

// «аписать блок в пам€ть
void sll_Write(int Page, int Offset, const uint8_t * Data, int Length)
{
    
}

// ƒелаем копию на все остальное
void sll_Duplicate(int Page, int Length)
{

}    
