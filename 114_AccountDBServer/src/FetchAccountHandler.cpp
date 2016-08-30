#include <string.h>
#include "AccountDBLogManager.hpp"
#include "NowTime.hpp"
#include "UnixTime.hpp"
#include "StringUtility.hpp"
#include "AccountDBApp.hpp"
#include "AppDef.hpp"
#include "AccountDBApp.hpp"

#include "FetchAccountHandler.hpp"

using namespace ServerLib;

//生成的SQL语句
char CFetchAccountHandler::m_szQueryString[GameConfig::ACCOUNT_TABLE_SPLIT_FACTOR][512];

CFetchAccountHandler::CFetchAccountHandler(DBClientWrapper* pDatabase)
{
    m_pDatabase = pDatabase;
}

void CFetchAccountHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult)
{
    if(!pstRequestMsg || !pstHandleResult)
    {
        return;
    }

    pstHandleResult->iNeedResponse = true;  //需要回复

    //拉取帐号详细信息的请求
    m_pstRequestMsg = pstRequestMsg;
    const AccountDB_FetchAccount_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_staccountdb_fetch_request();

    //拉取帐号详细信息的响应
    AccountDB_FetchAccount_Response* pstResp = pstHandleResult->stResponseMsg.mutable_m_stmsgbody()->mutable_m_staccountdb_fetch_response();

    //生成响应消息头
    GenerateResponseMsgHead(&pstHandleResult->stResponseMsg, m_pstRequestMsg->m_stmsghead().m_uisessionfd(), MSGID_ACCOUNTDB_FETCH_RESPONSE, 0);

    const AccountID& stAccountID = rstReq.staccountid();
    int iRet = FetchAccountInfo(stAccountID, rstReq.strpassword(), *pstResp);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to fetch account info, account:%s, type:%d, ret %d\n", stAccountID.straccount().c_str(), stAccountID.iaccounttype(), iRet);
        FillFailedResponse(iRet, pstHandleResult->stResponseMsg);

        return;
    }

    FillSuccessfulResponse(pstHandleResult->stResponseMsg);

    TRACE_THREAD(m_iThreadIdx, "Success to fetch account:%s, uin: %u\n", stAccountID.straccount().c_str(), pstResp->uin());

    return;
}

//拉取返回帐号详细信息
int CFetchAccountHandler::FetchAccountInfo(const AccountID& stAccountID, const std::string& strPassword, AccountDB_FetchAccount_Response& rstResp)
{
    //设置连接的DB
    const ONEACCOUNTDBINFO* pstDBConfig = (CAccountDBApp::m_stAccountDBConfigManager).GetOneAccountDBInfoByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get account db config, index %d\n", m_iThreadIdx);
        return -1;
    }

    int iRet = m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to set mysql db info, ret %d\n", iRet);
        return iRet;
    }

    char* pszQueryString = m_szQueryString[m_iThreadIdx];
    int iLength = SAFE_SPRINTF(pszQueryString, sizeof(m_szQueryString[m_iThreadIdx])-1, "select * from %s where accountID='%s' and accountType=%d", 
                 MYSQL_ACCOUNTINFO_TABLE, stAccountID.straccount().c_str(), stAccountID.iaccounttype());

    iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, true);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute sql query, ret %d\n", iRet);
        return iRet;
    }

     //分析结果
    int iRowNum = m_pDatabase->GetNumberRows();
    if(iRowNum != 1)
    {
        TRACE_THREAD(m_iThreadIdx, "Wrong result, invalid rows %d, account %s\n", iRowNum, stAccountID.straccount().c_str());
        return T_ACCOUNTDB_INVALID_RECORD;
    }

    MYSQL_ROW pstResult = NULL;
    unsigned long* pLengths = NULL;
    unsigned int uFields = 0;

    iRet = m_pDatabase->FetchOneRow(pstResult, pLengths, uFields);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to fetch rows, account %s, ret %d\n", stAccountID.straccount().c_str(), iRet);
        return iRet;
    }

    //判断uFields是否相符
    if(uFields != MYSQL_ACCOUNTINFO_FIELDS)
    {
        TRACE_THREAD(m_iThreadIdx, "Wrong result, real fields %u, needed %u\n", uFields, MYSQL_ACCOUNTINFO_FIELDS);
        return T_ACCOUNTDB_INVALID_RECORD;
    }

    //字段3是password
    std::string strDBPassword(pstResult[3], pLengths[3]);

    //检查密码是否一致
    if(strDBPassword.compare(strPassword) != 0)
    {
        return T_ACCOUNTDB_INVALID_RECORD;
    }

    //从结果中解析需要的字段
    rstResp.mutable_staccountid()->CopyFrom(stAccountID);

    //字段0是accountID, 字段1是accountType, 跳过

    //字段2是uin
    rstResp.set_uin(strtoul(pstResult[2],NULL,10));

    //字段4是lastWorldID
    rstResp.set_iworldid(atoi(pstResult[4]));

    //字段5是activeState
    rstResp.set_bisbinded(atoi(pstResult[5]));

    return T_SERVER_SUCESS;
}

void CFetchAccountHandler::FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg& stResponseMsg)
{
    AccountDB_FetchAccount_Response* pstResp = stResponseMsg.mutable_m_stmsgbody()->mutable_m_staccountdb_fetch_response();
    pstResp->Clear();

    pstResp->set_iresult(uiResultID);
    pstResp->mutable_staccountid()->CopyFrom(m_pstRequestMsg->m_stmsgbody().m_stregauth_regaccount_request().staccountid());

    return;
}

void CFetchAccountHandler::FillSuccessfulResponse(GameProtocolMsg& stResponseMsg)
{
    AccountDB_FetchAccount_Response* pstResp = stResponseMsg.mutable_m_stmsgbody()->mutable_m_staccountdb_fetch_response();
    pstResp->set_iresult(T_SERVER_SUCESS);

    return;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
