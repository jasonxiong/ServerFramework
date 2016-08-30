
#include <assert.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "SessionObj.hpp"
#include "AccountSingleton.hpp"

#include "ListZoneHandler.hpp"

using namespace ServerLib;

CListZoneHandler::CListZoneHandler()
{
    m_pstNetHead = NULL;
    m_pstRequestMsg = NULL;
}

void CListZoneHandler::OnClientMsg(TNetHead_V2* pstNetHead,
        GameProtocolMsg* pstMsg, SHandleResult* pstResult)
{
    ASSERT_AND_LOG_RTN_VOID(pstMsg);

    m_pstRequestMsg = pstMsg;
    m_pstNetHead = pstNetHead;

    switch (m_pstRequestMsg->m_stmsghead().m_uimsgid())
    {
        case MSGID_ACCOUNT_LISTZONE_REQUEST:
            {
                SendListZoneRequestToWorld();
            }
            break;

        case MSGID_ACCOUNT_LISTZONE_RESPONSE:
            {
                SendListZoneResponseToLotus();
            }
            break;

        default:
            {
                return;
            }
    }

}

void CListZoneHandler::SendListZoneRequestToWorld()
{
    ASSERT_AND_LOG_RTN_VOID(m_pstNetHead);

    // 修改消息头中的TimeStamp
    m_pstRequestMsg->mutable_m_stmsghead()->set_m_uisessionfd(ntohl(m_pstNetHead->m_uiSocketFD));

    if (EncodeAndSendCode(SSProtocolEngine,
                          NULL, m_pstRequestMsg, GAME_SERVER_WORLD) != 0)
    {
        LOGERROR("Failed to send ListZoneRequest to world server\n");
        return;
    }

    LOGDEBUG("Send ListZoneRequest to world server, uin %u\n", m_pstRequestMsg->m_stmsghead().m_uin());

    return;
}

void CListZoneHandler::SendListZoneResponseToLotus()
{
    unsigned int uiSessionFD = m_pstRequestMsg->m_stmsghead().m_uisessionfd();
    if (EncodeAndSendCode(CSProtocolEngine,
                          SessionManager->GetSession(uiSessionFD)->GetNetHead(),
                          m_pstRequestMsg, GAME_SERVER_LOTUSACCOUNT) != 0)
    {
        LOGERROR("Failed to send ListZoneResponse to lotus server\n");
        return;
    }

    LOGDEBUG("Send ListZoneResponse to lotus server\n");
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
