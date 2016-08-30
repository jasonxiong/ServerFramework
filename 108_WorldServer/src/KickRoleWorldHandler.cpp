#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "LogAdapter.hpp"
#include "WorldRoleStatus.hpp"

#include "KickRoleWorldHandler.hpp"

CKickRoleWorldHandler::~CKickRoleWorldHandler(void)
{
}


GameProtocolMsg CKickRoleWorldHandler::ms_stGameMsg;
int CKickRoleWorldHandler::OnClientMsg(const void* pMsg)
{
    ASSERT_AND_LOG_RTN_INT(pMsg);

    m_pRequestMsg = (GameProtocolMsg*)pMsg;

    switch (m_pRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_WORLD_KICKROLE_REQUEST:
        {
            OnZoneRequestKickRole();
            break;
        }

    case MSGID_WORLD_KICKROLE_RESPONSE:
        {
            OnZoneResponseKickRole();
            break;
        } 

    default:
        {
            break;
        }
    }

    return 0;
}

// 处理来自Zone的踢人请求
int CKickRoleWorldHandler::OnZoneRequestKickRole()
{
    const World_KickRole_Request& rstReq = m_pRequestMsg->m_stmsgbody().m_stworld_kickrole_request();

    int iRet = 0;

    // 判断本world的登录情况
    LOGDEBUG("Recv Zone KickRole Req: Uin = %u, From ZoneID = %d\n", rstReq.stkickedroleid().uin(), rstReq.ifromzoneid()); 

    CWorldRoleStatusWObj* pWorldRoleStatusWObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(rstReq.stkickedroleid());

    //如果就在该world登录, 向Zone发送踢人请求
    if (pWorldRoleStatusWObj != NULL)
    {
        int iZoneID = pWorldRoleStatusWObj->GetZoneID();
        if (CModuleHelper::GetZoneTick()->IsZoneActive(iZoneID))
        {
            CWorldMsgHelper::GenerateMsgHead(ms_stGameMsg, 0, MSGID_WORLD_KICKROLE_REQUEST, rstReq.stkickedroleid().uin());

            World_KickRole_Request* pstKickReq = ms_stGameMsg.mutable_m_stmsgbody()->mutable_m_stworld_kickrole_request();
            pstKickReq->CopyFrom(rstReq);

            iRet =  CWorldMsgHelper::SendWorldMsgToWGS(ms_stGameMsg, iZoneID);
            LOGDEBUG("Send zone KickRole Req: iRet:%d Uin = %u, zoneid:%d\n",iRet, rstReq.stkickedroleid().uin(), iZoneID);
            return 0;
        }
        else
        {
            // 直接删除该uin的信息，重新登录
            WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(rstReq.stkickedroleid().uin());
        }
    }

    // 向Zone发送回复
    CWorldMsgHelper::GenerateMsgHead(ms_stGameMsg, 0, MSGID_WORLD_KICKROLE_RESPONSE, rstReq.stkickedroleid().uin());
    World_KickRole_Response* pstKickResp = ms_stGameMsg.mutable_m_stmsgbody()->mutable_m_stworld_kickrole_response();
    pstKickResp->set_ifromworldid(rstReq.ifromworldid());
    pstKickResp->set_ifromzoneid(rstReq.ifromzoneid());
    pstKickResp->set_isessionid(rstReq.isessionid());
    pstKickResp->set_bislogin(rstReq.bislogin());
    pstKickResp->mutable_stkickedroleid()->CopyFrom(rstReq.stkickedroleid());
    
    pstKickResp->set_iresult(T_SERVER_SUCESS);

    iRet = CWorldMsgHelper::SendWorldMsgToWGS(ms_stGameMsg, rstReq.ifromzoneid());

    LOGDEBUG("Send Zone KickRole Resp: Uin = %u, iRet = %d\n", rstReq.stkickedroleid().uin(), iRet);

    return 0;
}

// 处理来自Zone的被踢回复
int CKickRoleWorldHandler::OnZoneResponseKickRole()
{
    const World_KickRole_Response& rstResp = m_pRequestMsg->m_stmsgbody().m_stworld_kickrole_response();
    unsigned int uin = rstResp.stkickedroleid().uin();
    int iRet = -1;

    LOGDEBUG("Recv Zone KickRole Resp: Uin = %u, result = %d\n", uin, rstResp.iresult());

    // 如果踢人成功, 则再次确认World没有数据. 如果有, 则强制删除缓存数据.
    if (rstResp.iresult() == T_SERVER_SUCESS)
    {
        CWorldRoleStatusWObj* pWorldRoleStatusWObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(rstResp.stkickedroleid());
        if (pWorldRoleStatusWObj)
        {
            WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uin);
        }
    }

    //登录的zone和踢人的zone属于一个world
    if (rstResp.ifromworldid() == CModuleHelper::GetWorldID())
    {
        // 向Zone发送回复
        CWorldMsgHelper::GenerateMsgHead(ms_stGameMsg, 0, MSGID_WORLD_KICKROLE_RESPONSE, uin);
        World_KickRole_Response* pstKickResp = ms_stGameMsg.mutable_m_stmsgbody()->mutable_m_stworld_kickrole_response();
        pstKickResp->CopyFrom(rstResp);

        iRet =CWorldMsgHelper::SendWorldMsgToWGS(ms_stGameMsg, rstResp.ifromzoneid());

        LOGDEBUG("Send Zone KickRole Resp: Uin = %u, iRet = %d\n", uin, iRet);

    }
    //换world登录，不回包了
    else
    {

    }

    return iRet;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
