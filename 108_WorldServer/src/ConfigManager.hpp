#ifndef __CONIFG_MANAGER_HPP__
#define __CONIFG_MANAGER_HPP__

#include "TemplateConfigManager.hpp"

typedef struct
{
    char m_szHostIP[24];
    unsigned short m_ushPort;
    unsigned short m_ushZoneID;
    char m_szZoneName[64];
} TZoneConf;

typedef struct
{
    unsigned short m_ushLen;

    // 保存网通和电信2个IP信息
    TZoneConf m_astZoneConf[MAX_ZONE_PER_WORLD*2];
} TZoneConfList;

//角色出生表的配置
extern const char GAME_ROLE_BIRTH_CONFIG_FILE[];
typedef CTemplateConfigManager<SRoleBirthConfig, MAX_ROLE_BIRTH_NUM, GAME_ROLE_BIRTH_CONFIG_FILE> CSRoleBirthConfigManager;

//战斗单位配置管理器
extern const char GAME_FIGHT_UNIT_CONFIG_FILE[];
typedef CTemplateConfigManager<SFightUnitConfig, MAX_FIGHT_UNIT_NUM, GAME_FIGHT_UNIT_CONFIG_FILE> CSFightUnitConfigManager;

class CConfigManager
{
public:
    int Initialize(bool bResume);

    //角色出生表配置
    CSRoleBirthConfigManager& GetBirthConfigManager() { return m_stBirthConfigManager; };

    //玩家战斗单位表配置
    CSFightUnitConfigManager& GetFightUnitConfigManager() { return m_stFightUnitConfigManager; };

    int LoadZoneConf(const char* pszConfFile);
    const TZoneConfList& GetZoneConfList()
    {
        return m_stZoneConfList;
    }

private:
    TZoneConfList m_stZoneConfList;

    //角色出生表配置
    CSRoleBirthConfigManager m_stBirthConfigManager;

    //玩家战斗单位表配置
    CSFightUnitConfigManager m_stFightUnitConfigManager;
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
