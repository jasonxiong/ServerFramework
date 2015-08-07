#include <string.h>

#include "AppDef.hpp"
#include "RoleDBLogManager.hpp"
#include "SeqConverter.hpp"
#include "StringUtility.hpp"
#include "RoleDBApp.hpp"

#include "RoleDBDeleteRoleHandler.hpp"

using namespace ServerLib;

//todo jasonxiong2 目前没有删号需求，等有再来开发

char CRoleDBDeleteRoleHandler::m_szQueryString[GameConfig::ROLE_TABLE_SPLIT_FACTOR][512];

CRoleDBDeleteRoleHandler::CRoleDBDeleteRoleHandler(DBClientWrapper* pDatabase)
{
    m_pDatabase = pDatabase;
}

void CRoleDBDeleteRoleHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg,
        SHandleResult* pstHandleResult)
{
    if (!pstRequestMsg || !pstHandleResult)
	{
		return;
	}

    // delete role请求消息
    m_pstRequestMsg = pstRequestMsg;
    const DeleteRole_Account_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_staccountdeleterolerequest();
    
    unsigned int uiUin = rstReq.stroleid().uin();

    TRACE_THREAD(m_iThreadIdx, "Handling DeleteRoleRequest, uin: %u\n", uiUin);

    // delete role响应消息
    GameProtocolMsg* pstMsgResp = &(pstHandleResult->stResponseMsg);

    // 响应消息头
    GenerateResponseMsgHead(pstMsgResp, 0, MSGID_ACCOUNT_DELETEROLE_RESPONSE, uiUin);

    // 根据uin和worldid读取数据库中的nickname，存入m_szNickname中
    int iRes = QueryRoleInfo(rstReq.stroleid());
    if (iRes != 0)
    {
        // 没有找到要删除的角色, 直接返回成功
        FillSuccessfulResponse(pstMsgResp);
        return;
    }

    // 根据RoleID删除表DBRoleInfo中的相应记录
    iRes = DeleteRole(rstReq.stroleid());
    if (iRes != 0)
    {
        FillFailedResponse(T_ROLEDB_SQL_EXECUTE_FAILED, pstMsgResp);
        return;
    }

    FillSuccessfulResponse(pstMsgResp);
}

int CRoleDBDeleteRoleHandler::DeleteRole(const RoleID& stRoleID)
{
    //拉取ROLEDB数据库相关配置
    const ONEROLEDBINFO* pstDBConfig = (CRoleDBApp::m_stRoleDBConfigManager).GetOneRoleDBInfoByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get roledb config, thread index %d\n", m_iThreadIdx);
        return -1;
    }

    //设置要操作的数据库相关信息
    m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);

    char* pszQueryString = m_szQueryString[m_iThreadIdx];
    int iQueryBuffLen = sizeof(m_szQueryString[m_iThreadIdx])-1;
    int iLength = SAFE_SPRINTF(pszQueryString, iQueryBuffLen, "delete from %s where uin = %u and seq=%u\n", MYSQL_USERINFO_TABLE, stRoleID.uin(), stRoleID.uiseq());

    int iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute delete sql query, uin %u, ret %d\n", stRoleID.uin(), iRet);
        return iRet;
    }

    DEBUG_THREAD(m_iThreadIdx, "The number of affected rows is %d\n", m_pDatabase->GetAffectedRows());
    return 0;
}

int CRoleDBDeleteRoleHandler::QueryRoleInfo(const RoleID& stRoleID)
{
    //从XML文件中读取数据库配置信息
    const ONEROLEDBINFO* pstDBInfoConfig = (CRoleDBApp::m_stRoleDBConfigManager).GetOneRoleDBInfoByIndex(m_iThreadIdx);
    if(!pstDBInfoConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to query role info, invalid DB cfg, thread idx %d\n", m_iThreadIdx);
        return -1;
    }

    //设置要操作的数据库相关信息
    m_pDatabase->SetMysqlDBInfo(pstDBInfoConfig->szDBHost, pstDBInfoConfig->szUserName, pstDBInfoConfig->szUserPasswd, pstDBInfoConfig->szDBName);

    //初始化查询的SQL语句
    char* pszQueryString = m_szQueryString[m_iThreadIdx];
    int iQueryBuffLen = sizeof(m_szQueryString[m_iThreadIdx])-1;
    int iLength = SAFE_SPRINTF(pszQueryString, iQueryBuffLen, "select uin from %s where uin=%u and seq=%u\n", MYSQL_USERINFO_TABLE, stRoleID.uin(), stRoleID.uiseq());

    int iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, true);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Fail to execute sql query, uin %u, ret %d\n", stRoleID.uin(), iRet);
        return T_ROLEDB_SQL_EXECUTE_FAILED;
    }

    //判断是否已经存在要删除的记录
    int iNum = m_pDatabase->GetNumberRows();
    if(iNum != 1)
    {
        TRACE_THREAD(m_iThreadIdx, "Wrong select result, uin %u\n", stRoleID.uin());
        return T_ROLEDB_INVALID_RECORD;
    }

    return 0;
}

void CRoleDBDeleteRoleHandler::FillFailedResponse(const unsigned int uiResultID,
        GameProtocolMsg* pstResponseMsg)
{
	if (!pstResponseMsg)
	{
		return;
	}
    
    DeleteRole_Account_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staccountdeleteroleresponse();
    pstResp->set_iresult(uiResultID);

    TRACE_THREAD(m_iThreadIdx, "Info of failed DeleteRoleResponse: result: %d\n",
             uiResultID);
}

void CRoleDBDeleteRoleHandler::FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg)
{
	if (!pstResponseMsg)
	{
		return;
	}

    DeleteRole_Account_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staccountdeleteroleresponse();
    pstResp->set_iresult(T_SERVER_SUCESS);

    TRACE_THREAD(m_iThreadIdx, "Info of DeleteRoleResponse: result: %d, uin: %u\n", T_SERVER_SUCESS, pstResponseMsg->m_stmsghead().m_uin());

    return;
}
