#ifndef __AUTH_ACCOUNT_HANDLER_HPP__
#define __AUTH_ACCOUNT_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "AccountDBPublic.hpp"

//认证平台帐号的相关请求处理

class CAccountDBHandlerSet;

// MSGID_ACCOUNTDB_FETCHROLE_REQUEST消息处理者
class CAuthAccountHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // 访问数据库的指针
    GameProtocolMsg* m_pstRequestMsg; // 待处理的消息

	//所属线程idx
	int m_iThreadIdx;
public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // 本类的对象只能在CAccountDBHandlerSet类中创建
    friend class CAccountDBHandlerSet;
    CAuthAccountHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:
    void OnAuthAccountRequest(SHandleResult* pstHandleResult);

    //校验认证账户的密码
    int CheckAccountPasswd(const std::string& strAccount, int iLoginType, const std::string& strPasswd, RegAuth_AuthAccount_Response& stResp);

private:
    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg);
};

#endif // __ROLE_DB_FETCH_ROLE_HANDLER_HPP__
