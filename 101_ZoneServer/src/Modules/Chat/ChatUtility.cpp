
#include "GameRole.hpp"
#include "ZoneErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "ZoneMsgHelper.hpp"
#include "ModuleHelper.hpp"

#include "ChatUtility.hpp"

using namespace ServerLib;

int CChatUtility::SendChatMsg(CGameRoleObj& stRoleObj, int iChannel, const char* pMsg)
{
    if(!pMsg)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    static GameProtocolMsg stChatNotify;
    CZoneMsgHelper::GenerateMsgHead(stChatNotify, MSGID_WORLD_CHAT_NOTIFY);

    World_Chat_Notify* pstNotify = stChatNotify.mutable_m_stmsgbody()->mutable_m_stworld_chat_notify();

    switch(iChannel)
    {
        case CHAT_CHANNEL_WORLD:
            {
                pstNotify->set_ichannel((ChatChannelType)iChannel);
                pstNotify->mutable_stroleid()->CopyFrom(stRoleObj.GetRoleID());
                //todo jasonxiong2 单机版没有名字
                //pstNotify->set_sznickname(stRoleObj.GetNickName());
                pstNotify->set_izoneid(CModuleHelper::GetZoneID());
                pstNotify->set_szmessage(pMsg);

                //先广播给本线的玩家
                CZoneMsgHelper::SendZoneMsgToZoneAll(stChatNotify);

                //转发给世界服务器
                CZoneMsgHelper::SendZoneMsgToWorld(stChatNotify);
            }
            break;

        case CHAT_CHANNEL_SYSTEM:
            {
                //系统消息
                pstNotify->set_ichannel((ChatChannelType)iChannel);
                pstNotify->set_szmessage(pMsg);

                //发送给客户端
                CZoneMsgHelper::SendZoneMsgToRole(stChatNotify, &stRoleObj);
            }
            break;

        case CHAT_CHANNEL_PRIVATE:
            {
                //玩家私聊，暂时不开发
                ;
            }
            break;

        default:
            {
                LOGERROR("Failed to do chat, invalid channel %d, uin %u\n", iChannel, stRoleObj.GetUin());
                return T_ZONE_SYSTEM_PARA_ERR;
            }
            break;
    }

    return T_SERVER_SUCESS;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
