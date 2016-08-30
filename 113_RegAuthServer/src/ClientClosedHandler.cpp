#include <assert.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "SessionObj.hpp"
#include "RegAuthSingleton.hpp"
#include "ClientClosedHandler.hpp"

CClientClosedHandler::CClientClosedHandler()
{
}

void CClientClosedHandler::OnClientMsg(TNetHead_V2* pstNetHead,
                                       GameProtocolMsg* pstMsg, SHandleResult* pstResult)
{
    // 不使用Result
    ASSERT_AND_LOG_RTN_VOID(pstNetHead);
    ASSERT_AND_LOG_RTN_VOID(pstMsg);

    // 清除cache中的session结点
    unsigned int uiSessionFD = ntohl(pstNetHead->m_uiSocketFD);
    TRACESVR("Handling ClientClosedRequest from lotus server, sockfd: %u\n", uiSessionFD);
    SessionManager->DeleteSession(uiSessionFD);
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
