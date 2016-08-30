#ifndef __FETCH_ACCOUNT_HANDLER_HPP__
#define __FETCH_ACCOUNT_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "AccountDBPublic.hpp"

class CAccountDBHandlerSet;

//MSGID_ACCOUNTDB_FETCH_REQUEST 消息处理者
class CFetchAccountHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;
    GameProtocolMsg* m_pstRequestMsg; // 待处理的消息

	int m_iThreadIdx;

    //生成的SQL语句
    static char m_szQueryString[GameConfig::ACCOUNT_TABLE_SPLIT_FACTOR][512];

public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // 本类的对象只能在CAccountDBHandlerSet类中创建
    friend class CAccountDBHandlerSet;
    CFetchAccountHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:

    //拉取返回帐号详细信息
    int FetchAccountInfo(const AccountID& stAccountID, const std::string& strPassword, AccountDB_FetchAccount_Response& rstResp);

    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg& stResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg& pstResponseMsg);
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
