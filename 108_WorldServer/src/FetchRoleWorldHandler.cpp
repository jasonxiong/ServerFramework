#include <string.h>

#include "ProtoDataUtility.hpp"
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "FetchRoleWorldHandler.hpp"
#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"

using namespace ServerLib;

CFetchRoleWorldHandler::~CFetchRoleWorldHandler()
{
}

int CFetchRoleWorldHandler::OnClientMsg(const void* pMsg)
{
	ASSERT_AND_LOG_RTN_INT(pMsg);

	m_pMsg = (GameProtocolMsg*)pMsg;

	switch (m_pMsg->m_stmsghead().m_uimsgid())
	{
	case MSGID_WORLD_FETCHROLE_REQUEST:
		{
			OnRequestFetchRoleWorld();
			break;
		}
    case MSGID_WORLD_FETCHROLE_RESPONSE:
        {
            OnResponseFetchRoleWorld();
            break;
        }
	default:
		{
			break;
		}
	}

	return 0;
}

int CFetchRoleWorldHandler::OnRequestFetchRoleWorld()
{
    const World_FetchRole_Request& rstRequest = m_pMsg->m_stmsgbody().m_stworld_fetchrole_request();

    unsigned uiUin = rstRequest.stroleid().uin();
	if (uiUin == 0)
	{
		SendFailedFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());
		return -1;
	}
    
    LOGDEBUG("FetchRoleWorld: Uin = %d\n", uiUin);

    CWorldRoleStatusWObj* pWorldRoleStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(rstRequest.stroleid());
    // 如果角色对象已经存在, 并且是登录请求, 则返回失败
    if (pWorldRoleStatusObj && rstRequest.bislogin())
    {
        LOGERROR("Role Already Login: Uin = %d.\n", uiUin);
        SendFailedFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());
        return -2;
    }

    // 如果不是登录请求，并且要查询的角色不存在，则返回失败
    if (!pWorldRoleStatusObj && !rstRequest.bislogin())
    {
        LOGERROR("Role Non-Exists: Uin = %u.\n", uiUin);
        SendFailedFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());
        return -3;
    }
    
    // 如果是登录或者读取存档请求, 创建角色对象并查询角色信息
    if (rstRequest.bislogin())
    {
        //登录请求需要重新创建缓存Obj
        pWorldRoleStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::CreateByUin(uiUin);
        if (!pWorldRoleStatusObj)
        {
            LOGERROR("CreateRoleStatusObj Failed: Uin = %u.\n", uiUin);
            SendFailedFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());
            return -4;
        }

        pWorldRoleStatusObj->SetZoneID(rstRequest.ireqid());
        pWorldRoleStatusObj->SetRoleID(rstRequest.stroleid());

        //todo jasonxiong2 暂时不需要EnterType，等后续需要时再添加
        pWorldRoleStatusObj->SetEnterType(0);

        // 从数据库中拉取数据
        int iRet = CWorldMsgHelper::SendWorldMsgToRoleDB(*m_pMsg);
        if (iRet < 0)
        {
            LOGERROR("SendFetchRoleDB Failed: Uin = %d, iRet = %d\n", uiUin, iRet);

            SendFailedFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());
            WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);
            return -6;
        }

        return 0;
    }
    
    // 否则直接返回角色数据
    SendFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());

	return 0;
}

