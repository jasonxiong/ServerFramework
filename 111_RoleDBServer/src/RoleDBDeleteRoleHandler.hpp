#ifndef __ROLE_DB_DELETE_ROLE_HANDLER_HPP__
#define __ROLE_DB_DELETE_ROLE_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "RoleDBPublic.hpp"

class CRoleDBHandlerSet;

// MSGID_ACCOUNT_DELETEROLE_REQUEST消息处理者
class CRoleDBDeleteRoleHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // 访问数据库的指针
    GameProtocolMsg* m_pstRequestMsg; // 待处理的消息

	int m_iThreadIdx;

    static char m_szQueryString[GameConfig::ROLE_TABLE_SPLIT_FACTOR][512];

public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // 本类的对象只能在CRoleDBHandlerSet类中创建
    friend class CRoleDBHandlerSet;
    CRoleDBDeleteRoleHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:
    int DeleteRole(const RoleID& stRoleID);
    int QueryRoleInfo(const RoleID& stRoleID);
    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg);
};

#endif // __ROLE_DB_DELETE_ROLE_HANDLER_HPP__
