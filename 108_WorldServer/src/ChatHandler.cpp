
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"
#include "ModuleHelper.hpp"
#include "ConfigHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "LogAdapter.hpp"
#include "WorldErrorNumDef.hpp"

#include "ChatHandler.hpp"

using namespace ServerLib;

CChatHandler::~CChatHandler()
{

}

int CChatHandler::OnClientMsg(const void* pMsg)
{
    ASSERT_AND_LOG_RTN_INT(pMsg);

    m_pMsg = (GameProtocolMsg*)pMsg;

    switch (m_pMsg->m_stmsghead().m_uimsgid())
    {
        case MSGID_WORLD_CHAT_NOTIFY:
            {
                OnRequestChatNotify();
                break;
            }

        default:
            {
                break;
            }
    }

    return 0;
}

int CChatHandler::OnRequestChatNotify()
{
    const World_Chat_Notify& rstNotify = m_pMsg->m_stmsgbody().m_stworld_chat_notify();

    //直接转发给ZoneServer
    for (int iZoneID = 1; iZoneID < MAX_ZONE_PER_WORLD; ++iZoneID)
    {
        //不需要转发给发送的人所在的分线
        if(iZoneID != rstNotify.izoneid())
        {
            CWorldMsgHelper::SendWorldMsgToWGS(*m_pMsg, iZoneID);
        }
    }

    return T_SERVER_SUCESS;
}
