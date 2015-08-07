
#ifndef __CHAT_HANDLER_HPP__
#define __CHAT_HANDLER_HPP__

#include "Handler.hpp"
#include "WorldGlobalDefine.hpp"
#include "GameProtocol.hpp"

class CChatHandler : public IHandler
{
public:
    virtual ~CChatHandler();

public:
    virtual int OnClientMsg(const void* pMsg);

private:
    int OnRequestChatNotify();

private:
    GameProtocolMsg* m_pMsg;
    GameProtocolMsg m_stWorldMsg;
};

#endif


