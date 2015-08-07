#include <stdlib.h>
#include <string.h>

#include "AppDef.hpp"
#include "AccountDBLogManager.hpp"
#include "UnixTime.hpp"
#include "NowTime.hpp"
#include "StringUtility.hpp"
#include "AccountDBApp.hpp"

#include "AddAccountHandler.hpp"

using namespace ServerLib;

//生成的SQL语句
char CAddAccountHandler::m_szQueryString[GameConfig::ACCOUNT_TABLE_SPLIT_FACTOR][1024];

CAddAccountHandler::CAddAccountHandler(DBClientWrapper* pDatabase)
{
    m_pDatabase = pDatabase;
}

void CAddAccountHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg,
        SHandleResult* pstHandleResult)
{
    if (!pstRequestMsg || !pstHandleResult)
	{
		return;
	}

    // 增加新帐号记录的请求
    m_pstRequestMsg = pstRequestMsg;

    switch (m_pstRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_ACCOUNTDB_ADDACCOUNT_REQUEST:
        {
            OnAddAccountRequest(pstHandleResult);
            break;
        }

    default:
        {
            break;
        }
    }

    return;
}

//进行必要的参数检查
int CAddAccountHandler::CheckParams()
{
    //检查请求的参数
    const AccountDB_AddAccount_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_staccountdb_addaccount_request();

    if(rstReq.staccountid().straccount().size() == 0)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to add new account, invalid param!\n");
        return T_COMMON_SYSTEM_PARA_ERR;
    }

    return T_SERVER_SUCESS;
}

void CAddAccountHandler::OnAddAccountRequest(SHandleResult* pstHandleResult)
{
	if (!pstHandleResult)
	{
		return;
	}

    pstHandleResult->iNeedResponse = true;  //需要回复

    int iRet = CheckParams();
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to add account request, param check failed, ret %d\n", iRet);
        FillFailedResponse(iRet, &pstHandleResult->stResponseMsg);

        return;
    }
    
    //请求的消息
    const AccountDB_AddAccount_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_staccountdb_addaccount_request();

    //生成响应的消息头
    const GameCSMsgHead& rstHead = m_pstRequestMsg->m_stmsghead();
    GenerateResponseMsgHead(&pstHandleResult->stResponseMsg, rstHead.m_uisessionfd(), MSGID_ACCOUNTDB_ADDACCOUNT_RESPONSE, 0);

    //检查帐号是否存在
    bool bIsExist = false;
    iRet = CheckAccountExist(rstReq.staccountid(), bIsExist);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to check game account exist, ret 0x%0x\n", iRet);
        FillFailedResponse(iRet, &pstHandleResult->stResponseMsg);

        return;
    }

    if(bIsExist)
    {
        //帐号已经存在
        TRACE_THREAD(m_iThreadIdx, "Failed to add new account, already exist, ret 0x%0x\n", T_ACCOUNTDB_ACCOUNT_EXISTS);
        FillFailedResponse(T_ACCOUNTDB_ACCOUNT_EXISTS, &pstHandleResult->stResponseMsg);

        return;
    }

    //如果不是绑定帐号的请求，则生成唯一uin
    unsigned int uin = rstReq.uin();
    iRet = GetAvaliableUin(uin);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get avaliable uin, ret %d\n", iRet);
        FillFailedResponse(iRet, &pstHandleResult->stResponseMsg);

        return;
    }

    //插入新的记录
    iRet = AddNewRecord(rstReq.staccountid(), uin, rstReq.iworldid(), rstReq.strpassword());
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to add new record, account %s, ret %d\n", rstReq.staccountid().straccount().c_str(), iRet);
        FillFailedResponse(iRet, &pstHandleResult->stResponseMsg);

        return;
    }

    FillSuccessfulResponse(pstHandleResult->stResponseMsg);

    return;
}

//检查帐号是否存在
int CAddAccountHandler::CheckAccountExist(const AccountID& stAccountID, bool& bIsExist)
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
    int iLength = SAFE_SPRINTF(pszQueryString, sizeof(m_szQueryString[m_iThreadIdx])-1, "select uin from %s where accountID='%s' and accountType=%d", 
                 MYSQL_ACCOUNTINFO_TABLE, stAccountID.straccount().c_str(), stAccountID.iaccounttype());

    iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, true);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute sql query, ret %d\n", iRet);
        return iRet;
    }

    if(m_pDatabase->GetNumberRows() != 0)
    {
        //已经存在该帐号
        bIsExist = true;
    }
    else
    {
        bIsExist = false;
    }

    return T_SERVER_SUCESS;
}

//拉取可用的帐号UIN
int CAddAccountHandler::GetAvaliableUin(unsigned int& uin)
{
    //拉取帐号数据库的配置
    const ONEACCOUNTDBINFO* pstDBConfig = (CAccountDBApp::m_stUniqUinDBConfigManager).GetOneAccountDBInfoByIndex(0);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get UniqUinDB config!\n");
        return -1;
    }

    int iRet = m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to set mysql db info, db name %s\n", pstDBConfig->szDBName);
        return iRet;
    }

    //生成SQL语句
    char* pszQueryString = m_szQueryString[m_iThreadIdx];
    int iLength = SAFE_SPRINTF(pszQueryString, sizeof(m_szQueryString[m_iThreadIdx])-1, "insert into %s set uin=NULL", MYSQL_UNIQUININFO_TABLE);

    iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute sql query, ret %d\n", iRet);
        return iRet;
    }

    uin = m_pDatabase->GetLastInsertID();
    if(uin == 0)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get uniq uin, should not be zero!\n");
        return -2;
    }

    TRACE_THREAD(m_iThreadIdx, "Success to get new register uin: %u\n", uin);

    return T_SERVER_SUCESS;
}

//插入新的帐号记录
int CAddAccountHandler::AddNewRecord(const AccountID& stAccountID, unsigned int uin, int iWorldID, const std::string& strPassword)
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
    int iLength = SAFE_SPRINTF(pszQueryString, sizeof(m_szQueryString[m_iThreadIdx])-1, \
                 "insert into %s(accountID,accountType,uin,password,lastWorldID,isBinded) "
                 "values('%s',%d,%u,'%s',%d,%d)", 
                 MYSQL_ACCOUNTINFO_TABLE, 
                 stAccountID.straccount().c_str(), stAccountID.iaccounttype(), uin, strPassword.c_str(), iWorldID, false);

    iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute sql query, ret %d\n", iRet);
        return iRet;
    }

    TRACE_THREAD(m_iThreadIdx, "Success to add new account: %s, uin %u\n", stAccountID.straccount().c_str(), uin);

    return T_SERVER_SUCESS;
}

void CAddAccountHandler::FillFailedResponse(const unsigned int uiResultID,
        GameProtocolMsg* pstResponseMsg)
{
	if (!pstResponseMsg)
	{
		return;
	}

    const std::string& strAccount = m_pstRequestMsg->m_stmsgbody().m_staccountdb_addaccount_request().staccountid().straccount();

    AccountDB_AddAccount_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staccountdb_addaccount_response();
    pstResp->set_iresult(uiResultID);

    TRACE_THREAD(m_iThreadIdx, "Info of failed FetchRoleResponse: result: %u, account: %s\n", uiResultID, strAccount.c_str());

    return;
}

void CAddAccountHandler::FillSuccessfulResponse(GameProtocolMsg& stResponseMsg)
{
    AccountDB_AddAccount_Response* pstResp = stResponseMsg.mutable_m_stmsgbody()->mutable_m_staccountdb_addaccount_response();
    pstResp->set_iresult(T_SERVER_SUCESS);

    return;
}


