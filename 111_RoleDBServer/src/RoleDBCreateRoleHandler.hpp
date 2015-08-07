#ifndef __ROLE_DB_CREATE_ROLE_HANDLER_HPP__
#define __ROLE_DB_CREATE_ROLE_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "RoleDBPublic.hpp"

class CRoleDBHandlerSet;

// MSGID_ACCOUNT_CREATEROLE_REQUEST消息处理者
class CRoleDBCreateRoleHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;
    GameProtocolMsg* m_pstRequestMsg; // 待处理的消息

	int m_iThreadIdx;

    static char m_szQueryString[GameConfig::ROLE_TABLE_SPLIT_FACTOR][10240];

public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // 本类的对象只能在CRoleDBHandlerSet类中创建
    friend class CRoleDBHandlerSet;
    CRoleDBCreateRoleHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:
    int InsertNewRoleRecord(const World_CreateRole_Request& rstCreateRoleRequest, unsigned int uiSeq);

    //获取玩家角色创建的时间
    void GetRoleCreateTime(unsigned short& usCreateTime);

    //生成插入的SQL Query语句
    int GenerateQueryString(const World_CreateRole_Request& rstCreateRoleRequest, unsigned int uiSeq, char* pszBuff, int iBuffLen, int& iLength);

    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(unsigned int uiSeq, GameProtocolMsg* pstResponseMsg);
};

#endif // __ROLE_DB_CREATE_ROLE_HANDLER_HPP__
