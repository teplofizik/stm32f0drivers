// ******************************************************
// settings_blocks.c
//
// teplofizik
// ******************************************************
 
#include "settings_blocks.h"
#include "settings_ll.h"
#include <string.h>

#define BLOCK_MAGIC 0x1122

// Заголовок страницы
typedef struct
{
    uint16_t Magic;
    uint32_t Version;
    
    uint8_t  BlockCount;
} THeader;

// Ссылки на структуры во флеше
typedef struct
{
    TBlockID BlockID;
    
    TBlock * Block;
} TBlockInfo;

int        ActivePage;
int        ActiveVersion = 0;
TBlockInfo BlockInfo[10];

// Загрузить настройки со страницы
static bool sd_LoadFromPage(int Page)
{
    THeader H;
    sll_Read(Page, 0, (uint8_t*)&H, sizeof(H));
    
    if(H.Magic == BLOCK_MAGIC)
    {
        int i;
        int Offset = 0x10;
        ActiveVersion = H.Version;
        
        for(i = 0; i < H.BlockCount; i++)
        {
            TBlock B;
            sll_Read(Page, Offset, (uint8_t*)&B, sizeof(B));
            
            // Данные будут вычитаны при загрузке
        }
        
        return true;
    }
    else
        return false;
}

void sb_Init(void)
{
    ActivePage = 0;
    ActiveVersion = 0;
    memset(BlockInfo, 0, sizeof(BlockInfo));
    
    sd_LoadFromPage(0);
}

// Получить блок по его идентификатору
static TBlockInfo * sb_Get(TBlockID Block)
{
    int i;
    for(i = 0; i < sizeof(BlockInfo)/sizeof(BlockInfo[0]); i++)
    {
        if(BlockInfo[i].BlockID == Block)
            return &BlockInfo[i];
    }
    
    return 0;
}

// Проверить 
bool sb_Check(TBlockID Block, void * Data, int Length)
{
    TBlockInfo * B = sb_Get(Block);
    if(B)
        return (Length != B->Block->Length) || !memcmp(&B->Block->Data[0], Data, Length);
    else
        return false;
}

// Вычисление контрольной суммы
static uint16_t sb_Checksum(const uint8_t * Data, int Length)
{
    int i;
    uint16_t Res = 0xAAAA;
    for(i = 0; i < Length; i++)
        Res += ((Data[i] << 8) | ~Data[i]) ^ 0x5555;

    return Res;
}

// Получить номер следующей страницы
static int sb_getNextPage(void)
{
    if(ActivePage + 1 >= sll_PageCount())
        return 0;
    else
        return ActivePage + 1;
}

// Записать заголовок страницы
static void sb_WriteHeader(int Page, int Version, int Count)
{
    THeader H;
    H.Magic = BLOCK_MAGIC;
    H.Version = Version;
    H.BlockCount = Count;
    
    sll_Write(Page, 0, (const uint8_t *)&H, sizeof(H));
}

// Запись блока настроек
static void sb_WriteBlock(int Page, int * Offset, TBlockID Block, int Version, void * Data, int Length)
{
    TBlock B;
    B.Version = (Version > 100000) ? 1 : Version;
    B.Length = Length;
    B.Checksum = sb_Checksum(Data, Length);
    B.ID = Block;
    
    // Запись информации о блоке
    sll_Write(Page, *Offset, (const uint8_t *)&B, sizeof(B));
    // Запись данных блока
    sll_Write(Page, *Offset + sizeof(Block), Data, Length);
    
    *Offset = (*Offset + Length + sizeof(Block) + 15) & 0xFFFFFFF0;
}

// Сохранить изменения в блоке
void sb_Save(TBlockID Block, void * Data, int Length)
{
    // Выбор свободной страницы в памяти
    // Копипаста
    int Page = sb_getNextPage();
    int i;
    int Offset = 0x10;
    int Count = 0;
    bool Writed = false;
    
    // Стираем страницу памяти
    sll_Erase(Page);
    
    for(i = 0; i < sizeof(BlockInfo)/sizeof(BlockInfo[0]); i++)
    {
        if(BlockInfo[i].BlockID == Block)
        {
            // Пишем новые данные
            sb_WriteBlock(Page, &Offset, Block, BlockInfo[i].Block->Version + 1, Data, Length);
            Writed = true;
            Count++;
        }
        else if (BlockInfo[i].BlockID)
        {
            // Пишем старые данные
            sb_WriteBlock(Page, &Offset, BlockInfo[i].BlockID, BlockInfo[i].Block->Version, BlockInfo[i].Block->Data, BlockInfo[i].Block->Length);
            Count++;
        }
        else
            break;
    }
    
    if(!Writed)
    {
        // Не было такого
        sb_WriteBlock(Page, &Offset, Block, 1, Data, Length);
        Count++;
    }
    
    // Переключимся
    ActivePage = Page;
    ActiveVersion = (ActiveVersion > 100000) ? 1 : ActiveVersion + 1;
    
    // Пишем информацию о странице
    sb_WriteHeader(Page, ActiveVersion, Count); // TODO: Version
}

// Загрузить данные из блока
bool sb_Load(TBlockID Block, void * Data, int Length)
{
    int i;
    for(i = 0; i < sizeof(BlockInfo)/sizeof(BlockInfo[0]); i++)
    {
        if(BlockInfo[i].BlockID == Block)
        {
            memcpy(Data, BlockInfo[i].Block->Data, Length);
            // Check CS?
            return true;
            
        }
    }
    
    return false;
}
