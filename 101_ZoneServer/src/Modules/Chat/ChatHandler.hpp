
#ifndef __CHAT_HANDLER_HPP__
#define __CHAT_HANDLER_HPP__

#include "Handler.hpp"
#include "GameProtocol.hpp"
#include "SessionDefine.hpp"
#include "ZoneErrorNumDef.hpp"

class CGameRoleObj;

class CChatHandler: public IHandler
{
public:
    virtual ~CChatHandler(void);
    CChatHandler();
public:
    int OnClientMsg();

private:

    //聊天消息的处理
    int OnRequestChat();

    //世界聊天消息的推送
    int OnNotifyChat();

private:
    int SendFailedResponse(int iMsgID, int iResultID, const TNetHead_V2& rstNetHead);

    //发送成功的回复
    int SendSuccessfulResponse();

protected:
	static GameProtocolMsg m_stGameMsg;
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
