#include <string.h>

#include "Handler.hpp"
#include "ConfigHelper.hpp"
#include "AccountDBApp.hpp"

void CHandler::GenerateResponseMsgHead(GameProtocolMsg* pstServerMsg, unsigned int uiSessionID, unsigned int uiMsgID, unsigned int uiUin)
{
    GameCSMsgHead* pstMsgHead = pstServerMsg->mutable_m_stmsghead();

    pstMsgHead->set_m_uimsgid((ProtocolMsgID)uiMsgID);
    pstMsgHead->set_m_uin(uiUin);
    pstMsgHead->set_m_uisessionfd(uiSessionID);

    pstServerMsg->mutable_m_stmsgbody()->Clear();
}
