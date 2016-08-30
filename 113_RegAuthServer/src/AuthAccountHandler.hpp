
#ifndef __AUTH_ACCOUNT_HANDLER_HPP__
#define __AUTH_ACCOUNT_HANDLER_HPP__

#include "Handler.hpp"

//玩家登录认证的消息处理

class CRegAuthHandlerSet;

// 处理来自LotusServer的MSGID_AUTHACCOUNT_REQUEST消息
class CAuthAccountHandler : public IHandler
{
private:
    TNetHead_V2* m_pstNetHead;  // 客户连接
    GameProtocolMsg* m_pstRequestMsg; // 待处理的消息

    // 下面两个变量用于标识一个session
    unsigned int m_uiSessionFD;
    unsigned short m_unValue;

private:
    // 本类的对象只能在CRegAuthHandlerSet类中创建
    friend class CRegAuthHandlerSet;
    CAuthAccountHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg, SHandleResult* pstResult);

private:
    int OnRequestAuthAccount();

private:

    //发送认证失败的回复给LotusServer
    void SendFailedResponseToLotus(unsigned int uiResult);

    int CheckParam();
};

#endif // __AUTH_ACCOUNT_HANDLER_HPP__


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
