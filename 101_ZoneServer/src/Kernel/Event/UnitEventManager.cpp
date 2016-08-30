
#include "UnitEventManager.hpp"
#include "ModuleHelper.hpp"
#include "GameProtocol.hpp"
#include "ZoneObjectHelper.hpp"
#include "GameSession.hpp"
#include "GameRole.hpp"
#include "LogAdapter.hpp"
#include "UnitUtility.hpp"
#include "ZoneMsgHelper.hpp"

void CUnitEventManager::NotifyUnitLogin(TUNITINFO* pUnit)
{

}

void CUnitEventManager::NotifyUnitLogout(TUNITINFO* pUnit)
{

}

void CUnitEventManager::NotifyTick()
{
    // 根据系统系统的闲忙状态, 动态调整系统负载
    EGameServerStatus enServerStatus = GetServerStatus();

    int iNumber = (enServerStatus == GAME_SERVER_STATUS_BUSY) ? 2 : 200;
    for (int i = 0; i < iNumber; i++)
    {
        int iRoleIdx = CUnitUtility::IterateRoleIdx();
        if (iRoleIdx < 0)
        {
            break;
        }

        CGameRoleObj* pRoleObj = GameTypeK32<CGameRoleObj>::GetByIdx(iRoleIdx);
        if (!pRoleObj)
        {
            continue;
        }

        TUNITINFO* pUnitInfo = &pRoleObj->GetRoleInfo().stUnitInfo;
        // 尝试删除单位
        if (CUnitUtility::IsUnitStatusSet(pUnitInfo, EGUS_DELETE))
        {
            CUnitUtility::DeleteUnit(pUnitInfo);
            continue;
        }

        // 角色Tick
        pRoleObj->OnTick();
    }

    return;
}

void CUnitEventManager::NotifyCombatEnd(CGameRoleObj& stRoleObj, bool bIsWin)
{

}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
