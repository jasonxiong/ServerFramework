#ifndef __LIST_ROLE_REQUEST_HANDLER_HPP__
#define __LIST_ROLE_REQUEST_HANDLER_HPP__

#include "Handler.hpp"

class CAccountHandlerSet;

// 处理来自LotusServer的MSGID_ACCOUNT_LISTROLE_REQUEST消息
class CListRoleRequestHandler : public IHandler
{
private:
    TNetHead_V2* m_pstNetHead;  // 客户连接
    GameProtocolMsg* m_pstRequestMsg; // 待处理的消息

    // 下面两个变量用于标识一个session
    unsigned int m_uiSessionFD;
    unsigned short m_unValue;

private:
    // 本类的对象只能在CAccountHandlerSet类中创建
    friend class CAccountHandlerSet;
    CListRoleRequestHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult);

private:
    void SendListRoleRequestToWorld();
    void SendResponseToLotus(const unsigned int uiResultID);
    int CheckParam(unsigned int& rushResultID);
};

#endif // __LIST_ROLE_REQUEST_HANDLER_HPP__


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
