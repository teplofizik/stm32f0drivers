// ******************************************************
// settings_blocks.h
//
// teplofizik
// ******************************************************
 
#include <stdint.h>
#include <stdbool.h>
 
#ifndef SETTINGS_BLOCKS_H_
#define SETTINGS_BLOCKS_H_

	typedef enum
    {
        SETT_NETWORK     = 0xAA01,    // Настройки сети
        SETT_RDM_UNIQUE  = 0xAA02,    // Настройки RDM, независимые от типа устройства
        SETT_DMX         = 0xAA03,    // Настройки DMX
        SETT_PATCH       = 0xAA04,    // Настройки софт-патча каналов
        
        SETT_APP1        = 0xEE01,    // Специфический для приложения блок
        SETT_APP2        = 0xEE02,    // Специфический для приложения блок
        SETT_APP3        = 0xEE03,    // Специфический для приложения блок
    } TBlockID;

    typedef struct
    {
        TBlockID  ID;       // Тип блока
        uint16_t  Version;  // Версия блока
        uint16_t  Length;   // Длина
        uint16_t  Checksum; // Контрольная сумма
        
        uint8_t   Data[];   // Данные
    } TBlock;
    
    // Блок с сетевыми настройками
    typedef struct
    {
        uint8_t  Interface; // Номер интерфейса, если их несколько, для единственного 0
        uint16_t Flags;     // DHCP/...
        
        uint8_t  IP[4];     // IP-адрес
        uint8_t  NM[4];     // Маска сети
        uint8_t  GW[4];     // Шлюз
        
        uint8_t  MAC[6];    // МАС-адрес сетевого интерфейса
    } TNetworkBlock;
    
    // Блок с адресом RDM
    typedef struct
    {
        // Адрес RDM
        uint8_t Address[6];
        
        // Дата производства
        uint8_t Year;
        uint8_t Month;
        uint8_t Day;
        
        // Номер блока (просто для идентификации)
        uint16_t Block;
    } TRDMUniqueBlock;
    
    // Настройки DMX
    typedef struct
    {
        uint8_t  Personality;  // Профиль управления
        
        uint16_t StartAddress; // Стартовый адрес DMX
        uint16_t SA_Phy;       // Используется при наличии физических переключаторах адреса
        
        uint16_t Flags;        // Опции: автостарт
        
        uint8_t  Source;       // Источник управления: управляющий сигнал (DMX/AN/ACN), ручной режим, тест
        
        uint8_t  Manual[12];   // Ручной режим
        uint8_t  Auto[12];     // Автозапуск
        uint8_t  Mapping[12];  // Маппинг входных каналов на выходные (по умолчанию 0->0, 1->1, 2->2)
        
        char     Label[32];    // Метка адреса
    } TDMXBlock;
    
    // Настройки патча
    typedef struct
    {
        uint16_t Patch[12];
    } TPatchBlock;
    
    // Проверить 
    bool sb_Check(TBlockID Block, void * Data, int Length);
    
    // Сохранить изменения в блоке
    void sb_Save(TBlockID Block, void * Data, int Length);
    
    // Загрузить данные из блока
    bool sb_Load(TBlockID Block, void * Data, int Length);
    
    
#endif
