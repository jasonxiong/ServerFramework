#include <assert.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "FixedHashCache.hpp"
#include "LRUHashCache.hpp"
#include "SessionObj.hpp"
#include "AccountSingleton.hpp"
#include "ProtoDataUtility.hpp"
#include "TimeStampConverter.hpp"

#include "ListRoleResponseHandler.hpp"

using namespace ServerLib;

CListRoleResponseHandler::CListRoleResponseHandler()
{
}

void CListRoleResponseHandler::OnClientMsg(TNetHead_V2* pstNetHead,
        GameProtocolMsg* pstMsg, SHandleResult* pstResult)
{
    // 不使用NetHead和Result
    ASSERT_AND_LOG_RTN_VOID(pstMsg);
    m_pstRequestMsg = pstMsg;
    m_uiSessionFD = TimeStampToSessionFD(m_pstRequestMsg->m_stmsghead().m_uisessionfd());
    m_unValue = TimeStampToValue(m_pstRequestMsg->m_stmsghead().m_uisessionfd());

    Account_ListRole_Response* pstListRoleResponse =
        m_pstRequestMsg->mutable_m_stmsgbody()->mutable_m_staccountlistroleresponse();
    unsigned int uiUin = pstListRoleResponse->uin();
    short nWorldID = pstListRoleResponse->world();

    LOGDETAIL("Handling ListRoleResponse from world server, result: %d, uin: %u, world: %d, role number: %u, sockfd: %u, value: %d\n",
             pstListRoleResponse->iresult(), uiUin, nWorldID, pstListRoleResponse->roles_size(), m_uiSessionFD, m_unValue);

    CSessionObj* pSession = SessionManager->GetSession(m_uiSessionFD, m_unValue);
    if (pSession != NULL) // 原始session仍然存在
    {
        // 是否禁止创建角色
        if (!ConfigMgr->IsCreateRoleEnabled())
        {
            if (pstListRoleResponse->roles_size() == 0)
            {
                pstListRoleResponse->set_iresult(T_ACCOUNT_CANNOT_CREATE_ROLE);
            }
        }

        SendListRoleResponseToLotus();

        // 清除cache中的session结点
        //SessionManager->DeleteSession(m_uiSessionFD);
    }
}

void CListRoleResponseHandler::SendListRoleResponseToLotus()
{
    if (EncodeAndSendCode(CSProtocolEngine,
                          SessionManager->GetSession(m_uiSessionFD, m_unValue)->GetNetHead(),
                          m_pstRequestMsg, GAME_SERVER_LOTUSACCOUNT) != 0)
    {
        LOGERROR("Failed to send ListRoleResponse to lotus server\n");
        return;
    }

    LOGDEBUG("Send ListRoleResponse to lotus server\n");
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
