#ifndef __ACCOUNT_DB_FETCH_HANDLER_HPP__
#define __ACCOUNT_DB_FETCH_HANDLER_HPP__

#include "Handler.hpp"

class CRegAuthHandlerSet;
struct WulinServerConfig;

// 处理来自AccountDB Server的 MSGID_ACCOUNTDB_FETCH_REQUEST 消息
class CAccountDBFetchHandler : public IHandler
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
    CAccountDBFetchHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg, SHandleResult* pstResult);

    //拉取帐号详细信息
    static void FetchAccount(unsigned uiSessionFd, const AccountID& stAccountID, const std::string& strPassword);

private:
    int OnResponseAccountDBFetch();

private:
    
    //发送认证失败回复给LotusServer
    void SendAuthFailedResponseToLotus(unsigned int uiSessionFd, const unsigned int uiResultID);

    //发送认证成功回复给LotusServer
    void SendAuthSuccessResponseToLotus(unsigned int uiSessionFd, const WulinServerConfig& stServerConfig, unsigned int uin, bool bIsBinded);
};

#endif // __ACCOUNT_DB_FETCH_HANDLER_HPP__

