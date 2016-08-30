
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "GameRole.hpp"
#include "ModuleShortcut.hpp"
#include "DropRewardUtility.hpp"

#include "CombatReward.hpp"

//领取战斗奖励
int CCombatReward::GetCombatReward(int iLevelID, CGameRoleObj& stRoleObj, CombatReward& stReward)
{
    //读取关卡配置
    const SBattleCrossConfig* pstCrossConfig = BattleCrossCfgMgr().GetConfig(iLevelID);
    if(!pstCrossConfig)
    {
        LOGERROR("Failed to get pve cross config, invalid cross id %d\n", iLevelID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //增加奖励
    int iRet = CDropRewardUtility::GetDropReward(pstCrossConfig->iDropRewardID, stRoleObj, stReward, 0);
    if(iRet)
    {
        LOGERROR("Failed to add combat reward, ret %d, uin %u\n", iRet, stRoleObj.GetUin());
        return iRet;
    }

    return T_SERVER_SUCESS;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
