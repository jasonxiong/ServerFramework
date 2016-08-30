
#ifndef __COMBAT_EVENT_MANAGER_HPP__
#define __COMBAT_EVENT_MANAGER_HPP__

#include "CombatEventListener.hpp"

class CGameRoleObj;

//战斗事件管理器
class CCombatEventManager
{
public:

    //战斗开始事件
    static void NotifyCombatBegin(CGameRoleObj& stRoleObj);

    //战斗结束事件
    static void NotifyCombatEnd(CGameRoleObj& stRoleObj);

    //回合开始事件
    static void NotifyRoundBegin(CGameRoleObj& stRoleObj);

    //回合结束事件
    static void NotifyRoundEnd(CGameRoleObj& stRoleObj);

    //战斗单位行动回合开始事件
    static void NotifyActionRoundBegin(CGameRoleObj& stRoleObj);

    //战斗单位行动回合结束事件
    static void NotifyActionRoundEnd(CGameRoleObj& stRoleObj);

    //战斗单位移动的事件
    static void NotifyCombatUnitMove(CGameRoleObj& stRoleObj);

    //战斗单位行动的事件
    static void NotifyCombatUnitAction(CGameRoleObj& stRoleObj);

private:
    static CCombatEventListener m_stCombatEventListener;
};
    
#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
