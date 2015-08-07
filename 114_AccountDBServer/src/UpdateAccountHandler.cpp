#include <string.h>

#include "AccountDBLogManager.hpp"
#include "StringUtility.hpp"
#include "AccountDBApp.hpp"
#include "AppDef.hpp"

#include "UpdateAccountHandler.hpp"

using namespace ServerLib;

//生成的SQL语句
char CUpdateAccountHandler::m_szQueryString[GameConfig::ACCOUNT_TABLE_SPLIT_FACTOR][1024];

CUpdateAccountHandler::CUpdateAccountHandler(DBClientWrapper* pDatabase)
{ 
    m_pDatabase = pDatabase;
}

void CUpdateAccountHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult)
{
    if (!pstRequestMsg || !pstHandleResult)
    {
    	return;
    }

    // update account请求消息
    m_pstRequestMsg = pstRequestMsg;

    switch (pstRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_ACCOUNTDB_UPDATE_REQUEST:
        {
            OnUpdateAccountRequest(pstHandleResult);
            break;
        }

    default:
        {
            break;
        }
    }
}

void CUpdateAccountHandler::OnUpdateAccountRequest(SHandleResult* pstHandleResult)
{
    const AccountDB_UpdateAccount_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_staccountdb_update_request();

    TRACE_THREAD(m_iThreadIdx, "Handling UpdateAccountRequest, account: %s, type %d\n", rstReq.staccountid().straccount().c_str(), rstReq.staccountid().iaccounttype());

    pstHandleResult->iNeedResponse = true;  //需要回复

    // 响应消息头
    GenerateResponseMsgHead(&(pstHandleResult->stResponseMsg), m_pstRequestMsg->m_stmsghead().m_uisessionfd(), MSGID_ACCOUNTDB_UPDATE_RESPONSE, 0);

    //首先检查帐号密码的有效性
    int iRet = UpdateSecurityCheck(rstReq.staccountid(), rstReq.strpassword());
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to update account %s, security check failed!\n", rstReq.staccountid().straccount().c_str());
        FillFailedResponse(iRet, &(pstHandleResult->stResponseMsg));

        return;
    }

    iRet = UpdateAccountInfo(rstReq.staccountid());
    if (iRet != 0)
    {
    	TRACE_THREAD(m_iThreadIdx, "Failed to execute db query\n");
    	FillFailedResponse(T_ACCOUNTDB_SQL_EXECUTE_FAILED, &(pstHandleResult->stResponseMsg));

        return;
    }

    FillSuccessfulResponse(&(pstHandleResult->stResponseMsg));

    return;
}

//检查帐号密码的有效性
int CUpdateAccountHandler::UpdateSecurityCheck(const AccountID& stAccountID, const std::string& strPassword)
{
    //获取到数据库的连接配置
    const ONEACCOUNTDBINFO* pstDBConfig = (CAccountDBApp::m_stAccountDBConfigManager).GetOneAccountDBInfoByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get accountDB config, invalid thread index %d\n", m_iThreadIdx);
        return -2;
    }

    //设置要操作的数据库相关信息
    int iRet = m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to set mysql db info, thread index %d\n", m_iThreadIdx);
        return iRet;
    }

    //初始化查询的SQL语句
    char* pszQueryString = m_szQueryString[m_iThreadIdx];
    int iLength = SAFE_SPRINTF(pszQueryString, sizeof(m_szQueryString[m_iThreadIdx])-1, "select password from %s where accountID='%s' and accountType=%d",
                  MYSQL_ACCOUNTINFO_TABLE, stAccountID.straccount().c_str(), stAccountID.iaccounttype());

    //执行查询
    iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, true);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute sql query, thread %d, account %s\n", m_iThreadIdx, stAccountID.straccount().c_str());
        return T_ACCOUNTDB_SQL_EXECUTE_FAILED;
    }

    int iNumRows = m_pDatabase->GetNumberRows();
    if(iNumRows != 1)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to update account %s, invalid record num %d\n", stAccountID.straccount().c_str(), iNumRows);
        return T_ACCOUNTDB_INVALID_RECORD;
    }

    //校验密码是否一致
    MYSQL_ROW pstResult = NULL;
    unsigned long* pLengths = NULL;
    unsigned int uFields = 0;
    iRet = m_pDatabase->FetchOneRow(pstResult, pLengths, uFields);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to fetch one row, ret %d\n", iRet);
        return T_ACCOUNTDB_INVALID_RECORD;
    }

    std::string strDBPassword(pstResult[0], pLengths[0]);

    if(strDBPassword.compare(strPassword) != 0)
    {
        //密码不正确，返回错误
        TRACE_THREAD(m_iThreadIdx, "Failed to update account %s, password not match!\n", stAccountID.straccount().c_str());
        return T_ACCOUNTDB_PASSWD_NOT_MATCH;
    }

    return T_SERVER_SUCESS;
}

int CUpdateAccountHandler::UpdateAccountInfo(const AccountID& stAccountID)
{
    //获取连接的数据库相关的配置
    const ONEACCOUNTDBINFO* pstDBConfig = (CAccountDBApp::m_stAccountDBConfigManager).GetOneAccountDBInfoByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get accountDB config, invalid thread index %d\n", m_iThreadIdx);
        return -2;
    }

    //设置要操作的数据库相关信息
    m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);

    //初始化SQL语句
    char* pszQueryString = m_szQueryString[m_iThreadIdx];
    int iLength = SAFE_SPRINTF(pszQueryString, sizeof(m_szQueryString[m_iThreadIdx])-1, 
                 "update %s set lastWorldID=%d where accountID='%s' and accountType=%d",
                 MYSQL_ACCOUNTINFO_TABLE, 1, stAccountID.straccount().c_str(), stAccountID.iaccounttype());

    //执行
    int iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Fail to execute select sql query, account %s\n", stAccountID.straccount().c_str());
        return iRet;
    }

    DEBUG_THREAD(m_iThreadIdx, "The number of affected rows is %d\n", m_pDatabase->GetAffectedRows());

    return 0;
}

void CUpdateAccountHandler::FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg)
{
    AccountDB_UpdateAccount_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staccountdb_update_response();
    pstResp->set_iresult(uiResultID);

    TRACE_THREAD(m_iThreadIdx, "Info of failed UpdateAccountResponse: result: %d\n", uiResultID);

    return;
}

void CUpdateAccountHandler::FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg)
{
    AccountDB_UpdateAccount_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staccountdb_update_response();
    pstResp->set_iresult(T_SERVER_SUCESS);

    TRACE_THREAD(m_iThreadIdx, "Info of success UpdateAccountResponse: result: %d\n", T_SERVER_SUCESS);

    return;
}

