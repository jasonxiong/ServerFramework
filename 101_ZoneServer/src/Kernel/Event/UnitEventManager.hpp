
#ifndef __UNIT_EVENT_MANAGER_HPP__
#define __UNIT_EVENT_MANAGER_HPP__

#include "GameProtocol.hpp"
#include "ZoneErrorNumDef.hpp"
#include "GameObjCommDef.hpp"
#include "GameRole.hpp"
#include "SessionDefine.hpp"

//服务端所有事件通知处理的入口

class CUnitEventManager
{
public:
    void NotifyUnitLogin(TUNITINFO* pUnit);
    void NotifyUnitLogout(TUNITINFO* pUnit);

    void NotifyCombatEnd(CGameRoleObj& stRoleObj, bool bIsWin);

    void NotifyTick();
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
