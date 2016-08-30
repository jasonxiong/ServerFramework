
#include "GameRole.hpp"
#include "CombatEventListener.hpp"

#include "CombatEventManager.hpp"

CCombatEventListener CCombatEventManager::m_stCombatEventListener;

//战斗开始事件
void CCombatEventManager::NotifyCombatBegin(CGameRoleObj& stRoleObj)
{
    m_stCombatEventListener.OnCombatBeginEvent(stRoleObj);
}

//战斗结束事件
void CCombatEventManager::NotifyCombatEnd(CGameRoleObj& stRoleObj)
{
    m_stCombatEventListener.OnCombatEndEvent(stRoleObj);
}

//回合开始事件
void CCombatEventManager::NotifyRoundBegin(CGameRoleObj& stRoleObj)
{
    m_stCombatEventListener.OnRoundBeginEvent(stRoleObj);
}

//回合结束事件
void CCombatEventManager::NotifyRoundEnd(CGameRoleObj& stRoleObj)
{
    m_stCombatEventListener.OnRoundEndEvent(stRoleObj);
}

//战斗单位行动回合开始事件
void CCombatEventManager::NotifyActionRoundBegin(CGameRoleObj& stRoleObj)
{
    m_stCombatEventListener.OnActionRoundBeginEvent(stRoleObj);
}

//战斗单位行动回合结束事件
void CCombatEventManager::NotifyActionRoundEnd(CGameRoleObj& stRoleObj)
{
    m_stCombatEventListener.OnActionRoundEndEvent(stRoleObj);
}

//战斗单位移动的事件
void CCombatEventManager::NotifyCombatUnitMove(CGameRoleObj& stRoleObj)
{
    m_stCombatEventListener.OnCombatUnitMoveEvent(stRoleObj);
}

//战斗单位行动的事件
void CCombatEventManager::NotifyCombatUnitAction(CGameRoleObj& stRoleObj)
{
    m_stCombatEventListener.OnCombatUnitActionEvent(stRoleObj);
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
