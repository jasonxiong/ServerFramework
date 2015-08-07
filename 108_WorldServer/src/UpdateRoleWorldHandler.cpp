#include <string.h>

#include "ProtoDataUtility.hpp"
#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "LogoutHandler.hpp"
#include "LogAdapter.hpp"

#include "UpdateRoleWorldHandler.hpp"

using namespace ServerLib;

CUpdateRoleWorldHandler::~CUpdateRoleWorldHandler()
{
}

int CUpdateRoleWorldHandler::OnClientMsg(const void* pMsg)
{
	ASSERT_AND_LOG_RTN_INT(pMsg);

	m_pMsg = (GameProtocolMsg*)pMsg;

	switch (m_pMsg->m_stmsghead().m_uimsgid())
	{
	case MSGID_WORLD_UPDATEROLE_REQUEST:
		{
			OnRequestUpdateRoleWorld();
			break;
		}

	case MSGID_WORLD_UPDATEROLE_RESPONSE:
		{
			OnResponseUpdateRoleWorld();
			break;
		}
	default:
		{
			break;
		}
	}

	return 0;
}

int CUpdateRoleWorldHandler::OnRequestUpdateRoleWorld()
{
	World_UpdateRole_Request* pstReq = m_pMsg->mutable_m_stmsgbody()->mutable_m_stworld_updaterole_request();
	unsigned uiUin = pstReq->stroleid().uin();

    CWorldRoleStatusWObj* pWorldRoleStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(pstReq->stroleid());
	if (!pWorldRoleStatusObj)
	{
		// World没有cache，重建角色信息
		pWorldRoleStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::CreateByUin(uiUin);
		if (!pWorldRoleStatusObj)
		{
			LOGERROR("CreateRoleStatusObj Failed: Uin = %u.\n", uiUin);
			SendUpdateRoleResponseWGS(pstReq->stroleid(), pstReq->ireqid(), T_WORLD_SERVER_BUSY);
			return -1;
		}

		pWorldRoleStatusObj->SetRoleID(pstReq->stroleid());
		pWorldRoleStatusObj->SetZoneID(pstReq->ireqid());

		BASEDBINFO stBaseInfo;
		if(!DecodeProtoData(pstReq->stuserinfo().strbaseinfo(), stBaseInfo))
		{
			LOGERROR("Failed to decode base proto data, uin %u\n", uiUin);
			return -3;
		}

		unsigned int uStatus = stBaseInfo.ustatus();
		if(pstReq->bneedresponse())
		{
			uStatus &= ~(EGUS_ONLINE);
		}
		else
		{
			uStatus |= EGUS_ONLINE;
		}

		stBaseInfo.set_ustatus(uStatus);
		if(!EncodeProtoData(stBaseInfo, *pstReq->mutable_stuserinfo()->mutable_strbaseinfo()))
		{
			LOGERROR("Failed to encode proto data, uin %u\n", uiUin);
			return -5;
		}

		//更新缓存信息
		pWorldRoleStatusObj->SetRoleInfo(pstReq->stuserinfo());
	}

    LOGDEBUG("UpdateRole: Uin = %u\n", uiUin);

    // 发送更新请求到RoleDB
    int iRet = CWorldMsgHelper::SendWorldMsgToRoleDB(*m_pMsg);
    if(iRet < 0)
    {
        LOGERROR("UpdateRoleWorld Failed: iRet = %d\n", iRet);

        if (pstReq->bneedresponse())
        {
            int iZoneID = pWorldRoleStatusObj->GetZoneID();
            SendUpdateRoleResponseWGS(pstReq->stroleid(), iZoneID, T_WORLD_SERVER_BUSY);
        }

        return -2;
    }

    return 0;
}

//更新数据返回的处理入口
int CUpdateRoleWorldHandler::OnResponseUpdateRoleWorld()
{
	//直接转发给Game Server
	const World_UpdateRole_Response& rstResp = m_pMsg->m_stmsgbody().m_stworld_updaterole_response();
	CWorldRoleStatusWObj* pstUserStatus = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(rstResp.stroleid());
	if(!pstUserStatus)
	{
		LOGERROR("Fail to get world user status, uin %u\n", rstResp.stroleid().uin());
		return -1;
	}

	int iRet = SendUpdateRoleResponseWGS(rstResp.stroleid(), pstUserStatus->GetZoneID(), rstResp.iresult());

	return iRet;
}

int CUpdateRoleWorldHandler::SendUpdateRoleResponseWGS(const RoleID& stRoleID, int iZoneID, unsigned int uiResultID)
{
    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, 0, MSGID_WORLD_UPDATEROLE_RESPONSE, stRoleID.uin());

	World_UpdateRole_Response* pstResp = m_stWorldMsg.mutable_m_stmsgbody()->mutable_m_stworld_updaterole_response();
	pstResp->mutable_stroleid()->CopyFrom(stRoleID);
	pstResp->set_iresult(uiResultID);
    
    CWorldMsgHelper::SendWorldMsgToWGS(m_stWorldMsg, iZoneID);
	return 0;
}




