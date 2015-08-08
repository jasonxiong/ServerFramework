#include <assert.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "FixedHashCache.hpp"
#include "LRUHashCache.hpp"
#include "CreateRoleRequestObj.hpp"
#include "SessionObj.hpp"
#include "AccountSingleton.hpp"
#include "TimeStampConverter.hpp"
#include "CreateRoleRequestHandler.hpp"
#include "StringUtility.hpp"
#include "ConfigMgr.hpp"
#include "AccountApp.hpp"

using namespace ServerLib;

CCreateRoleRequestHandler::CCreateRoleRequestHandler()
{
}

int CCreateRoleRequestHandler::CheckParam()
{
    // 对m_pstRequestMsg中的字段做检查
    CreateRole_Account_Request* pstCreateRoleRequest =
        m_pstRequestMsg->mutable_m_stmsgbody()->mutable_m_staccountcreaterolerequest();

    // 检查Uin
    CSessionObj* pSessionObj = SessionManager->GetSession(ntohl(m_pstNetHead->m_uiSocketFD), 0);
    if (!pSessionObj || pSessionObj->GetUin() != m_pstRequestMsg->m_stmsghead().m_uin())
    {
        return -1;
    }

    if (m_pstRequestMsg->m_stmsghead().m_uin() != pstCreateRoleRequest->uin())
    {
        LOGERROR("Invalid Uin: %d\n", pstCreateRoleRequest->uin());
        return -1;
    }

    return 0;
}

void CCreateRoleRequestHandler::OnClientMsg(TNetHead_V2* pstNetHead,
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
        SendFailedResponseToLotus(T_COMMON_SYSTEM_PARA_ERR);
        return;
    }

    const CreateRole_Account_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_staccountcreaterolerequest();
    const char* pszNickName = rstReq.sznickname().c_str();
    unsigned int uiUin = rstReq.uin();
    short nWorldID = rstReq.worldid();

    if(nWorldID != CAccountApp::GetWorldID())
    {
        LOGERROR("Failed to create role account, invalid world id %d, uin %u\n", nWorldID, uiUin);
        return;
    }

    // 是否禁止创建角色
    if (!ConfigMgr->IsCreateRoleEnabled())
    {
        SendFailedResponseToLotus(T_ACCOUNT_CANNOT_CREATE_ROLE);
        return;
    }

    // 以前的相同uin的创建角色请求消息缓存结点还没有删除，拒绝服务
    if (CFixedHashCache<CCreateRoleRequestObj>::GetByUin(uiUin) != NULL)
    {
        LOGERROR("Previous CreateRoleRequestObj cache node has not been deleted, uin: %u\n", uiUin);
        return;
    }

    LOGERROR("Handling CreateRoleRequest from lotus server, uin: %u, world: %u, nickname: %s\n", uiUin, nWorldID, pszNickName);

    // 检查session是否已经存在，session个数是否到达上限
    int iRes = SessionManager->CheckSession(*m_pstNetHead);
    if (iRes != T_SERVER_SUCESS)
    {
        SendFailedResponseToLotus(iRes);
        return;
    }

    // 检查cache中的创建角色消息个数是否达到上限，达到上限则拒绝服务
    if (CFixedHashCache<CCreateRoleRequestObj>::GetFreeNodeNumber() < 1)
    {
        LOGERROR("CreateRoleRequestObj cache is full, uin: %u\n", uiUin);
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
    LOGERROR("sockfd: %u, value: %d\n", m_uiSessionFD, m_unValue);

    char szTime[32] = "";
    pSessionObj->GetCreatedTime(szTime, sizeof(szTime));
    LOGERROR("session cache created time: %s", szTime); // 例如: Wed Sep 29 10:59:46 2010

    // 缓存该创建角色消息
    CCreateRoleRequestObj* pRequestObj = CFixedHashCache<CCreateRoleRequestObj>::CreateByUin(uiUin);
    ASSERT_AND_LOG_RTN_VOID(pRequestObj);
    pRequestObj->Initialize();
    pRequestObj->SetRequestInfo(rstReq);
    pRequestObj->SetCreatedTime(&tmNow);

    pRequestObj->GetCreatedTime(szTime, sizeof(szTime));
    LOGERROR("CreateRoleRequest cache created time: %s", szTime);

    // 发送增加新名字请求给WorldServer
    SendAddNewNameRequestToWorld(rstReq);

    return;
}

void CCreateRoleRequestHandler::SendAddNewNameRequestToWorld(const CreateRole_Account_Request& rstReq)
{
    static GameProtocolMsg stNameMsg;

    GenerateMsgHead(&stNameMsg, m_uiSessionFD, MSGID_ADDNEWNAME_REQUEST, rstReq.uin());

    //填充消息体
    AddNewName_Request* pstReq = stNameMsg.mutable_m_stmsgbody()->mutable_m_staddnewname_request();
    pstReq->set_strname(rstReq.sznickname());
    pstReq->set_itype(EN_NAME_TYPE_ROLE);
    pstReq->set_name_id(rstReq.uin());

    if (EncodeAndSendCode(SSProtocolEngine,
                          NULL, &stNameMsg, GAME_SERVER_WORLD) != 0)
    {
        LOGERROR("Failed to send AddNewNameRequest to world server\n");
        return;
    }

    LOGERROR("Send AddNewNameRequest to world server\n");
}

void CCreateRoleRequestHandler::SendFailedResponseToLotus(const unsigned int uiResultID)
{
    GameProtocolMsg stFailedResponse;
    GenerateMsgHead(&stFailedResponse,
        m_pstRequestMsg->m_stmsghead().m_uisessionfd(), MSGID_ACCOUNT_CREATEROLE_RESPONSE, m_pstRequestMsg->m_stmsghead().m_uin());

    CreateRole_Account_Response* pstCreateRoleResponse =
        stFailedResponse.mutable_m_stmsgbody()->mutable_m_staccountcreateroleresponse();

    pstCreateRoleResponse->set_iresult(uiResultID); // 错误码
    //todo jasonxiong2 单机版不需要名字
    //pstCreateRoleResponse->set_sznickname(m_pstRequestMsg->m_stmsgbody().m_staccountcreaterolerequest().sznickname());

    if (EncodeAndSendCode(CSProtocolEngine,
                          m_pstNetHead, &stFailedResponse, GAME_SERVER_LOTUSACCOUNT) != 0)
    {
        LOGERROR("Failed to send failed CreateRoleResponse to lotus server\n");
        return;
    }

    LOGERROR("Send failed CreateRoleResponse to lotus server, uin: %u, result: %d\n",
             stFailedResponse.m_stmsghead().m_uin(),
             stFailedResponse.m_stmsgbody().m_staccountcreateroleresponse().iresult());
}

