#ifndef __REGISTER_ACCOUNT_HANDLER_HPP__
#define __REGISTER_ACCOUNT_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "AccountDBPublic.hpp"

class CAccountDBHandlerSet;

// MSGID_ACCOUNTDB_CREATEROLE_REQUEST消息处理者
class CRegisterAccountHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;
    GameProtocolMsg* m_pstRequestMsg; // 待处理的消息

	int m_iThreadIdx;
public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // 本类的对象只能在CAccountDBHandlerSet类中创建
    friend class CAccountDBHandlerSet;
    CRegisterAccountHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:

    //先检查相同的帐号是否已经存在
    int CheckAccountIsExist(bool& bIsAccountExist);

    //获取当前可用可注册的Uin
    int GetAccountUin(unsigned& uiUin);

    //向数据库中插入一条新注册帐号的记录
    int InsertNewAccountRecord(const RegAuth_RegAccount_Request& rstReq, unsigned int uiUin);

    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(unsigned int uiUin, GameProtocolMsg* pstResponseMsg);
};

#endif // __ROLE_DB_CREATE_ROLE_HANDLER_HPP__
