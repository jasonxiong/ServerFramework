#ifndef __CLIENT_CLOSED_HANDLER_HPP__
#define __CLIENT_CLOSED_HANDLER_HPP__

#include "Handler.hpp"

class CAccountHandlerSet;

// 处理来自LotusServer的MSGID_LOGOUTSERVER_REQUEST消息
class CClientClosedHandler : public IHandler
{
private:
    // 本类的对象只能在CAccountHandlerSet类中创建
    friend class CAccountHandlerSet;
    CClientClosedHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult);
};

#endif // __CLIENT_CLOSED_HANDLER_HPP__

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
