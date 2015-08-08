#ifndef __ACCOUNT_ADD_NEW_NAME_HANDLER_HPP__
#define __ACCOUNT_ADD_NEW_NAME_HANDLER_HPP__

#include "Handler.hpp"

class CAccountHandlerSet;

// 处理来自World的 MSGID_ADDNEWNAME_RESPONSE 消息
class CAccountAddNewNameHandler : public IHandler
{
private:
    // 本类的对象只能在CAccountHandlerSet类中创建
    friend class CAccountHandlerSet;
    CAccountAddNewNameHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg, SHandleResult* pstResult);

private:

    //发送创建角色的请求
    int SendCreateRoleRequestToWorld(const std::string& strName, unsigned int uin);

    //发送创建角色失败的请求到Lotus
    void SendCreateRoleFailedToLotus(unsigned int uin, int iResult);

    //删除NameDB中的名字
    int SendDeleteNameRequestToWorld(const std::string& strName, int iType, unsigned uin);

private:
    GameProtocolMsg* m_pRequestMsg;
};

#endif // __ACCOUNT_ADD_NEW_NAME_HANDLER_HPP__
