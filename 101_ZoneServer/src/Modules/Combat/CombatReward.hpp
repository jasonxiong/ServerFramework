#ifndef __COMBAT_REWARD_HPP__
#define __COMBAT_REWARD_HPP__

//战斗奖励的处理

#include "GameProtocol.hpp"

class CGameRoleObj;
class CCombatReward
{
public:
    
    //领取战斗奖励
    static int GetCombatReward(int iLevelID, CGameRoleObj& stRoleObj, CombatReward& stReward);
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
