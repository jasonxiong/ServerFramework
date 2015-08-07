#include <assert.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "AuthAccountHandler.hpp"
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

CAuthAccountHandler::CAuthAccountHandler()
{
}

int CAuthAccountHandler::CheckParam()
{
    //todo jasonxiong 这个里面后面需要根据策划需求添加帐号和密码的字符合法性检查

    return 0;
}

void CAuthAccountHandler::OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg, SHandleResult* pstResult)
{
    // 不使用Result
    ASSERT_AND_LOG_RTN_VOID(pstNetHead);
    ASSERT_AND_LOG_RTN_VOID(pstMsg);

    m_pstNetHead = pstNetHead;
    m_pstRequestMsg = pstMsg;

    switch (m_pstRequestMsg->m_stmsghead().m_uimsgid())
    {
        case MSGID_REGAUTH_AUTHACCOUNT_REQUEST:
            {
                //认证平台帐号的请求
                OnRequestAuthAccount();
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

int CAuthAccountHandler::OnRequestAuthAccount()
{
    // 检查请求消息中是否存在非法字段
    if (CheckParam() != 0)
    {
        TRACESVR("Invalid parameter found in the request\n");
        SendFailedResponseToLotus(T_COMMON_SYSTEM_PARA_ERR);
        return -1;
    }

    // 检查session是否已经存在，session个数是否到达上限
    int iRes = SessionManager->CheckSession(*m_pstNetHead);
    if (iRes != T_SERVER_SUCESS)
    {
        SendFailedResponseToLotus(iRes);
        return -4;
    }

    time_t tmNow = time(NULL); // 创建缓存结点的时间

    // 缓存NetHead，即session
    CSessionObj* pSessionObj = SessionManager->CreateSession(*m_pstNetHead);
    ASSERT_AND_LOG_RTN_INT(pSessionObj);
    pSessionObj->SetCreatedTime(&tmNow);

    m_uiSessionFD = pSessionObj->GetSessionFD();
    m_unValue = pSessionObj->GetValue();
    TRACESVR("sockfd: %u, value: %d\n", m_uiSessionFD, m_unValue);

    char szTime[32] = "";
    pSessionObj->GetCreatedTime(szTime, sizeof(szTime));
    TRACESVR("session cache created time: %s", szTime); // 例如: Wed Sep 29 10:59:46 2010

    const RegAuth_AuthAccount_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_stregauth_authaccount_request();

    //todo jasonxiong 后面如果接入第三方平台的话，这边需要添加相应平台的认证

    //向AccountDB发送拉取数据的请求
    CAccountDBFetchHandler::FetchAccount(m_uiSessionFD, rstReq.staccountid(), rstReq.strpassword());

    return T_SERVER_SUCESS;
}

void CAuthAccountHandler::SendFailedResponseToLotus(const unsigned int uiResultID)
{
    GameProtocolMsg stFailedResponse;
    GenerateMsgHead(&stFailedResponse, m_pstRequestMsg->m_stmsghead().m_uisessionfd(), MSGID_REGAUTH_AUTHACCOUNT_RESPONSE, 0);

    RegAuth_AuthAccount_Response* pstResp = stFailedResponse.mutable_m_stmsgbody()->mutable_m_stregauth_authaccount_response();

    pstResp->set_iresult(uiResultID); // 错误码

    if (EncodeAndSendCode(CSProtocolEngine, m_pstNetHead, &stFailedResponse, GAME_SERVER_LOTUSREGAUTH) != 0)
    {
        TRACESVR("Failed to send failed authAccountResponse to lotus server\n");
        return;
    }

    TRACESVR("Send failed authAccountResponse to lotus server, account: %s, result: %d\n",
             m_pstRequestMsg->m_stmsgbody().m_stregauth_authaccount_request().staccountid().straccount().c_str(), uiResultID);

    return;
}

