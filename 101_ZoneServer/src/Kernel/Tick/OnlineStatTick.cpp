#include "GameProtocol.hpp"
#include "ZoneObjectHelper.hpp"
#include "ZoneMsgHelper.hpp"
#include "ModuleHelper.hpp"
#include "UnixTime.hpp"
#include "OnlineStatTick.hpp"

using namespace ServerLib;

COnlineStatTick::COnlineStatTick()
{
}

int COnlineStatTick::Initialize(bool bResume)
{
    // 启动后立刻向World发送一条在线消息, World利用该消息作为Zone的心跳包
    SendZoneOnlineStat(!bResume);

    if (bResume)
    {
        return 0;
    }

    m_stLastTickTime = time(NULL);

    return 0;
}

int COnlineStatTick::OnTick()
{
    time_t stNow = time(NULL);
    int iSec = stNow - m_stLastTickTime;

    if (iSec >= ONLINE_STAT_INTERVAL)
    {
        SendZoneOnlineStat(false);
        m_stLastTickTime = stNow;
    }

    return 0;
}

int COnlineStatTick::SendZoneOnlineStat(bool bInit)
{
    CZoneMsgHelper::GenerateMsgHead(m_stMsg, MSGID_ZONE_ONLINEROLENUM_REQUEST);

    Zone_OnlineRole_Request* pstReq = m_stMsg.mutable_m_stmsgbody()->mutable_m_stzone_onlinerole_request();
    pstReq->set_iworldid(CModuleHelper::GetWorldID());
    pstReq->set_izoneid(CModuleHelper::GetZoneID());

	if (bInit)
	{
        pstReq->set_irolenum(-1);
	}
	else
	{   
        pstReq->set_irolenum(GameTypeK32<CGameRoleObj>::GetUsedObjNumber());
	}

    char szNow[] = "YYYY-MM-DD HH:MM:SS";
    CUnixTime now(time(NULL));
    now.GetTimeString(szNow);
    pstReq->set_strrecordtime(szNow);

    CZoneMsgHelper::SendZoneMsgToWorld(m_stMsg);

    return 0;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
