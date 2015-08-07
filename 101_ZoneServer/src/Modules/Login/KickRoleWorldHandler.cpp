
#include "AppLoop.hpp"
#include "LogAdapter.hpp"
#include "HandlerHelper.hpp"
#include "ZoneMsgHelper.hpp"
#include "LogoutHandler.hpp"
#include "UnitUtility.hpp"

#include "KickRoleWorldHandler.hpp"

int CalcForbidTime(int iForbidType, int iIsForbidden)
{
	//对应封号时间
	//永久封号设为2年
	int aiForbidTime[] = {0, 3600, 3*3600, 5*3600, 24*3600, 2*365*3600};
	int tForbidTime = 0;	
	//封号
	if (iIsForbidden)
	{
		if ((iForbidType < 0))
		{
			TRACESVR("unknown forbidden tyep:%d\n", iForbidType);
			tForbidTime = 0;
		}
		else
		{
			tForbidTime = time(NULL) + aiForbidTime[iForbidType];
		}
	}
	//解封
	else
	{
		tForbidTime = 0;
	}
	return tForbidTime;
}

CKickRoleWorldHandler::~CKickRoleWorldHandler()
{
}

GameProtocolMsg CKickRoleWorldHandler::ms_stZoneMsg;
int CKickRoleWorldHandler::OnClientMsg()
{
    switch (m_pRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_WORLD_KICKROLE_REQUEST:
        {
            OnRequestKickRole();
            break;
        }

    case MSGID_WORLD_KICKROLE_RESPONSE:
        {
            OnResponseKickRole();
            break;
        }

    default:
        {
            break;
        }
    }

    return 0;
}

// 收到World的被踢下线通知
int CKickRoleWorldHandler::OnRequestKickRole()
{
	const World_KickRole_Request& rstReq = m_pRequestMsg->m_stmsgbody().m_stworld_kickrole_request();
	const unsigned int uiKickedUin = rstReq.stkickedroleid().uin();

    LOGDEBUG("Recv KickRole Req: Uin = %u\n", uiKickedUin);

    CGameRoleObj* pRoleObj = CUnitUtility::GetRoleByUin(uiKickedUin);
    // 找不到玩家, 返回成功
    if (!pRoleObj)
    {
        LOGERROR("KickRole OK: Uin = %u\n", uiKickedUin);

        SendSuccessfullResponse(rstReq);
        return 0;
    }

	//设置踢人者
	pRoleObj->SetKicker(rstReq);

    // 将玩家踢下线 
    int iRet = CLogoutHandler::LogoutRole(pRoleObj, LOGOUT_REASON_KICKOFF);
    if (iRet < 0)
    {
        LOGERROR("KickRole Failed: Uin = %u\n", uiKickedUin);
        
        SendFailedResponse(rstReq);
        return iRet;
    }
     
    pRoleObj->SetKicker(rstReq);

    return iRet;
}

int CKickRoleWorldHandler::OnForbidKickRole()
{
	return 0;

	/*
	FORBIDROLEREQUEST &rstReq = m_pRequestMsg->m_stMsgBody.m_stForbidRoleRequest; 
	ROLEID stRoleID = rstReq.m_stRoleID;
	
	CGameRoleObj* pRoleObj = CUnitUtility::GetRoleByUin(stRoleID.m_uiUin);
	LOGDEBUG("Recv Forbid Kick Role");

	if (pRoleObj == NULL)
	{
		TRACESVR("not online\n");
		return 0;
	}
	
	if (pRoleObj->GetRoleID().m_uiSeq != stRoleID.m_uiSeq)
	{
		TRACESVR("not online\n");
		return 0;
	}

	//todo，给封号提示
	CPromptMsgHelper::SendPromptMsgToRole(pRoleObj, 
			EQEC_Kick_Reason, PROMPTMSG_TYPE_POPUP, rstReq.m_szForbidReason);
	
	//设置T人时间
	pRoleObj->SetForbidTime(CalcForbidTime(rstReq.m_cForbidType, rstReq.m_cIsForbidden));

    // 将玩家踢下线 
    int iRet = CLogoutHandler::LogoutRole(pRoleObj, EQEC_Logout_Kickoff);
    if (iRet < 0)
    {
        TRACESVR("KickRole Failed: Uin = %u\n", stRoleID.m_uiUin);
    }

	return iRet;
	*/
}

