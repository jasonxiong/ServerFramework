#ifndef __ROLE_DB_FETCH_ROLE_HANDLER_HPP__
#define __ROLE_DB_FETCH_ROLE_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "RoleDBPublic.hpp"
#include "SeqConverter.hpp"

class CRoleDBHandlerSet;

// MSGID_ROLEDB_FETCHROLE_REQUEST消息处理者
class CRoleDBFetchRoleHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // 访问数据库的指针
    GameProtocolMsg* m_pstRequestMsg; // 待处理的消息

	//所属线程idx
	int m_iThreadIdx;

    //SQL语句字符串缓冲区
    static char m_szQueryString[GameConfig::ROLE_TABLE_SPLIT_FACTOR][512];

public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // 本类的对象只能在CRoleDBHandlerSet类中创建
    friend class CRoleDBHandlerSet;
    CRoleDBFetchRoleHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:
    void OnFetchRoleRequest(SHandleResult* pstHandleResult);

private:
    int QueryRole(const RoleID& stRoleID, GameUserInfo& rstDBUserInfo);
    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg);
};

#endif // __ROLE_DB_FETCH_ROLE_HANDLER_HPP__

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
