#include <assert.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "SessionObj.hpp"
#include "RegAuthSingleton.hpp"
#include "TimeStampConverter.hpp"
#include "StringUtility.hpp"
#include "ConfigMgr.hpp"
#include "RegAuthApp.hpp"
#include "PasswordEncryptionUtility.hpp"
#include "PlatformDefine.hpp"
#include "SessionKeyUtility.hpp"
#include "RegAuthBillLog.hpp"

using namespace ServerLib;

CAccountDBAddHandler::CAccountDBAddHandler()
{
}

void CAccountDBAddHandler::OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg, SHandleResult* pstResult)
{
    // 不使用Result
    ASSERT_AND_LOG_RTN_VOID(pstNetHead);
    ASSERT_AND_LOG_RTN_VOID(pstMsg);

    m_pstNetHead = pstNetHead;
    m_pstRequestMsg = pstMsg;

    switch (m_pstRequestMsg->m_stmsghead().m_uimsgid())
    {
        case MSGID_ACCOUNTDB_ADDACCOUNT_RESPONSE:
            {
                //增加新帐号的返回
                OnResponseAccountDBAdd();
                return;
            }
            break;

        default:
            {

            }
            break;
    }

    return;
}

int CAccountDBAddHandler::OnResponseAccountDBAdd()
{
    //AccountDB增加帐号的返回
    const AccountDB_AddAccount_Response& rstResp = m_pstRequestMsg->m_stmsgbody().m_staccountdb_addaccount_response();

    SendResponseToLotus(m_pstRequestMsg->m_stmsghead().m_uisessionfd(), rstResp.iresult());

    return T_SERVER_SUCESS;
}

//新增加帐号
void CAccountDBAddHandler::AddAccount(unsigned uiSessionFd, const AccountID& stAccountID, int iWorldID, const std::string& strPasswd)
{
    static GameProtocolMsg stMsg;

    //生成消息头
    GenerateMsgHead(&stMsg, uiSessionFd, MSGID_ACCOUNTDB_ADDACCOUNT_REQUEST, GetAccountHash(stAccountID.straccount()));

    //AccountDB插入新帐号的请求
    AccountDB_AddAccount_Request* pstReq = stMsg.mutable_m_stmsgbody()->mutable_m_staccountdb_addaccount_request();
    pstReq->mutable_staccountid()->CopyFrom(stAccountID);
    pstReq->set_iworldid(iWorldID);

    //加密密码
    char szEncryptPasswd[256] = {0};
    int iEncryptBuffLen = sizeof(szEncryptPasswd);

    int iRet = CPasswordEncryptionUtility::DoPasswordEncryption(strPasswd.c_str(), strPasswd.size(), szEncryptPasswd, iEncryptBuffLen);
    if(iRet)
    {
        TRACESVR("Failed to encrypt account password, account: %s, password: %s\n", stAccountID.straccount().c_str(), strPasswd.c_str());
        return;
    }

    //设置密码为加密后的密码
    pstReq->set_strpassword(szEncryptPasswd, iEncryptBuffLen);

    //转发消息给AccountDBServer
    if(EncodeAndSendCode(SSProtocolEngine, NULL, &stMsg, GAME_SERVER_ACCOUNTDB) != 0)
    {
        TRACESVR("Failed to send add account request to Account DB server\n");
        return;
    }

    LOGDEBUG("Send add account request to Account DB server\n");

    return;
}

void CAccountDBAddHandler::SendResponseToLotus(unsigned int uiSessionFd, const unsigned int uiResultID)
{
    GameProtocolMsg stResponse;
    GenerateMsgHead(&stResponse, m_pstRequestMsg->m_stmsghead().m_uisessionfd(), MSGID_REGAUTH_REGACCOUNT_RESPONSE, 0);

    //生成消息体
    RegAuth_RegAccount_Response* pstResp = stResponse.mutable_m_stmsgbody()->mutable_m_stregauth_regaccount_response();
    pstResp->set_iresult(uiResultID);

    //获取session的NetHead
    CSessionObj* pSessionObj = SessionManager->GetSession(uiSessionFd);
    ASSERT_AND_LOG_RTN_VOID(pSessionObj);

    TNetHead_V2* pstNetHead = pSessionObj->GetNetHead();
    ASSERT_AND_LOG_RTN_VOID(pstNetHead);

    if (EncodeAndSendCode(CSProtocolEngine, pstNetHead, &stResponse, GAME_SERVER_LOTUSREGAUTH) != 0)
    {
        TRACESVR("Failed to send RegAccountResponse to lotus server\n");
        return;
    }

    TRACESVR("Send RegAccountResponse to lotus server, result: %d\n", uiResultID);

    return;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
