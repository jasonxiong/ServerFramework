
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "ZoneObjectHelper.hpp"
#include "FightUnitObj.hpp"
#include "ModuleShortcut.hpp"
#include "GameRole.hpp"
#include "ZoneMsgHelper.hpp"
#include "FightUnitManager.hpp"
#include "ModuleHelper.hpp"
#include "UnitUtility.hpp"

GameProtocolMsg CFightUnitManager::ms_stZoneMsg;

CFightUnitManager::CFightUnitManager()
{
    Initialize();
}

CFightUnitManager::~CFightUnitManager()
{

}

//初始化 FightUnitManager
int CFightUnitManager::Initialize()
{
    m_uiUin = 0;

    //所有战斗单位的信息
    m_iFightUnitNum = 0;
    for(int i=0; i<MAX_ROLE_FIGHTUNIT_NUM; ++i)
    {
        m_aiUnitObjID[i] = -1;
    }

    //阵型信息
    memset(m_aiFightForm, 0, sizeof(m_aiFightForm));

    return T_SERVER_SUCESS;
}

void CFightUnitManager::SetOwner(unsigned int uin)
{
    m_uiUin = uin;
}

unsigned int CFightUnitManager::GetOwner()
{
    return m_uiUin;
}

//获取战斗单位信息
CFightUnitObj* CFightUnitManager::GetFightUnitByID(int iFightUnitID)
{
    for(int i=0; i<m_iFightUnitNum; ++i)
    {
        CFightUnitObj* pstFightUnitObj = GameType<CFightUnitObj>::Get(m_aiUnitObjID[i]);
        if(pstFightUnitObj && pstFightUnitObj->GetFightUnitID()==iFightUnitID)
        {
            return pstFightUnitObj;
        }
    }

    return NULL;
}

//战斗背包中增加战斗单位
int CFightUnitManager::AddFightUnit(int iFightUnitID)
{
    if(m_iFightUnitNum >= MAX_ROLE_FIGHTUNIT_NUM)
    {
        LOGERROR("Failed to add fight unit, number reach max, uin %u!\n", m_uiUin);

        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //判断相同ID的是否存在
    CFightUnitObj* pstUnitObj = GetFightUnitByID(iFightUnitID);
    if(pstUnitObj)
    {
        return T_SERVER_SUCESS;
    }

    m_aiUnitObjID[m_iFightUnitNum] = GameType<CFightUnitObj>::Create();
    if(m_aiUnitObjID[m_iFightUnitNum] < 0)
    {
        LOGERROR("Failed to create fight unit obj, uin %u\n", m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    CFightUnitObj* pstFightUnitObj = GameType<CFightUnitObj>::Get(m_aiUnitObjID[m_iFightUnitNum]);
    if(!pstFightUnitObj)
    {
        LOGERROR("Failed to get fight unit obj, uin %u\n", m_uiUin);

        GameType<CFightUnitObj>::Del(m_aiUnitObjID[m_iFightUnitNum]);

        return T_ZONE_SYSTEM_PARA_ERR;
    }

    pstFightUnitObj->SetOwner(m_uiUin);

    int iRet = pstFightUnitObj->InitFightUnitByID(iFightUnitID);
    if(iRet)
    {
        LOGERROR("Failed to add fight unit, id %d, uin %u\n", iFightUnitID, m_uiUin);

        GameType<CFightUnitObj>::Del(m_aiUnitObjID[m_iFightUnitNum]);

        return iRet;
    }

    ++m_iFightUnitNum;

    return T_SERVER_SUCESS;
}

//获取阵型战斗单位信息
void CFightUnitManager::GetFormFightUnitInfo(std::vector<int>& vFormUnitInfo)
{
    for(int i=0; i<MAX_FIGHTUNIT_ON_FORM; ++i)
    {
        if(m_aiFightForm[i] == 0)
        {
            break;
        }

        vFormUnitInfo.push_back(m_aiFightForm[i]);
    }

    return;
}

int CFightUnitManager::InitFightUnitInfoFromDB(const FIGHTDBINFO& rstFightUnitInfo)
{
    const FightUnitRepInfo& stUnitRep = rstFightUnitInfo.stunitrep();

    //初始化玩家所有战斗单位的信息
    for(int i=0; i<stUnitRep.stunits_size(); ++i)
    {
        m_aiUnitObjID[i] = GameType<CFightUnitObj>::Create();
        if(m_aiUnitObjID[i] < 0)
        {
            LOGERROR("Failed to create fight unit obj, uin %u\n", m_uiUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        CFightUnitObj* pstFightUnitObj = GameType<CFightUnitObj>::Get(m_aiUnitObjID[i]);
        if(!pstFightUnitObj)
        {
            LOGERROR("Failed to get fight unit obj, uin %u\n", m_uiUin);
            GameType<CFightUnitObj>::Del(m_aiUnitObjID[i]);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        pstFightUnitObj->SetOwner(m_uiUin);
        pstFightUnitObj->InitFightUnitFromDB(stUnitRep.stunits(i));

        ++m_iFightUnitNum;
    }

    //初始化战斗阵型
    for(int i=0; i<rstFightUnitInfo.stforms().ifightunitids_size(); ++i)
    {
        m_aiFightForm[i] = rstFightUnitInfo.stforms().ifightunitids(i);
    }

    return T_SERVER_SUCESS;
}

int CFightUnitManager::UpdateFightUnitInfoToDB(FIGHTDBINFO& rstFightUnitInfo)
{
    FightUnitRepInfo* pstUnitRep = rstFightUnitInfo.mutable_stunitrep();

    //更新玩家所有战斗单位的信息
    for(int i=0; i<m_iFightUnitNum; ++i)
    {
        CFightUnitObj* pstFightUnitObj = GameType<CFightUnitObj>::Get(m_aiUnitObjID[i]);
        if(!pstFightUnitObj)
        {
            LOGERROR("Failed to get fight unit obj, index %d, uin %u\n", m_aiUnitObjID[i], m_uiUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        pstFightUnitObj->UpdateFightUnitToDB(*pstUnitRep->add_stunits());
    }

    //更新玩家阵型信息
    for(int i=0; i<MAX_FIGHTUNIT_ON_FORM; ++i)
    {
        if(m_aiFightForm[i] == 0)
        {
            break;
        }

        rstFightUnitInfo.mutable_stforms()->add_ifightunitids(m_aiFightForm[i]);
    }

    return T_SERVER_SUCESS;
}

//清空内存中的FightUnitObj
void CFightUnitManager::ClearFightUnitObj()
{
    for(int i=0; i<m_iFightUnitNum; ++i)
    {
        GameType<CFightUnitObj>::Del(m_aiUnitObjID[i]);
    }

    m_iFightUnitNum = 0;

    return;
}

