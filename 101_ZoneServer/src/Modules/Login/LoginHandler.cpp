#include <string.h>

#include "GameProtocol.hpp"
#include "ModuleHelper.hpp"
#include "ZoneObjectHelper.hpp"
#include "HandlerFactory.hpp"
#include "ZoneMsgHelper.hpp"
#include "AppLoop.hpp"
#include "HandlerHelper.hpp"
#include "ZoneOssLog.hpp"
#include "StringUtility.hpp"
#include "LogAdapter.hpp"
#include "UnitUtility.hpp"

#include "LoginHandler.hpp"

GameProtocolMsg CLoginHandler::ms_stZoneMsg;

CLoginHandler::~CLoginHandler()
{
}

int CLoginHandler::OnClientMsg()
{
	switch (m_pRequestMsg->m_stmsghead().m_uimsgid())
	{
	case MSGID_ZONE_LOGINSERVER_REQUEST:
		{
			OnRequestLoginServer();
			break;
		}

	default:
		{
			break;
		}
	}

	return 0;
}

int CLoginHandler::OnRequestLoginServer()
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    int iRet = SecurityCheck();
    if (iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);

        LoginFailed(*m_pNetHead);
        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    const Zone_LoginServer_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_loginserver_request();
    iRet = LoginRole(rstRequest.stroleid(), m_pNetHead, rstRequest);
    if (iRet < 0)
    {
        CHandlerHelper::SetErrorCode(T_ZONE_LOGINSERVER_FAILED);
        LoginFailed(*m_pNetHead);
    }

    return iRet;	
}

//通知角色登录
int CLoginHandler::NotifyRoleLogin(CGameRoleObj* pstRoleObj)
{
    /*
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_LOGINSERVER_NOTIFY);

    Zone_LoginServer_Notify* pstNotify = ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stloginserver_notify();
    pstNotify->mutable_stroleid()->CopyFrom(pstRoleObj->GetRoleID());
    pstNotify->set_iunitid(pstRoleObj->GetRoleInfo().stUnitInfo.iUnitID);
    pstNotify->set_izoneid(CModuleHelper::GetZoneID());

    //todo jasonxiong2 后面确认是否要添加通知
    //CZoneMsgHelper::SendZoneMsgToSight(ms_stZoneMsg, &(pstRoleObj->GetRoleInfo().stUnitInfo));
    */ 
     
    return 0;
}

int CLoginHandler::SecurityCheck()
{
    unsigned int uiUin = m_pRequestMsg->m_stmsghead().m_uin();
    if (uiUin == 0)
    {
        CHandlerHelper::SetErrorCode(T_ZONE_SYSTEM_PARA_ERR);
        return -1; //nolog
    }

    //todo jasonxiong 后面的开发中可能需要添加黑白名单
    /*
    CWhiteListConfig& rWhiteListConfig = 
        WhiteListCfgMgr();
    if (rWhiteListConfig.IsInBlackList(uiUin))
    {
        TRACESVR("uin: %u is in black list, can not login\n", uiUin);
        CHandlerHelper::SetErrorCode(EQEC_UinInBlackList);
        return -2;
    }
    */
	
    return 0;
}

int CLoginHandler::LoginRole(const RoleID& stRoleID, TNetHead_V2* pNetHead, const Zone_LoginServer_Request& rstRequest)
{
    ASSERT_AND_LOG_RTN_INT(pNetHead);

	int iSessionID = (int)ntohl(pNetHead->m_uiSocketFD);

	LOGDETAIL("Login Req: Uin = %u, Session = %d\n", stRoleID.uin(), iSessionID);

	// 该Session已经存在
	CSessionManager* pSessionManager = CModuleHelper::GetSessionManager();
	CGameSessionObj* pSessionObj = pSessionManager->FindSessionByID(iSessionID);
	if (pSessionObj)
	{
		LOGERROR("Session Already Exist: ID = %d\n", iSessionID);
		return -2;
	}

	// 该RoleID的Session已经存在，且处于未登录状态
	pSessionObj = pSessionManager->FindSessionByRoleID(stRoleID);
	if (pSessionObj && pSessionObj->GetBindingRole() == NULL)
	{
		LOGERROR("Session Alreay Binding Role: ID = %d, Uin = %u\n", iSessionID, stRoleID.uin());
		return -3;
	}

    // 创建一个新的会话
    pSessionObj = pSessionManager->CreateSession(pNetHead, stRoleID);
    if(!pSessionObj)
    {
        LOGERROR("Cannot Create Session: Uin = %u, SessionID = %d\n", stRoleID.uin(), iSessionID);  
        return -2;
    }

    // 保存客户端版本号
    pSessionObj->SetClientVersion(rstRequest.uclientversion());

	//保存登录原因
	pSessionObj->SetLoginReason(rstRequest.uloginreason());

    // 向World发送踢人请求,相同uin的都踢下去
    int iRet = KickRoleFromWorldServer(stRoleID, iSessionID);
    if (iRet < 0)
    {
        CModuleHelper::GetSessionManager()->DeleteSession(iSessionID);
    }

    return iRet;
}

