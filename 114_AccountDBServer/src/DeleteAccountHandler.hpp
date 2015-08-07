#ifndef __DELETE_ACCOUNT_HANDLER_HPP__
#define __DELETE_ACCOUNT_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "AccountDBPublic.hpp"

class CAccountDBHandlerSet;

// MSGID_ACCOUNTDB_DELETE_REQUEST 消息处理者
class CDeleteAccountHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // 访问数据库的指针
    GameProtocolMsg* m_pstRequestMsg; // 待处理的消息

	int m_iThreadIdx;

    //生成的SQL语句
    static char m_szQueryString[GameConfig::ACCOUNT_TABLE_SPLIT_FACTOR][256];

public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // 本类的对象只能在CAccountDBHandlerSet类中创建
    friend class CAccountDBHandlerSet;
    CDeleteAccountHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:

    //删除Account帐号
    int DeleteAccount(const AccountID& stAccountID);

    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg);
};

#endif