//查询请求返回到World的处理入口
int CFetchRoleWorldHandler::OnResponseFetchRoleWorld()
{
    World_FetchRole_Response* pstResp = m_pMsg->mutable_m_stmsgbody()->mutable_m_stworld_fetchrole_response();
    unsigned uiUin = pstResp->stroleid().uin();

    //查找World缓存的本地玩家数据
    CWorldRoleStatusWObj* pUserStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(pstResp->stroleid());
    if(!pUserStatusObj)
    {
        LOGERROR("World No Cache Data: Uin = %u\n", uiUin);
        return -1;
    }

    int iZoneID = pUserStatusObj->GetZoneID();

    //如果是登录，验证玩家的不在线状态
    if(pstResp->bislogin() && (pUserStatusObj->GetRoleStatus() & EGUS_ONLINE))
    {
        LOGERROR("Fail to fetch role from DB, already online, uin %u\n", uiUin);
        
        WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);
        SendFailedFetchRoleResponse(pstResp->stroleid(), iZoneID);
        return -2;
    }

    //如果从DB下拉数据失败
    if(pstResp->iresult())
    {
        LOGERROR("Fail to fetch role from DB, uin %u, ret 0x%0x\n", uiUin, pstResp->iresult());

        WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);
        SendFailedFetchRoleResponse(pstResp->stroleid(), iZoneID);
        return -3;
    }

    LOGDEBUG("Fetch ROLEDB OK, uin %u, from zone id %d\n", uiUin, iZoneID);

    //更新World的玩家缓存数据信息
    BASEDBINFO stBaseInfo;
    if(!DecodeProtoData(pstResp->stuserinfo().strbaseinfo(), stBaseInfo))
    {
        LOGERROR("Failed to decode base proto data, uin %u\n", uiUin);
        return -4;
    }

    unsigned int uStatus = stBaseInfo.ustatus();
    uStatus = uStatus|EGUS_ONLINE;

    stBaseInfo.set_ustatus(uStatus);
    if(!EncodeProtoData(stBaseInfo, *pstResp->mutable_stuserinfo()->mutable_strbaseinfo()))
    {
        LOGERROR("Failed to encode base proto data, uin %u\n", uiUin);
        return -5;
    }

    pUserStatusObj->SetRoleInfo(pstResp->stuserinfo());
    pUserStatusObj->SetRoleStatus(uStatus);

    //返回查询结果
    int iRet = CWorldMsgHelper::SendWorldMsgToWGS(*m_pMsg, iZoneID);
    if(iRet)
    {
        LOGERROR("SendFetchRoleResponse to GS failed, uin %u, zone id %d\n", uiUin, iZoneID);

        WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);
        SendFailedFetchRoleResponse(pstResp->stroleid(), iZoneID); 
    }

    return iRet;
}

// 返回失败信息
int CFetchRoleWorldHandler::SendFailedFetchRoleResponse(const RoleID& stRoleID, int iReqID)
{
    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, 0, MSGID_WORLD_FETCHROLE_RESPONSE, stRoleID.uin());
    
    World_FetchRole_Response* rstResp = m_stWorldMsg.mutable_m_stmsgbody()->mutable_m_stworld_fetchrole_response();
    rstResp->mutable_stroleid()->CopyFrom(stRoleID);
    rstResp->set_iresult(T_WORLD_FETCHROLE_FAIED);
	
    int iRet = CWorldMsgHelper::SendWorldMsgToWGS(m_stWorldMsg, iReqID);

	return iRet;
}

// 返回角色数据
int CFetchRoleWorldHandler::SendFetchRoleResponse(const RoleID& stRoleID, int iReqID)
{
    CWorldRoleStatusWObj* pWorldRoleStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(stRoleID);
    if (!pWorldRoleStatusObj)
    {
        return -1;
    }

    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, 0, MSGID_WORLD_FETCHROLE_RESPONSE, stRoleID.uin());

    World_FetchRole_Response* pstResp = m_stWorldMsg.mutable_m_stmsgbody()->mutable_m_stworld_fetchrole_response();
    pstResp->set_iresult(T_SERVER_SUCESS);
    pstResp->mutable_stroleid()->CopyFrom(stRoleID);

    pstResp->mutable_stuserinfo()->CopyFrom(pWorldRoleStatusObj->GetRoleInfo());

    int iRet = CWorldMsgHelper::SendWorldMsgToWGS(m_stWorldMsg, iReqID);

    return iRet;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
