
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "ModuleShortcut.hpp"

#include "CombatTrapObj.hpp"

using namespace ServerLib;

IMPLEMENT_DYN(CCombatTrapObj)

CCombatTrapObj::CCombatTrapObj()
{
    Initialize();
}

CCombatTrapObj::~CCombatTrapObj()
{

}

int CCombatTrapObj::Initialize()
{
    //机关的配置ID
    m_iConfigID = 0;

    //机关的配置
    m_pstConfig = NULL;

    //机关的当前血量
    m_iTrapHp = 0;

    //当前所属的阵营
    m_iCamp = FIGHT_CAMP_INVALID;

    //机关当前的位置
    m_stPos.iPosX = -1; //X -1表示是非法的位置

    //机关的面向
    m_iDirection = COMBAT_DIRECTION_INVALID;

    return T_SERVER_SUCESS;
}

//初始化机关
int CCombatTrapObj::InitTrap(int iConfigID, int iCamp, const TUNITPOSITION& stPos, int iDirection)
{
    m_iConfigID = iConfigID;

    //设置配置
    m_pstConfig = TrapCfgMgr().GetConfig(iConfigID);
    if(!m_pstConfig)
    {
        LOGERROR("Failed to get combat trap config, invalid trap id %d\n", m_iConfigID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    m_iCamp = iCamp;

    //如果血量 <0 则不能被攻击
    m_iTrapHp = m_pstConfig->iTrapHP;

    //设置位置
    m_stPos.iPosX = stPos.iPosX;
    m_stPos.iPosY = stPos.iPosY;

    m_iDirection = iDirection;

    return T_SERVER_SUCESS;
}

//获取机关当前的位置
const TUNITPOSITION& CCombatTrapObj::GetPosition()
{
    return m_stPos;
}

//修改机关的血量
void CCombatTrapObj::AddTrapHP(int iAddNum)
{
    m_iTrapHp += iAddNum;
    if(m_iTrapHp < 0)
    {
        m_iTrapHp = 0;
    }

    return;
}

int CCombatTrapObj::GetTrapHP()
{
    return m_iTrapHp;
}

//判断机关是否已损坏
bool CCombatTrapObj::IsTrapDead()
{
    return (m_iTrapHp == 0);
}

//判断机关是否可被攻击
bool CCombatTrapObj::IsTrapCanAttacked()
{
    return (m_iTrapHp >= 0);
}

//获取该机关的配置
const STrapConfig* CCombatTrapObj::GetConfig()
{
    return m_pstConfig;
}

//获取机关的配置ID
int CCombatTrapObj::GetTrapConfigID()
{
    return m_iConfigID;
}

//获取机关的阵营
int CCombatTrapObj::GetTrapCamp()
{
    return m_iCamp;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
