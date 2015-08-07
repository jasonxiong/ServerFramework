#ifndef __LIST_ROLE_RESPONSE_HANDLER_HPP__
#define __LIST_ROLE_RESPONSE_HANDLER_HPP__

#include "Handler.hpp"

class CAccountHandlerSet;

// 处理来自WorldServer的MSGID_ACCOUNT_LISTROLE_RESPONSE消息
class CListRoleResponseHandler : public IHandler
{
private:
    GameProtocolMsg* m_pstRequestMsg; // 待处理的消息

    // 下面两个变量用于标识一个session
    unsigned int m_uiSessionFD;
    unsigned short m_unValue;

private:
    // 本类的对象只能在CAccountHandlerSet类中创建
    friend class CAccountHandlerSet;
    CListRoleResponseHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult);
private:

    //void SendUpdateNicknameRequestToName();
    void SendListRoleResponseToLotus();
};

#endif // __LIST_ROLE_RESPONSE_HANDLER_HPP__

