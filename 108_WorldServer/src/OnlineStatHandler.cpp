#include <string.h>

#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "ModuleHelper.hpp"
#include "LogAdapter.hpp"

#include "OnlineStatHandler.hpp"

COnlineStatHandler::~COnlineStatHandler()
{

}

int COnlineStatHandler::OnClientMsg(const void* pMsg)
{
    ASSERT_AND_LOG_RTN_INT(pMsg);

    m_pRequestMsg = (GameProtocolMsg*)pMsg;

    switch (m_pRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_ZONE_ONLINEROLENUM_REQUEST:
        {
            OnZoneOnlineStatReq();
            break;
        }

        /*
    case MSGID_WORLD_ONLINESTAT_RESPONSE:
        {
            OnWorldOnlineStatResp();
        }
        */

    default:
        {
            break;
        }
    }

    return 0;
}

int COnlineStatHandler::OnZoneOnlineStatReq()
{
    //TRACESVR("OnOnlineStatReq\n");

    const Zone_OnlineRole_Request& rstReq = m_pRequestMsg->m_stmsgbody().m_stzone_onlinerole_request();
    
    CModuleHelper::GetZoneTick()->ActiveRefresh(rstReq.izoneid(), rstReq.irolenum());

    return 0;
}

int COnlineStatHandler::OnWorldOnlineStatResp()
{   
    //CModuleHelper::GetClusterTick()->ActiveRefresh();

    return 0;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
