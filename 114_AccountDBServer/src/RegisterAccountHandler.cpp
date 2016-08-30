#include <string.h>
#include "AccountDBLogManager.hpp"
#include "NowTime.hpp"
#include "UnixTime.hpp"
#include "StringUtility.hpp"
#include "AccountDBApp.hpp"
#include "AppDef.hpp"
#include "AccountDBApp.hpp"

#include "RegisterAccountHandler.hpp"

using namespace ServerLib;

CRegisterAccountHandler::CRegisterAccountHandler(DBClientWrapper* pDatabase)
{
    m_pDatabase = pDatabase;
}

void CRegisterAccountHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult)
{
    if(!pstRequestMsg || !pstHandleResult)
    {
        return;
    }

    //注册帐号的请求
    m_pstRequestMsg = pstRequestMsg;
    const RegAuth_RegAccount_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_stregauth_regaccount_request();

    //注册帐号的响应
    GameProtocolMsg* pstMsgResp = &(pstHandleResult->stResponseMsg);

    //生成响应消息头
    GenerateResponseMsgHead(pstMsgResp, m_pstRequestMsg->m_stmsghead().m_uisessionfd(), 
                            MSGID_REGAUTH_REGACCOUNT_RESPONSE, m_pstRequestMsg->m_stmsghead().m_uin());

    //首先查找相同的帐号是否已经注册
    bool bIsAccountExist = false;
    int iRet = CheckAccountIsExist(bIsAccountExist);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to check whether account is exist, ret %d\n", iRet);
        FillFailedResponse(T_ACCOUNTDB_SQL_EXECUTE_FAILED, pstMsgResp);
        return;
    }

    if(bIsAccountExist)
    {
        //要注册的帐号已经存在
        FillFailedResponse(T_ACCOUNTDB_ACCOUNT_EXISTS, pstMsgResp);
        return;
    }

    //从Uin数据库中获取当前可用的Uin
    unsigned int uiUin = 0;
    iRet = GetAccountUin(uiUin);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get account uin from uin DB, ret %d\n", iRet);
        FillFailedResponse(T_ACCOUNTDB_SQL_EXECUTE_FAILED, pstMsgResp);
        return;
    }

    //将新玩家的帐号数据插入到AccountDB中
    iRet = InsertNewAccountRecord(rstReq, uiUin);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to insert new account record, ret %d\n", iRet);
        FillFailedResponse(T_ACCOUNTDB_SQL_EXECUTE_FAILED, pstMsgResp);
        return;
    }

    FillSuccessfulResponse(uiUin, pstMsgResp);

    TRACE_THREAD(m_iThreadIdx, "Success to RegAccount: account: %s, uin: %u\n", rstReq.staccountid().straccount().c_str(), uiUin);

    return;
}

//先检查相同的帐号是否已经存在
int CRegisterAccountHandler::CheckAccountIsExist(bool& bIsAccountExist)
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

    const RegAuth_RegAccount_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_stregauth_regaccount_request();

    static char szQueryString[256] = {0};
    int iLength = SAFE_SPRINTF(szQueryString, sizeof(szQueryString)-1, "select uin from %s where accountID=\"%s\"", 
                 MYSQL_ACCOUNTINFO_TABLE, rstReq.staccountid().straccount().c_str());

    iRet = m_pDatabase->ExecuteRealQuery(szQueryString, iLength, true);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute sql query, ret %d\n", iRet);
        return iRet;
    }

    if(m_pDatabase->GetNumberRows() != 0)
    {
        //已经存在该帐号
        bIsAccountExist = true;
    }
    else
    {
        bIsAccountExist = false;
    }

    return T_SERVER_SUCESS;
}

//获取当前可用可注册的Uin
int CRegisterAccountHandler::GetAccountUin(unsigned& uiUin)
{
    //这个数据库的配置中只有一项
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

    //获取最新的UIN
    static char szQueryString[256] = {0};
    int iLength = SAFE_SPRINTF(szQueryString, sizeof(szQueryString)-1, "insert into %s set uin=NULL", MYSQL_UNIQUININFO_TABLE);

    iRet = m_pDatabase->ExecuteRealQuery(szQueryString, iLength, false);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute sql query, ret %d\n", iRet);
        return iRet;
    }

    uiUin = m_pDatabase->GetLastInsertID();
    if(uiUin == 0)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get uniq uin, should not be zero!\n");
        return -2;
    }

    TRACE_THREAD(m_iThreadIdx, "Success to get new register uin: %u\n", uiUin);

    return T_SERVER_SUCESS;
}

//向数据库中插入一条新注册帐号的记录
int CRegisterAccountHandler::InsertNewAccountRecord(const RegAuth_RegAccount_Request& rstReq, unsigned int uiUin)
{
    //设置连接的DB信息
    const ONEACCOUNTDBINFO* pstDBConfig = (CAccountDBApp::m_stAccountDBConfigManager).GetOneAccountDBInfoByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get account db config, db index %d\n", m_iThreadIdx);
        return T_ACCOUNTDB_INVALID_CONFIG;
    }

    int iRet = m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to set mysql db info, db name %s\n", pstDBConfig->szDBName);
        return T_ACCOUNTDB_SQL_EXECUTE_FAILED;
    }

    //插入新的记录
    static char szQueryString[512] = {0};
    int iLength = SAFE_SPRINTF(szQueryString, sizeof(szQueryString)-1, "insert into %s(accountID,uin,accountType,password,lastWorldID,activeState) values(\"%s\",%u,%u,\"%s\",%d,%d)", \
                 MYSQL_ACCOUNTINFO_TABLE, rstReq.staccountid().straccount().c_str(), uiUin, rstReq.staccountid().iaccounttype(), rstReq.strpassword().c_str(), 1, 0);

    iRet = m_pDatabase->ExecuteRealQuery(szQueryString, iLength, false);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute sql query, ret %d\n", iRet);
        return iRet;
    }

    TRACE_THREAD(m_iThreadIdx, "Success to regsiter account, account: %s, uin: %u\n", rstReq.staccountid().straccount().c_str(), uiUin);

    return T_SERVER_SUCESS;
}

void CRegisterAccountHandler::FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg)
{
    if(!pstResponseMsg)
    {
        return;
    }

    RegAuth_RegAccount_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stregauth_regaccount_response();
    pstResp->set_iresult(uiResultID);

    return;
}

void CRegisterAccountHandler::FillSuccessfulResponse(unsigned int uiUin, GameProtocolMsg* pstResponseMsg)
{
    if(!pstResponseMsg)
    {
        return;
    }

    RegAuth_RegAccount_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stregauth_regaccount_response();
    pstResp->set_iresult(T_SERVER_SUCESS);

    return;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
