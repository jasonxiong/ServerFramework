#ifndef __DROP_REWARD_UTILITY_HPP__
#define __DROP_REWARD_UTILITY_HPP__

//随机掉落工具类

#include "GameProtocol.hpp"

class CGameRoleObj;
class CDropRewardUtility
{
public:

    //获取随机奖励
    static int GetDropReward(int iDropRewardID, CGameRoleObj& stRoleObj, CombatReward& stReward, int iItemChannel, int iRandParam = 100);

    //获取随机奖励
    static int GetDropReward(int iDropRewardID, CGameRoleObj& stRoleObj, int iItemChannel, int iRandParam = 100);
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