// 收到World的踢人下线回复
int CKickRoleWorldHandler::OnResponseKickRole()
{
	World_KickRole_Response* pstKickRoleResp = m_pRequestMsg->mutable_m_stmsgbody()->mutable_m_stworld_kickrole_response();
	unsigned int uiUin = pstKickRoleResp->stkickedroleid().uin();

	LOGDEBUG("Recv KickRole Resp: Uin = %u, iRet = %d\n", uiUin, pstKickRoleResp->iresult());

	if (pstKickRoleResp->ifromworldid() != CModuleHelper::GetWorldID()
		|| pstKickRoleResp->ifromzoneid() != CModuleHelper::GetZoneID())
	{
		LOGERROR("Not From Mine!\n");
		return -1;
	}

    // 找到Session
    CGameSessionObj *pSession = CModuleHelper::GetSessionManager()->FindSessionByID(pstKickRoleResp->isessionid());
    if (!pSession)
    {
		LOGERROR("Cannot find Session: ID = %d\n", pstKickRoleResp->isessionid());
        return -1;
    }

	// 确认Session未登录
	if (pSession->GetBindingRole() != NULL)
	{
		LOGERROR("Session Already Binding Role!\n");
		return -2;
	}

    if (pstKickRoleResp->iresult() != T_SERVER_SUCESS)
    {
		if (pstKickRoleResp->bislogin())
		{
            //如果是登录
			CLoginHandler::LoginFailed(pSession->GetNetHead());
			CModuleHelper::GetSessionManager()->DeleteSession(pSession->GetID());
		}

		LOGERROR("Kick fail: %u, %d\n", uiUin, pstKickRoleResp->iresult());
        return -3;
    }

    // 如果踢人成功, 则再次确认Zone没有数据
    CGameRoleObj* pRoleObj = CUnitUtility::GetRoleByUin(uiUin);
    if (pRoleObj)
    {
		if (pstKickRoleResp->bislogin())
		{
			CLoginHandler::LoginFailed(pSession->GetNetHead());
			CModuleHelper::GetSessionManager()->DeleteSession(pSession->GetID());
		}

		LOGERROR("Role Already Login!\n");
		return -4;
	}

	//如果是登录则重新拉取数据
	if(pstKickRoleResp->bislogin())
	{
		int iRet = FetchRoleFromWorldServer(pSession->GetRoleID(), 0);
		if (iRet < 0)
		{
			CLoginHandler::LoginFailed(pSession->GetNetHead());
			CModuleHelper::GetSessionManager()->DeleteSession(pSession->GetID());
		}
	}

	LOGDEBUG("Recv KickRole Ok: Uin = %u\n", uiUin);

    return 0;
}

int CKickRoleWorldHandler::FetchRoleFromWorldServer(const RoleID& stRoleID, char cEnterType)
{
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_WORLD_FETCHROLE_REQUEST);
	ms_stZoneMsg.mutable_m_stmsghead()->set_m_uin(stRoleID.uin());

	World_FetchRole_Request* pstFetchRequest = ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stworld_fetchrole_request();

	pstFetchRequest->mutable_stroleid()->CopyFrom(stRoleID);
	pstFetchRequest->set_bislogin(true);

	pstFetchRequest->set_ireqid(CModuleHelper::GetZoneID());

    LOGDETAIL("Send FetchRole Request: Uin = %u\n", stRoleID.uin());

    int iRet = CZoneMsgHelper::SendZoneMsgToWorld(ms_stZoneMsg);

    return iRet;
}

// 返回成功回复
int CKickRoleWorldHandler::SendSuccessfullResponse(const World_KickRole_Request& rstKicker)
{
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_WORLD_KICKROLE_RESPONSE);
    World_KickRole_Response* pstResponse = ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stworld_kickrole_response();

	pstResponse->set_ifromworldid(rstKicker.ifromworldid());
	pstResponse->set_ifromzoneid(rstKicker.ifromzoneid());
	pstResponse->set_isessionid(rstKicker.isessionid());
	pstResponse->mutable_stkickedroleid()->CopyFrom(rstKicker.stkickedroleid());
	pstResponse->set_bislogin(rstKicker.bislogin());
	pstResponse->set_iresult(T_SERVER_SUCESS);
	
    CZoneMsgHelper::SendZoneMsgToWorld(ms_stZoneMsg);

    return 0;
}

// 返回失败回复
int CKickRoleWorldHandler::SendFailedResponse(const World_KickRole_Request& rstKicker)
{
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_WORLD_KICKROLE_RESPONSE);
    World_KickRole_Response* pstResponse = ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stworld_kickrole_response();

	pstResponse->set_ifromworldid(rstKicker.ifromworldid());
	pstResponse->set_ifromzoneid(rstKicker.ifromzoneid());
	pstResponse->set_isessionid(rstKicker.isessionid());
	pstResponse->mutable_stkickedroleid()->CopyFrom(rstKicker.stkickedroleid());
	pstResponse->set_bislogin(rstKicker.bislogin());
	pstResponse->set_iresult(T_ZONE_LOGINSERVER_FAILED);

    CZoneMsgHelper::SendZoneMsgToWorld(ms_stZoneMsg);

    return 0;
}
