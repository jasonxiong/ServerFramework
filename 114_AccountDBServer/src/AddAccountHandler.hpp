#ifndef __ADD_ACCOUNT_HANDLER_HPP__
#define __ADD_ACCOUNT_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "AccountDBPublic.hpp"

//新增帐号相关请求的处理

class CAccountDBHandlerSet;

// MSGID_ACCOUNTDB_ADDACCOUNT_REQUEST 消息处理者
class CAddAccountHandler : public CHandler
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
    CAddAccountHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:

    //进行必要的参数检查
    int CheckParams();

    void OnAddAccountRequest(SHandleResult* pstHandleResult);

    //检查帐号是否存在
    int CheckAccountExist(const AccountID& stAccountID, bool& bIsExist);

    //获取还未被使用的UIN
    int GetAvaliableUin(unsigned int& uin);

    //插入新的记录
    int AddNewRecord(const AccountID& stAccountID, unsigned int uin, int iWorldID, const std::string& strPassword);

private:
    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg& stResponseMsg);
};

#endif // __ADD_ACCOUNT_HANDLER_HPP__
