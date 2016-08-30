#ifndef __HANDLER_LIST_HPP__
#define __HANDLER_LIST_HPP__

#include "LoginHandler.hpp"
#include "LogoutHandler.hpp"
#include "FetchRoleWorldHandler.hpp"
#include "UpdateRoleInfoHandler.hpp"
#include "KickRoleWorldHandler.hpp"
#include "GMCommandHandler.hpp"
#include "RepThingsHandler.hpp"
#include "ChatHandler.hpp"
#include "FightUnitHandler.hpp"
#include "CombatHandler.hpp"

class CHandlerList
{
public:
	int RegisterAllHandler();

private:
	// 所有的消息处理函数
	CLoginHandler m_stLoginHandler;
	CLogoutHandler m_stLogoutHandler;
    CFetchRoleWorldHandler m_stFetchRoleWorldHandler;
	CUpdateRoleInfo_Handler m_stUpdateRoleInfoHandler;
    CKickRoleWorldHandler m_stKickRoleWorldHandler;
    CGMCommandHandler m_stGMCommandHandler;
    CRepThingsHandler m_stRepThingsHandler;
    CChatHandler m_stChatHandler;
    CCombatHandler m_stCombatHandler;
	CFightUnitHandler m_stFightUnitHandler;
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
