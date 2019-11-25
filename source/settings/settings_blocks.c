// ******************************************************
// settings_blocks.c
//
// teplofizik
// ******************************************************
 
#include "settings_blocks.h"
#include "settings_ll.h"
#include <string.h>

#define BLOCK_MAGIC 0x1122

// ��������� ��������
typedef struct
{
    uint16_t Magic;
    uint32_t Version;
    
    uint8_t  BlockCount;
} THeader;

// ������ �� ��������� �� �����
typedef struct
{
    TBlockID BlockID;
    
    TBlock * Block;
} TBlockInfo;

int        ActivePage;
int        ActiveVersion = 0;
TBlockInfo BlockInfo[10];

// ��������� ��������� �� ��������
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
            
            // ������ ����� �������� ��� ��������
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

// �������� ���� �� ��� ��������������
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

// ��������� 
bool sb_Check(TBlockID Block, void * Data, int Length)
{
    TBlockInfo * B = sb_Get(Block);
    if(B)
        return (Length != B->Block->Length) || !memcmp(&B->Block->Data[0], Data, Length);
    else
        return false;
}

// ���������� ����������� �����
static uint16_t sb_Checksum(const uint8_t * Data, int Length)
{
    int i;
    uint16_t Res = 0xAAAA;
    for(i = 0; i < Length; i++)
        Res += ((Data[i] << 8) | ~Data[i]) ^ 0x5555;

    return Res;
}

// �������� ����� ��������� ��������
static int sb_getNextPage(void)
{
    if(ActivePage + 1 >= sll_PageCount())
        return 0;
    else
        return ActivePage + 1;
}

// �������� ��������� ��������
static void sb_WriteHeader(int Page, int Version, int Count)
{
    THeader H;
    H.Magic = BLOCK_MAGIC;
    H.Version = Version;
    H.BlockCount = Count;
    
    sll_Write(Page, 0, (const uint8_t *)&H, sizeof(H));
}

// ������ ����� ��������
static void sb_WriteBlock(int Page, int * Offset, TBlockID Block, int Version, void * Data, int Length)
{
    TBlock B;
    B.Version = (Version > 100000) ? 1 : Version;
    B.Length = Length;
    B.Checksum = sb_Checksum(Data, Length);
    B.ID = Block;
    
    // ������ ���������� � �����
    sll_Write(Page, *Offset, (const uint8_t *)&B, sizeof(B));
    // ������ ������ �����
    sll_Write(Page, *Offset + sizeof(Block), Data, Length);
    
    *Offset = (*Offset + Length + sizeof(Block) + 15) & 0xFFFFFFF0;
}

// ��������� ��������� � �����
void sb_Save(TBlockID Block, void * Data, int Length)
{
    // ����� ��������� �������� � ������
    // ���������
    int Page = sb_getNextPage();
    int i;
    int Offset = 0x10;
    int Count = 0;
    bool Writed = false;
    
    // ������� �������� ������
    sll_Erase(Page);
    
    for(i = 0; i < sizeof(BlockInfo)/sizeof(BlockInfo[0]); i++)
    {
        if(BlockInfo[i].BlockID == Block)
        {
            // ����� ����� ������
            sb_WriteBlock(Page, &Offset, Block, BlockInfo[i].Block->Version + 1, Data, Length);
            Writed = true;
            Count++;
        }
        else if (BlockInfo[i].BlockID)
        {
            // ����� ������ ������
            sb_WriteBlock(Page, &Offset, BlockInfo[i].BlockID, BlockInfo[i].Block->Version, BlockInfo[i].Block->Data, BlockInfo[i].Block->Length);
            Count++;
        }
        else
            break;
    }
    
    if(!Writed)
    {
        // �� ���� ������
        sb_WriteBlock(Page, &Offset, Block, 1, Data, Length);
        Count++;
    }
    
    // ������������
    ActivePage = Page;
    ActiveVersion = (ActiveVersion > 100000) ? 1 : ActiveVersion + 1;
    
    // ����� ���������� � ��������
    sb_WriteHeader(Page, ActiveVersion, Count); // TODO: Version
}

// ��������� ������ �� �����
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
