#ifndef __WORLD_PROTOCOL_ENGINE_W_HPP__
#define __WORLD_PROTOCOL_ENGINE_W_HPP__

#include "UnixTime.hpp"
#include "WorldGlobalDefine.hpp"
#include "HandlerFactory.hpp"
#include "FetchRoleWorldHandler.hpp"
#include "UpdateRoleWorldHandler.hpp"
#include "CreateRoleAccountHandler.hpp"
#include "DeleteRoleAccountHandler.hpp"
#include "ListRoleAccountHandler.hpp"
#include "GameMasterHandler.hpp"
#include "LogoutHandler.hpp"
#include "KickRoleWorldHandler.hpp"
#include "ConfigHelper.hpp"
#include "WorldNameHandler.hpp"
#include "ChatHandler.hpp"
#include "ListZoneHandler.hpp"
#include "OnlineStatHandler.hpp"

class CAppLoopW;

class CWorldProtocolEngineW
{
public:
    int Initialize(bool bResume);
    void SetService(CAppLoopW* pAppLoop);

public:
    // 将数据解包, 并交给Handler处理
    int OnRecvCode(char* pszMsgBuffer, const int iMsgLength, EGameServerID enMsgPeer, int iInstance = 1);

    // 将数据打包, 并发送到对端
    int SendWorldMsg(const GameProtocolMsg& rstWorldMsg, EGameServerID enMsgPeer, int iInstance = 1);

private:

    int ForwardMsg(char* pszMsgBuffer, const int iMsgLength);

private:
    char m_szCode[MAX_MSGBUFFER_SIZE];

    GameProtocolMsg m_stWorldMsg;

    CAppLoopW* m_pAppLoop;

public:
    int RegisterAllHandler();

private:
    CFetchRoleWorldHandler m_stFetchRoleWorldHandler;
    CUpdateRoleWorldHandler m_stUpdateRoleWorldHandler;
    CGameMasterHandler m_stGameMasterHandler;
    CCreateRoleAccountHandler m_stCreateRoleAccountHandler;
    CDeleteRoleAccountHandler m_stDeleteRoleAccountHandler;
    CListRoleAccountHandler m_stListRoleAccountHandler;

    CWorldNameHandler m_stWorldNameHandler;

    COnlineStatHandler m_OnlineStatHandler;

    CLogoutHandler m_stLogoutHandler;
    CKickRoleWorldHandler m_stKickRoleWorldHandler;
    CChatHandler m_stChatHandler;
    CListZoneHandler m_stListZoneHandler;
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
