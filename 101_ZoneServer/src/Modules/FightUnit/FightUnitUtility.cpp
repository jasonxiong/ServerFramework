
#include "GameProtocol.hpp"
#include "ZoneErrorNumDef.hpp"
#include "GameRole.hpp"
#include "FightUnitManager.hpp"
#include "FightUnitObj.hpp"
#include "ModuleShortcut.hpp"
#include "ZoneMsgHelper.hpp"

#include "FightUnitUtility.hpp"

//获取战斗单位属性
int CFightUnitUtility::GetFightUnitAttr(CGameRoleObj& rstRoleObj, int iFightUnitID, int iAttrType)
{
    //获取战斗单位管理器
    CFightUnitManager& stUnitManager = rstRoleObj.GetFightUnitManager();

    //获取战斗单位
    CFightUnitObj* pstUnitObj = stUnitManager.GetFightUnitByID(iFightUnitID);
    if(!pstUnitObj)
    {
        return 0;
    }

    return pstUnitObj->GetFightUnitAttr(iAttrType);
}

//增加战斗单位属性
int CFightUnitUtility::AddFightUnitAttr(CGameRoleObj& rstRoleObj, int iFightUnitID, int iAttrType, int iAddAttrNum)
{
    //获取战斗单位管理器
    CFightUnitManager& stUnitManager = rstRoleObj.GetFightUnitManager();

    //获取战斗单位
    CFightUnitObj* pstUnitObj = stUnitManager.GetFightUnitByID(iFightUnitID);
    if(!pstUnitObj)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    return pstUnitObj->AddFightUnitAttr(iAttrType, iAddAttrNum);
}

//保存战斗单位的属性
int CFightUnitUtility::SaveCombatUnitAttr(CGameRoleObj& stRoleObj, int iFightUnitID, int iAttrType, int iSaveNum)
{
    CFightUnitManager& stFightUnitMgr = stRoleObj.GetFightUnitManager();
    CFightUnitObj* pstUnitObj = stFightUnitMgr.GetFightUnitByID(iFightUnitID);
    if(!pstUnitObj)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //最少为1点
    iSaveNum = iSaveNum<=0 ? 1 : iSaveNum;

    pstUnitObj->AddFightUnitAttr(iAttrType, iSaveNum-pstUnitObj->GetFightUnitAttr(iAttrType));

    return T_SERVER_SUCESS;
}

