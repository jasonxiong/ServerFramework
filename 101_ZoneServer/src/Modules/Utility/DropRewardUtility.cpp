
#include "GameProtocol.hpp"
#include "ZoneErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "RepThingsUtility.hpp"
#include "Random.hpp"
#include "ModuleShortcut.hpp"
#include "GameRole.hpp"
#include "FightUnitObj.hpp"

#include "DropRewardUtility.hpp"

//获取随机奖励
int CDropRewardUtility::GetDropReward(int iDropRewardID, CGameRoleObj& stRoleObj, CombatReward& stReward, int iItemChannel, int iRandParam)
{
    if(iDropRewardID == 0)
    {
        //没有配置随机奖励
        return T_SERVER_SUCESS;
    }

    //获取随机奖励的配置
    const SDropRewardConfig* pstRewardConfig = DropRewardCfgMgr().GetConfig(iDropRewardID);
    if(!pstRewardConfig)
    {
        LOGERROR("Failed to get drop reward config, invalid reward id %d\n", iDropRewardID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //随机奖励的内容
    int iRandMax = 0;
    for(int i=0; i<pstRewardConfig->iTotalRewardNum; ++i)
    {
        iRandMax += pstRewardConfig->astRewards[i].iDropRate;
    }

    int iRandNum = CRandomCalculator::GetRandomNumberInRange(iRandMax)*iRandParam/100;
    int iIndex = -1;
    for(int i=0; i<pstRewardConfig->iTotalRewardNum; ++i)
    {
        if(iRandNum <= pstRewardConfig->astRewards[i].iDropRate)
        {
            iIndex = i;
            break;
        }

        iRandNum -= pstRewardConfig->astRewards[i].iDropRate;
    }

    if(iIndex < 0)
    {
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //随机奖励的数量
    int iNum = CRandomCalculator::GetRandomNumber(pstRewardConfig->astRewards[iIndex].iDropMinNum, pstRewardConfig->astRewards[iIndex].iDropMaxNum);

    //根据类型增加奖励
    int iRet = T_SERVER_SUCESS;
    const OneRewardConfig& stOneConfig = pstRewardConfig->astRewards[iIndex];
    switch(stOneConfig.iDropType)
    {
        case REWARD_TYPE_ITEM:
            {
                iRet = CRepThingsUtility::AddItemNumByID(stRoleObj, stOneConfig.iDropID, iNum, iItemChannel);
                if(iRet == 0)
                {
                    //添加物品成功，增加返回
                    OneRewardInfo* pstOneInfo = stReward.add_strewardinfo();
                    pstOneInfo->set_irewardtype(stOneConfig.iDropType);
                    pstOneInfo->set_iitemid(stOneConfig.iDropID);
                    pstOneInfo->set_idropnum(iNum);                
                }
            }
            break;

        default:
            {
                LOGERROR("Failed to add drop reward, invalid reward type %d, reward id %d\n", pstRewardConfig->astRewards[iIndex].iDropType, iDropRewardID);
                return T_ZONE_SYSTEM_INVALID_CFG;
            }
            break;
    }

    if(pstRewardConfig->iAddedRewardID != 0)
    {
        //有关联奖励，增加关联奖励
        return GetDropReward(pstRewardConfig->iAddedRewardID, stRoleObj, stReward, iItemChannel);
    }

    return T_SERVER_SUCESS;
}

//获取随机奖励
int CDropRewardUtility::GetDropReward(int iDropRewardID, CGameRoleObj& stRoleObj, int iItemChannel, int iRandParam)
{
    static CombatReward stReward;
    stReward.Clear();

    return GetDropReward(iDropRewardID,stRoleObj,stReward, iItemChannel, iRandParam);
}
