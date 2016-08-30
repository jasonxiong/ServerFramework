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
#include "TimeUtility.hpp"
#include "RegAuthBillLog.hpp"

using namespace ServerLib;

CAccountDBFetchHandler::CAccountDBFetchHandler()
{
}

void CAccountDBFetchHandler::OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg, SHandleResult* pstResult)
{
    // 不使用Result
    ASSERT_AND_LOG_RTN_VOID(pstNetHead);
    ASSERT_AND_LOG_RTN_VOID(pstMsg);

    m_pstNetHead = pstNetHead;
    m_pstRequestMsg = pstMsg;

    switch (m_pstRequestMsg->m_stmsghead().m_uimsgid())
    {
        case MSGID_ACCOUNTDB_FETCH_RESPONSE:
            {
                //拉取帐号信息的返回
                OnResponseAccountDBFetch();
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

int CAccountDBFetchHandler::OnResponseAccountDBFetch()
{
    //获取发送消息的FD
    unsigned int uiSessionFd = m_pstRequestMsg->m_stmsghead().m_uisessionfd();

    //AccountDB拉取帐号信息的返回
    const AccountDB_FetchAccount_Response& rstResp = m_pstRequestMsg->m_stmsgbody().m_staccountdb_fetch_response();
    if(rstResp.iresult() != 0)
    {
        //认证查询帐号失败，返回认证失败给LotusServer
        SendAuthFailedResponseToLotus(uiSessionFd, rstResp.iresult());

        return -1;
    }

    //获取所在服务器组的配置
    const WulinServerConfig* pstServerConfig = ConfigMgr->GetServerInfo(rstResp.iworldid());
    if(!pstServerConfig)
    {
        TRACESVR("Failed to get server config, world id %d, uin %u\n", rstResp.iworldid(), rstResp.uin());

        SendAuthFailedResponseToLotus(uiSessionFd, T_REGAUTH_SYSTEM_PARA_ERR);
        return -2;
    }

    //认证登录成功，返回认证成功给LotusServer
    SendAuthSuccessResponseToLotus(uiSessionFd, *pstServerConfig, rstResp.uin(), rstResp.bisbinded());

    LOGDEBUG("Success to do auth account, account: %s, uin %u, lastworld:%d\n", rstResp.staccountid().straccount().c_str(), rstResp.uin(), rstResp.iworldid());

    return T_SERVER_SUCESS;
}

//拉取帐号详细信息
void CAccountDBFetchHandler::FetchAccount(unsigned uiSessionFd, const AccountID& stAccountID, const std::string& strPassword)
{
    static GameProtocolMsg stMsg;

    // 修改消息头中的Session FD
    GenerateMsgHead(&stMsg, uiSessionFd, MSGID_ACCOUNTDB_FETCH_REQUEST, GetAccountHash(stAccountID.straccount()));

    //AccountDB拉取帐号信息的请求
    AccountDB_FetchAccount_Request* pstReq = stMsg.mutable_m_stmsgbody()->mutable_m_staccountdb_fetch_request();
    pstReq->mutable_staccountid()->CopyFrom(stAccountID);

    //加密密码
    if(strPassword.size() != 0)
    {
        char szEncryptPasswd[256] = {0};
        int iEncryptBuffLen = sizeof(szEncryptPasswd);

        int iRet = CPasswordEncryptionUtility::DoPasswordEncryption(strPassword.c_str(), strPassword.size(), szEncryptPasswd, iEncryptBuffLen);
        if(iRet)
        {
            TRACESVR("Failed to encrypt account password, account: %s, password: %s\n", stAccountID.straccount().c_str(), strPassword.c_str());
            return;
        }

        //设置密码为加密后的密码
        pstReq->set_strpassword(szEncryptPasswd, iEncryptBuffLen);
    }
    
    //转发消息给AccountDBServer
    if (EncodeAndSendCode(SSProtocolEngine, NULL, &stMsg, GAME_SERVER_ACCOUNTDB) != 0)
    {
        TRACESVR("Failed to send fetch account request to Account DB server\n");
        return;
    }

    LOGDEBUG("Send fetch account request to Account DB server\n");

    return;
}

//发送认证失败回复给LotusServer
void CAccountDBFetchHandler::SendAuthFailedResponseToLotus(unsigned int uiSessionFd, const unsigned int uiResultID)
{
    static GameProtocolMsg stResponse;
    GenerateMsgHead(&stResponse, uiSessionFd, MSGID_REGAUTH_AUTHACCOUNT_RESPONSE, 0);

    //生成消息体
    RegAuth_AuthAccount_Response* pstResp = stResponse.mutable_m_stmsgbody()->mutable_m_stregauth_authaccount_response();
    pstResp->set_iresult(uiResultID);

    if (EncodeAndSendCodeBySession(CSProtocolEngine, uiSessionFd, &stResponse, GAME_SERVER_LOTUSREGAUTH) != 0)
    {
        TRACESVR("Failed to send authaccount response to lotus server\n");
        return;
    }

    LOGDEBUG("Send authaccount response to lotus server, result: %d\n", uiResultID);

    return;
}

//发送认证成功回复给LotusServer
void CAccountDBFetchHandler::SendAuthSuccessResponseToLotus(unsigned int uiSessionFd, const WulinServerConfig& stServerConfig, unsigned int uin, bool bIsBinded)
{
    static GameProtocolMsg stResponse;
    GenerateMsgHead(&stResponse, uiSessionFd, MSGID_REGAUTH_AUTHACCOUNT_RESPONSE, 0);

    //生成响应消息体
    RegAuth_AuthAccount_Response* pstResp = stResponse.mutable_m_stmsgbody()->mutable_m_stregauth_authaccount_response();
    pstResp->set_iresult(T_SERVER_SUCESS);
    pstResp->set_uin(uin);
    pstResp->set_iworldid(stServerConfig.iServerID);
    pstResp->set_straccountip(stServerConfig.szAccountIP);
    pstResp->set_strzoneip(stServerConfig.szZoneIP);

    //生成并返回sessionkey
    //session key: uin|time|bIsBinded|WorldID
    static char szOriginSessionKey[256];
    SAFE_SPRINTF(szOriginSessionKey, sizeof(szOriginSessionKey)-1, "%u|%u|%d|%d", uin, (unsigned)CTimeUtility::GetNowTime(), bIsBinded, stServerConfig.iServerID);

    static char szSessionKey[256];
    int iSessionKeyLen = sizeof(szSessionKey);
    CSessionKeyUtility::GenerateSessionKey(szOriginSessionKey, strlen(szOriginSessionKey)+1, szSessionKey, iSessionKeyLen);

    pstResp->set_strsessionkey(szSessionKey, iSessionKeyLen);

    if (EncodeAndSendCodeBySession(CSProtocolEngine, uiSessionFd, &stResponse, GAME_SERVER_LOTUSREGAUTH) != 0)
    {
        TRACESVR("Failed to send authaccount response to lotus server\n");
        return;
    }

    LOGDEBUG("Send authaccount response to lotus server, result: %d\n", T_SERVER_SUCESS);

    return;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
