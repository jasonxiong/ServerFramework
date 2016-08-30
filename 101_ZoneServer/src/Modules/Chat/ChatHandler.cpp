
#include "GameProtocol.hpp"
#include "GameRole.hpp"
#include "ZoneObjectHelper.hpp"
#include "ZoneMsgHelper.hpp"
#include "ModuleHelper.hpp"
#include "ConfigManager.hpp"
#include "ChatUtility.hpp"

#include "ChatHandler.hpp"

GameProtocolMsg CChatHandler::m_stGameMsg;

CChatHandler::~CChatHandler(void)
{

}

CChatHandler::CChatHandler()
{

}

int CChatHandler::OnClientMsg()
{
    switch (m_pRequestMsg->m_stmsghead().m_uimsgid())
    {
        case MSGID_ZONE_CHAT_REQUEST:
            {
                OnRequestChat();
            }
            break;

        case MSGID_WORLD_CHAT_NOTIFY:
            {
                OnNotifyChat();
            }
            break;

        default:
            {
                LOGERROR("Failed to handler request msg, invalid msgid: %u\n", m_pRequestMsg->m_stmsghead().m_uimsgid());
                return -1;
            }
            break;
    }

    return 0;
}

int CChatHandler::OnRequestChat()
{
    int iRet = SecurityCheck();
    if(iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        
        SendFailedResponse(MSGID_ZONE_CHAT_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

        return -1;
    }

    unsigned int uiUin = m_pRoleObj->GetUin();
    CGameRoleObj* pstRoleObj = GameTypeK32<CGameRoleObj>::GetByKey(uiUin);
    ASSERT_AND_LOG_RTN_INT(pstRoleObj);

    //玩家聊天操作的处理
    const Zone_Chat_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_chat_request();
    CChatUtility::SendChatMsg(*pstRoleObj, rstRequest.ichannel(), rstRequest.strmessage().c_str());

    //处理聊天消息成功，返回
    SendSuccessfulResponse();

    return T_SERVER_SUCESS;
}

int CChatHandler::OnNotifyChat()
{
    //广播给本线的所有用户
    CZoneMsgHelper::SendZoneMsgToZoneAll(*m_pRequestMsg);

    return T_SERVER_SUCESS;
}

int CChatHandler::SendFailedResponse(int iMsgID, int iResultID, const TNetHead_V2& rstNetHead)
{
    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, iMsgID);

    m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_chat_response()->set_iresult(iResultID);

    CZoneMsgHelper::SendZoneMsgToClient(m_stGameMsg, rstNetHead);

    return 0;
}

//发送成功的回复
int CChatHandler::SendSuccessfulResponse()
{
    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_CHAT_RESPONSE);

    CGameRoleObj* pRoleObj = m_pSession->GetBindingRole();
    ASSERT_AND_LOG_RTN_INT(pRoleObj);

    m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_chat_response()->set_iresult(T_SERVER_SUCESS);

    CZoneMsgHelper::SendZoneMsgToRole(m_stGameMsg, pRoleObj);

    return 0;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
