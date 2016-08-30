#ifndef __FIGHT_UNIT_UTILITY_HPP__
#define __FIGHT_UNIT_UTILITY_HPP__

class CGameRoleObj;
class CFightUnitUtility
{
public:
    
    //获取战斗单位属性
    static int GetFightUnitAttr(CGameRoleObj& rstRoleObj, int iFightUnitID, int iAttrType);

    //增加战斗单位属性
    static int AddFightUnitAttr(CGameRoleObj& rstRoleObj, int iFightUnitID, int iAttrType, int iAddAttrNum);

    //保存战斗单位的属性
    static int SaveCombatUnitAttr(CGameRoleObj& stRoleObj, int iFightUnitID, int iAttrType, int iSaveNum);
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
