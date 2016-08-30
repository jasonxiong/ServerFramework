#include "GameProtocol.hpp"
#include "GameRole.hpp"
#include "ModuleHelper.hpp"
#include "ZoneMsgHelper.hpp"
#include "ZoneObjectHelper.hpp"
#include "FightUnitObj.hpp"
#include "ZoneOssLog.hpp"
#include "ConfigManager.hpp"

#include "FightUnitHandler.hpp"

using namespace GameConfig;

GameProtocolMsg CFightUnitHandler::ms_stZoneMsg;

int CFightUnitHandler::OnClientMsg()
{
	int iRet = securityCheck();
	if (iRet<0) 
	{
		return iRet;
	}

	switch(m_pRequestMsg->m_stmsghead().m_uimsgid()) 
	{
		//todo jasonxiong4 增加接口

	default:
		{
			break;
		}

	}
	return 0;
}

int CFightUnitHandler::securityCheck()
{
	int iRet = SecurityCheck();
	if(iRet < 0)
	{
	    LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);

		//SendFailedResponse(MSGID_ZONE_SELLITEM_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

	    return -1;
	}

	return 0;
}

void CFightUnitHandler::SendHandlerResponse(int iRespMsgID, int code)
{
	CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, iRespMsgID);
	switch(iRespMsgID)
	{
		//todo jasonxiong4 增加新接口

	default:
		{
			LOGERROR("Failed to send response, invalid resp %d\n", iRespMsgID);
			return;
		}
		break;
	}

	CZoneMsgHelper::SendZoneMsgToRole(ms_stZoneMsg, m_pRoleObj);

	return;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
