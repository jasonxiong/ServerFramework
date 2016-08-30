#ifndef __REGISTER_ACCOUNT_HANDLER_HPP__
#define __REGISTER_ACCOUNT_HANDLER_HPP__

#include "Handler.hpp"

class CRegAuthHandlerSet;

// 处理来自LotusServer的 MSGID_REGAUTH_REGACCOUNT_REQUEST 消息
class CRegisterAccountHandler : public IHandler
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
    CRegisterAccountHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult);

private:
    int OnRequestRegAccount();

private:

    void SendFailedResponseToLotus(const unsigned int uiResultID);

    int CheckParam();
};

#endif // __CREATE_ROLE_REQUEST_HANDLER_HPP__


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
