#include <assert.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "FixedHashCache.hpp"
#include "LRUHashCache.hpp"
#include "RoleNumberObj.hpp"
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
        // 创建缓存
        CRoleNumberObj* pRoleNumberObj = CLRUHashCache<CRoleNumberObj>::GetByUin(uiUin);
        if (!pRoleNumberObj)
        {
            // 该用户还未在角色个数缓存区中创建结点或者cache被换出
            // 这里首先需要访问数据库，将数据加载到cache中，如果该用户在所有的world上的角色个数都为0，
            // 则置cache中none role标志为true。

            // 创建缓存结点并初始化
            pRoleNumberObj = CLRUHashCache<CRoleNumberObj>::CreateByUin(uiUin);
            if (pRoleNumberObj)
            {
                pRoleNumberObj->Initialize();
                pRoleNumberObj->SetUin(uiUin);

                // 初始化缓存数据
                int i = 0;
                for (i = 0; i < pstListRoleResponse->roles_size(); i++)
                {
                    pRoleNumberObj->AddOneRoleToWorld(pstListRoleResponse->roles(i).stroleid());
                }
            }
            else
            {
                LOGERROR("Failed to create RoleNumberObj in cache, uin: %u\n", uiUin);
            }
        }

        // 将该响应消息加上NetHead后转发给LotusServer

        // 是否禁止创建角色
        if (!ConfigMgr->IsCreateRoleEnabled())
        {
            if (pstListRoleResponse->roles_size() == 0)
            {
                pstListRoleResponse->set_iresult(T_ACCOUNT_CANNOT_CREATE_ROLE);
            }
        }

        //将存档摘要信息解码
        //todo jasonxiong2 这部分暂时去掉,应该不需要多角色
        /*
        for(int i=0; i<pstListRoleResponse->roles_size(); ++i)
        {
            RoleSummary* pstOneRole = pstListRoleResponse->mutable_roles(i);

            for(int j=0; j<pstOneRole->strdigests_size(); ++j)
            {
                if(!DecodeProtoData(pstOneRole->strdigests(j), *pstOneRole->add_stdidests()))
                {
                    LOGERROR("Failed to decode base role info, uin %u\n", pstListRoleResponse->uin());
                    continue;
                }
            }

            //清除未解码数据
            pstOneRole->clear_strdigests();
        }
        */

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