int CLoginHandler::SendFailedResponse(const unsigned int uiResultID, const TNetHead_V2& rstNetHead)
{
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_LOGINSERVER_RESPONSE);

    Zone_LoginServer_Response* pstResp = ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_loginserver_response();
    pstResp->Clear();

    pstResp->set_iresult(uiResultID);

    CZoneMsgHelper::SendZoneMsgToClient(ms_stZoneMsg, rstNetHead);

    return 0;
}

int CLoginHandler::SendSuccessfulResponse(CGameRoleObj* pLoginRoleObj)
{
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_LOGINSERVER_RESPONSE);
    
    Zone_LoginServer_Response* pstLoginResp = ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_loginserver_response();
    pstLoginResp->set_iresult(T_SERVER_SUCESS);
    pstLoginResp->set_iworldid(CModuleHelper::GetWorldID());
    pstLoginResp->set_izoneid(CModuleHelper::GetZoneID());

    int iRet = T_SERVER_SUCESS;

    //返回玩家身上的基本信息
    BASEDBINFO* pstBaseInfo = pstLoginResp->mutable_stlogininfo()->mutable_stbaseinfo();
    pLoginRoleObj->UpdateBaseInfoToDB(*pstBaseInfo);

    //返回玩家的背包信息
    ITEMDBINFO* pstItemInfo = pstLoginResp->mutable_stlogininfo()->mutable_stiteminfo();
    pLoginRoleObj->UpdateRepThingsToDB(*pstItemInfo);

    //返回玩家的战斗单位信息
    FIGHTDBINFO* pstFightUnitInfo = pstLoginResp->mutable_stlogininfo()->mutable_stfightinfo();
    iRet = pLoginRoleObj->UpdateFightUnitToDB(*pstFightUnitInfo);
    if(iRet)
    {
        pLoginRoleObj->GetFightUnitManager().ClearFightUnitObj();
        return iRet;
    }

    CZoneMsgHelper::SendZoneMsgToRole(ms_stZoneMsg, pLoginRoleObj);

    return 0;
}

// 登录成功处理
int CLoginHandler::LoginOK(unsigned int uiUin, bool bNeedResponse)
{
    CGameRoleObj* pLoginRoleObj =  GameTypeK32<CGameRoleObj>::GetByKey(uiUin);
    ASSERT_AND_LOG_RTN_INT(pLoginRoleObj);

    if(bNeedResponse)
    {
        // 登录成功
        SendSuccessfulResponse(pLoginRoleObj);
    }

    // 激发登录事件
    CModuleHelper::GetUnitEventManager()->NotifyUnitLogin(&(pLoginRoleObj->GetRoleInfo().stUnitInfo));

    // 通知其他用户
    NotifyRoleLogin(pLoginRoleObj);

    LOGDEBUG("Uin: %u\n", uiUin);

    // 记录流水
    CZoneOssLog::TraceLogin(*pLoginRoleObj);

    return 0;
}

// 登录失败处理
int CLoginHandler::LoginFailed(const TNetHead_V2& rstNetHead)
{ 
	unsigned int uiResultID = CHandlerHelper::GetErrorCode();
	SendFailedResponse(uiResultID, rstNetHead);

    return 0;
}

//通过World将相同uin的已经登录的号踢下线
int CLoginHandler::KickRoleFromWorldServer(const RoleID& stRoleID, int iFromSessionID)
{
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_WORLD_KICKROLE_REQUEST);

    World_KickRole_Request* pstReq = ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stworld_kickrole_request();

    pstReq->set_ifromworldid(CModuleHelper::GetWorldID());
    pstReq->set_ifromzoneid(CModuleHelper::GetZoneID());
    pstReq->set_isessionid(iFromSessionID);
    pstReq->mutable_stkickedroleid()->CopyFrom(stRoleID);
    pstReq->set_bislogin((iFromSessionID>0));

    LOGDEBUG("Send KickRole Request: Uin = %u\n", stRoleID.uin());

    int iRet = CZoneMsgHelper::SendZoneMsgToWorld(ms_stZoneMsg);

    return iRet;
}
