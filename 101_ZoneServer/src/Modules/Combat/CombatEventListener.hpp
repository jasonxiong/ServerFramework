
#ifndef __COMBAT_EVENT_LISTENER_HPP__
#define __COMBAT_EVENT_LISTENER_HPP__

class CGameRoleObj;

//战斗事件的监听处理
class CCombatEventListener
{
public:
    CCombatEventListener();
    ~CCombatEventListener();

public:
    void OnCombatBeginEvent(CGameRoleObj& stRoleObj);
    void OnCombatEndEvent(CGameRoleObj& stRoleObj);
    void OnRoundBeginEvent(CGameRoleObj& stRoleObj);
    void OnRoundEndEvent(CGameRoleObj& stRoleObj);
    void OnActionRoundBeginEvent(CGameRoleObj& stRoleObj);
    void OnActionRoundEndEvent(CGameRoleObj& stRoleObj);
    void OnCombatUnitMoveEvent(CGameRoleObj& stRoleObj);
    void OnCombatUnitActionEvent(CGameRoleObj& stRoleObj);

private:
    unsigned int m_uiUin;   //当前战斗事件关联角色的uin
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
