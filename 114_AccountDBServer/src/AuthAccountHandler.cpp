#include <stdlib.h>
#include <string.h>

#include "AppDef.hpp"
#include "AccountDBLogManager.hpp"
#include "UnixTime.hpp"
#include "NowTime.hpp"
#include "StringUtility.hpp"
#include "AccountDBApp.hpp"

#include "AuthAccountHandler.hpp"

using namespace ServerLib;

CAuthAccountHandler::CAuthAccountHandler(DBClientWrapper* pDatabase)
{
    m_pDatabase = pDatabase;
}

void CAuthAccountHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg,
        SHandleResult* pstHandleResult)
{
    if (!pstRequestMsg || !pstHandleResult)
	{
		return;
	}

    // 认证平台帐号的请求
    m_pstRequestMsg = pstRequestMsg;

    switch (m_pstRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_REGAUTH_AUTHACCOUNT_REQUEST:
        {
            OnAuthAccountRequest(pstHandleResult);
            break;
        }

    default:
        {
            break;
        }
    }

    return;
}

void CAuthAccountHandler::OnAuthAccountRequest(SHandleResult* pstHandleResult)
{
	if (!pstHandleResult)
	{
		return;
	}

    RegAuth_AuthAccount_Request* pstReq = m_pstRequestMsg->mutable_m_stmsgbody()->mutable_m_stregauth_authaccount_request();

    const std::string& strAccount = pstReq->staccountid().straccount();
    const std::string& strPassword = pstReq->strpassword();
    int iLoginType = pstReq->staccountid().iaccounttype();

    TRACE_THREAD(m_iThreadIdx, "Handling AuthAccountRequest, account: %s\n", strAccount.c_str());

    // 认账平台帐号的响应消息
    GameProtocolMsg* pstMsgResp = &(pstHandleResult->stResponseMsg);

    // 响应消息头
    GenerateResponseMsgHead(pstMsgResp, m_pstRequestMsg->m_stmsghead().m_uisessionfd(), MSGID_REGAUTH_AUTHACCOUNT_RESPONSE, 0);

    // 响应消息体
    RegAuth_AuthAccount_Response* pstResp = pstMsgResp->mutable_m_stmsgbody()->mutable_m_stregauth_authaccount_response();

    // 根据AccountID查找玩家帐号信息
    int iRet = CheckAccountPasswd(strAccount, iLoginType, strPassword, *pstResp);
    if (iRet != 0)
    {
		TRACE_THREAD(m_iThreadIdx, "Query Role info failed, account:%s, iRes:%d\n", strAccount.c_str(), iRet);
        FillFailedResponse(iRet, pstMsgResp);
        return;
    }

    // 填充成功回复
    FillSuccessfulResponse(pstMsgResp);
}

//查询玩家帐号详细信息
int CAuthAccountHandler::CheckAccountPasswd(const std::string& strAccount, int iLoginType, const std::string& strPasswd, RegAuth_AuthAccount_Response& stResp)
{
    //读取ACCOUNTDB数据库的配置
    const ONEACCOUNTDBINFO* pstDBConfig = (CAccountDBApp::m_stAccountDBConfigManager).GetOneAccountDBInfoByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get roledb config, thread index %d\n", m_iThreadIdx);
        return T_ACCOUNTDB_INVALID_CONFIG;
    }

    //设置要操作的数据库相关信息
    int iRet = m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to set mysql DB info, db name %s\n", pstDBConfig->szDBName);
        return T_ACCOUNTDB_SQL_EXECUTE_FAILED;
    }

    //构造SQL语句
    static char szQueryString[256];
    int iLength = SAFE_SPRINTF(szQueryString, sizeof(szQueryString)-1, "select uin,password from %s where accountID = \"%s\" and accountType=%d", 
                 MYSQL_ACCOUNTINFO_TABLE, strAccount.c_str(), iLoginType);

    //执行
    iRet = m_pDatabase->ExecuteRealQuery(szQueryString, iLength, true);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Fail to execute select sql query, account: %s\n", strAccount.c_str());
        return T_ACCOUNTDB_SQL_EXECUTE_FAILED;
    }

    //分析结果
    int iRowNum = m_pDatabase->GetNumberRows();
    if(iRowNum != 1)
    {
        TRACE_THREAD(m_iThreadIdx, "Wrong result, invalid rows %d, account %s\n", iRowNum, strAccount.c_str());
        return T_ACCOUNTDB_INVALID_RECORD;
    }

    MYSQL_ROW pstResult = NULL;
    unsigned long* pLengths = NULL;
    unsigned int uFields = 0;

    iRet = m_pDatabase->FetchOneRow(pstResult, pLengths, uFields);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to fetch rows, uin %s, ret %d\n", strAccount.c_str(), iRet);
        return T_ACCOUNTDB_SQL_EXECUTE_FAILED;
    }

    //判断uFields是否相符
    if(uFields != 2)
    {
        TRACE_THREAD(m_iThreadIdx, "Wrong result, real fields %u, needed %u\n", uFields, 2);
        return T_ACCOUNTDB_INVALID_RECORD;
    }

    //先验证帐号密码
    if(SAFE_STRCMP(strPasswd.c_str(), pstResult[1], pLengths[1]) != 0)
    {
        //密码校验不通过，返回错误
        TRACE_THREAD(m_iThreadIdx, "Failed to check password, account: %s, type: %d\n", strAccount.c_str(), iLoginType);
        return T_ACCOUNTDB_AUTH_FAILED;
    }

    //从结果中解析需要的字段填充resp

    //字段1是uin
    stResp.set_uin(strtoul(pstResult[0], NULL, 10));

    return 0;
}

void CAuthAccountHandler::FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg)
{
	if (!pstResponseMsg)
	{
		return;
	}

    const std::string& strAccount = m_pstRequestMsg->m_stmsgbody().m_stregauth_authaccount_request().staccountid().straccount();

    RegAuth_AuthAccount_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stregauth_authaccount_response();
    pstResp->set_iresult(uiResultID);

    TRACE_THREAD(m_iThreadIdx, "Info of failed FetchRoleResponse: result: %u, account: %s\n", uiResultID, strAccount.c_str());

    return;
}

void CAuthAccountHandler::FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg)
{
    if(!pstResponseMsg)
    {
        return;
    }

    RegAuth_AuthAccount_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stregauth_authaccount_response();
    pstResp->set_iresult(T_SERVER_SUCESS);

    return;
}


