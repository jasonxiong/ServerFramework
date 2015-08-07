#include <assert.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "FixedHashCache.hpp"
#include "SessionObj.hpp"
#include "AccountSingleton.hpp"
#include "TimeStampConverter.hpp"
#include "DeleteRoleRequestHandler.hpp"
#include "AccountApp.hpp"

CDeleteRoleRequestHandler::CDeleteRoleRequestHandler()
{
}

int CDeleteRoleRequestHandler::CheckParam()
{
    // 对m_pstRequestMsg中的字段做检查
    DeleteRole_Account_Request* pstDeleteRoleRequest =
        m_pstRequestMsg->mutable_m_stmsgbody()->mutable_m_staccountdeleterolerequest();

    // 检查Uin
    CSessionObj* pSessionObj = SessionManager->GetSession(ntohl(m_pstNetHead->m_uiSocketFD), 0);
    if (!pSessionObj || pSessionObj->GetUin() != m_pstRequestMsg->m_stmsghead().m_uin())
    {
        return -1;
    }

    if (m_pstRequestMsg->m_stmsghead().m_uin() != pstDeleteRoleRequest->stroleid().uin())
    {
        LOGERROR("Invalid Uin: %d\n", pstDeleteRoleRequest->stroleid().uin());
        return -1;
    }

//    unsigned int uiDBSeq = pstDeleteRoleRequest->m_stRoleID.m_uiSeq;
//    if (CheckDBSeq(uiDBSeq) != 0)
//    {
//        return -1;
//    }
//
    return 0;
}

void CDeleteRoleRequestHandler::OnClientMsg(TNetHead_V2* pstNetHead,
        GameProtocolMsg* pstMsg, SHandleResult* pstResult)
{
    // 不使用Result
    ASSERT_AND_LOG_RTN_VOID(pstNetHead);
    ASSERT_AND_LOG_RTN_VOID(pstMsg);

    m_pstNetHead = pstNetHead;
    m_pstRequestMsg = pstMsg;

    // 检查请求消息中是否存在非法字段
    if (CheckParam() != 0)
    {
        LOGERROR("Invalid parameter found in the request\n");
        SendFailedResponseToLotus(T_ACCOUNT_SYSTEM_PARA_ERR);
        return;
    }

    DeleteRole_Account_Request* pstDeleteRoleRequest =
        m_pstRequestMsg->mutable_m_stmsgbody()->mutable_m_staccountdeleterolerequest();
    unsigned int uiUin = pstDeleteRoleRequest->stroleid().uin();

    LOGDEBUG("Handling DeleteRoleRequest from lotus server, "
             "uin: %u\n", uiUin);

    // 检查session是否已经存在，session个数是否到达上限
    int iRes = SessionManager->CheckSession(*m_pstNetHead);
    if (iRes != T_SERVER_SUCESS)
    {
        SendFailedResponseToLotus(iRes);
        return;
    }

    time_t tmNow = time(NULL); // 创建缓存结点的时间

    // 缓存NetHead，即session
    CSessionObj* pSessionObj = SessionManager->CreateSession(*m_pstNetHead);
    ASSERT_AND_LOG_RTN_VOID(pSessionObj);
    pSessionObj->SetCreatedTime(&tmNow);
    //pSessionObj->SetClientVersion(m_pstRequestMsg->m_stMsgHead.m_shMsgVersion);

    m_uiSessionFD = pSessionObj->GetSessionFD();
    m_unValue = pSessionObj->GetValue();
    LOGDEBUG("sockfd: %u, value: %d\n", m_uiSessionFD, m_unValue);

    char szTime[32] = "";
    pSessionObj->GetCreatedTime(szTime, sizeof(szTime));

    LOGDEBUG("session cache created time: %s", szTime); // 例如: Wed Sep 29 10:59:46 2010

    // 将消息转发给world server
    SendDeleteRoleRequestToWorld();
}

void CDeleteRoleRequestHandler::SendDeleteRoleRequestToWorld()
{
    // 修改消息头中的TimeStamp
    m_pstRequestMsg->mutable_m_stmsghead()->set_m_uisessionfd(GenerateTimeStamp(m_uiSessionFD, m_unValue));

    if (EncodeAndSendCode(SSProtocolEngine,
                          NULL, m_pstRequestMsg, GAME_SERVER_WORLD) != 0)
    {
        LOGERROR("Failed to send DeleteRoleRequest to world server\n");
        return;
    }

    LOGDEBUG("Send DeleteRoleRequest to world server\n");
}

void CDeleteRoleRequestHandler::SendFailedResponseToLotus(const unsigned int uiResultID)
{
    GameProtocolMsg stFailedResponse;

    GenerateMsgHead(&stFailedResponse,
        m_pstRequestMsg->m_stmsghead().m_uisessionfd(), MSGID_ACCOUNT_DELETEROLE_RESPONSE, m_pstRequestMsg->m_stmsghead().m_uin());

    stFailedResponse.mutable_m_stmsgbody()->mutable_m_staccountdeleteroleresponse()->set_iresult(uiResultID);

    if (EncodeAndSendCode(CSProtocolEngine,
                          m_pstNetHead, &stFailedResponse, GAME_SERVER_LOTUSACCOUNT) != 0)
    {
        LOGERROR("Failed to send failed DeleteRoleResponse to lotus server\n");
        return;
    }

    LOGDEBUG("Send failed DeleteRoleResponse to lotus server, uin: %u, result: %d\n", stFailedResponse.m_stmsghead().m_uin(), uiResultID);
}

