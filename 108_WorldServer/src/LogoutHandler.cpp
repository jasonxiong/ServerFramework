#include <string.h>

#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "LogAdapter.hpp"

#include "LogoutHandler.hpp"

CLogoutHandler::CLogoutHandler(void)
{
}

CLogoutHandler::~CLogoutHandler(void)
{
}

GameProtocolMsg CLogoutHandler::m_stWorldMsg;
int CLogoutHandler::OnClientMsg(const void* pMsg)
{
    ASSERT_AND_LOG_RTN_INT(pMsg);

    m_pRequestMsgWGS = (GameProtocolMsg*)pMsg;

    switch (m_pRequestMsgWGS->m_stmsghead().m_uimsgid())
    {
    case MSGID_ZONE_LOGOUT_NOTIFY:
        {
            OnLogoutNotify();
            break;
        }
    default:
        {
            break;
        }
    }

    return 0;
}

int CLogoutHandler::OnLogoutNotify()
{
    const Zone_Logout_Notify& rstNotify = m_pRequestMsgWGS->m_stmsgbody().m_stzone_logout_notify();
    CWorldRoleStatusWObj* pWorldRoleStatusWObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(rstNotify.stroleid());
    if (!pWorldRoleStatusWObj)
    {
        return -1;
    }

    LogoutNotify(pWorldRoleStatusWObj);

    return 0;
}

// 通知Zone服务器角色下线
int CLogoutHandler::LogoutRole(int iZoneID, const unsigned int uin)
{
    //todo jasonxiong 暂时先注释掉这个函数的实现，后面如果需要实现再增加
    /*
    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, time(NULL), M, rstRoleID.m_uiUin);
    KICKROLE_WORLD_REQUEST& rstWorldRequest = m_stWorldMsg.m_stMsgBody.m_stKickRoleWorld_Request;

    rstWorldRequest.m_stRoleID = rstRoleID;

    // 向Zone发送踢人请求
    CWorldMsgHelper::SendWorldMsgToWGS(m_stWorldMsg, iZoneID);

    return 0;
    */

    return 0;
}

int CLogoutHandler::LogoutNotify(CWorldRoleStatusWObj* pRoleObj)
{
    unsigned int uiUin = pRoleObj->GetRoleID().uin();

    LOGDEBUG("LogoutNotify: Uin = %u\n", uiUin);

    /*
    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, 0, MSGID_ZONE_LOGOUT_NOTIFY, uiUin);

    Zone_Logout_Notify* pstNotify = m_stWorldMsg.mutable_m_stmsgbody()->mutable_m_stzone_logout_notify();

    pstNotify->mutable_stroleid()->CopyFrom(pRoleObj->GetRoleID());

    //向主备cluster都发一份
	//CWorldMsgHelper::SendWorldMsgToCluster(m_stWorldMsg);
    */

    WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);

    return 0;
}





