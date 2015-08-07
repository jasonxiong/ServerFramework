#ifndef __UPDATE_ACCOUNT_HANDLER_HPP__
#define __UPDATE_ACCOUNT_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "AccountDBPublic.hpp"

class CAccountDBHandlerSet;

//MSGID_ACCOUNTDB_UPDATE_REQUEST 消息处理者
class CUpdateAccountHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // 访问数据库的指针
    GameProtocolMsg* m_pstRequestMsg; // 待处理的消息

	//所属线程idx
	int m_iThreadIdx;

    //生成的SQL语句
    static char m_szQueryString[GameConfig::ACCOUNT_TABLE_SPLIT_FACTOR][1024];

public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // 本类的对象只能在CAccountDBHandlerSet类中创建
    friend class CAccountDBHandlerSet;
    CUpdateAccountHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:
    void OnUpdateAccountRequest(SHandleResult* pstHandleResult);

private:

    //检查帐号密码的有效性
    int UpdateSecurityCheck(const AccountID& stAccountID, const std::string& strPassword);

    //更新帐号信息
    int UpdateAccountInfo(const AccountID& stAccountID);

    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg);
};

#endif
