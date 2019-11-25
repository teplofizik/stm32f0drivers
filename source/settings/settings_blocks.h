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
        SETT_NETWORK     = 0xAA01,    // ��������� ����
        SETT_RDM_UNIQUE  = 0xAA02,    // ��������� RDM, ����������� �� ���� ����������
        SETT_DMX         = 0xAA03,    // ��������� DMX
        SETT_PATCH       = 0xAA04,    // ��������� ����-����� �������
        
        SETT_APP1        = 0xEE01,    // ������������� ��� ���������� ����
        SETT_APP2        = 0xEE02,    // ������������� ��� ���������� ����
        SETT_APP3        = 0xEE03,    // ������������� ��� ���������� ����
    } TBlockID;

    typedef struct
    {
        TBlockID  ID;       // ��� �����
        uint16_t  Version;  // ������ �����
        uint16_t  Length;   // �����
        uint16_t  Checksum; // ����������� �����
        
        uint8_t   Data[];   // ������
    } TBlock;
    
    // ���� � �������� �����������
    typedef struct
    {
        uint8_t  Interface; // ����� ����������, ���� �� ���������, ��� ������������� 0
        uint16_t Flags;     // DHCP/...
        
        uint8_t  IP[4];     // IP-�����
        uint8_t  NM[4];     // ����� ����
        uint8_t  GW[4];     // ����
        
        uint8_t  MAC[6];    // ���-����� �������� ����������
    } TNetworkBlock;
    
    // ���� � ������� RDM
    typedef struct
    {
        // ����� RDM
        uint8_t Address[6];
        
        // ���� ������������
        uint8_t Year;
        uint8_t Month;
        uint8_t Day;
        
        // ����� ����� (������ ��� �������������)
        uint16_t Block;
    } TRDMUniqueBlock;
    
    // ��������� DMX
    typedef struct
    {
        uint8_t  Personality;  // ������� ����������
        
        uint16_t StartAddress; // ��������� ����� DMX
        uint16_t SA_Phy;       // ������������ ��� ������� ���������� �������������� ������
        
        uint16_t Flags;        // �����: ���������
        
        uint8_t  Source;       // �������� ����������: ����������� ������ (DMX/AN/ACN), ������ �����, ����
        
        uint8_t  Manual[12];   // ������ �����
        uint8_t  Auto[12];     // ����������
        uint8_t  Mapping[12];  // ������� ������� ������� �� �������� (�� ��������� 0->0, 1->1, 2->2)
        
        char     Label[32];    // ����� ������
    } TDMXBlock;
    
    // ��������� �����
    typedef struct
    {
        uint16_t Patch[12];
    } TPatchBlock;
    
    // ��������� 
    bool sb_Check(TBlockID Block, void * Data, int Length);
    
    // ��������� ��������� � �����
    void sb_Save(TBlockID Block, void * Data, int Length);
    
    // ��������� ������ �� �����
    bool sb_Load(TBlockID Block, void * Data, int Length);
    
    
#endif
