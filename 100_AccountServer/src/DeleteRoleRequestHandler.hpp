#ifndef __DELETE_ROLE_REQUEST_HANDLER_HPP__
#define __DELETE_ROLE_REQUEST_HANDLER_HPP__

#include "Handler.hpp"

class CAccountHandlerSet;

// 处理来自LotusServer的MSGID_ACCOUNT_DELETEROLE_REQUEST消息
class CDeleteRoleRequestHandler : public IHandler
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
    CDeleteRoleRequestHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult);

private:
    void SendDeleteRoleRequestToWorld();
    void SendFailedResponseToLotus(const unsigned int uiResultID);
    int CheckParam();
};

#endif // __DELETE_ROLE_REQUEST_HANDLER_HPP__

