#ifndef __FIGHT_UNIT_MANAGER_HPP__
#define __FIGHT_UNIT_MANAGER_HPP__

//玩家战斗单位管理器

#include "GameConfigDefine.hpp"
#include "GameProtocol.hpp"

using namespace GameConfig;

class CFightUnitObj;
class CGameRoleObj;

class CFightUnitManager
{
public:
    CFightUnitManager();
    ~CFightUnitManager();

public:

    //初始化 FightUnitManager
    int Initialize();

    void SetOwner(unsigned int uin);
    unsigned int GetOwner();

    //获取战斗单位信息
    CFightUnitObj* GetFightUnitByID(int iFightUnitID);

    //战斗背包中增加战斗单位
    int AddFightUnit(int iFightUnitID);

    //获取阵型战斗单位信息
    void GetFormFightUnitInfo(std::vector<int>& vFormUnitInfo);

    int InitFightUnitInfoFromDB(const FIGHTDBINFO& rstFightUnitInfo);
    int UpdateFightUnitInfoToDB(FIGHTDBINFO& rstFightUnitInfo);

    //清空内存中的FightUnitObj
    void ClearFightUnitObj();

private:

    //玩家角色的uin
    unsigned int m_uiUin;

    //玩家所有的战斗单位的信息
    int m_iFightUnitNum;
    int m_aiUnitObjID[MAX_ROLE_FIGHTUNIT_NUM];

    //玩家阵型信息
    int m_aiFightForm[MAX_FIGHTUNIT_ON_FORM];

    static GameProtocolMsg ms_stZoneMsg; 
};

#endif
