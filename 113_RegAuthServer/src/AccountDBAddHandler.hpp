#ifndef __ACCOUNT_DB_ADD_HANDLER_HPP__
#define __ACCOUNT_DB_ADD_HANDLER_HPP__

#include "Handler.hpp"

class CRegAuthHandlerSet;

// 处理来自AccountDB Server的 MSGID_ACCOUNTDB_ADDACCOUNT_RESPONSE 消息
class CAccountDBAddHandler : public IHandler
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
    CAccountDBAddHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg, SHandleResult* pstResult);

    //新增加帐号
    static void AddAccount(unsigned uiSessionFd, const AccountID& stAccountID, int iWorldID, const std::string& strPasswd);

private:
    int OnResponseAccountDBAdd();

private:

    void SendResponseToLotus(unsigned int uiSessionFd, const unsigned int uiResultID);
};

#endif // __ACCOUNT_DB_ADD_HANDLER_HPP__


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
