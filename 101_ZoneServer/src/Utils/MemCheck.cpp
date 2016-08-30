#include "GameProtocol.hpp"
#include "MemCheck.hpp"
#include "ConfigHelper.hpp"
#include "GameRole.hpp"
#include "ZoneObjectHelper.hpp"


int CMemCheck::CheckRoleException(unsigned int uiMsgID, int iLineNum)
{
	int iCheckID = 0;
	int iRoleIdx = GameTypeK32<CGameRoleObj>::GetUsedHead();
	while (iRoleIdx >= 0)
	{
		CGameRoleObj* pRoleObj = GameTypeK32<CGameRoleObj>::GetByIdx(iRoleIdx);
		if (!pRoleObj || !pRoleObj->IsOnline())
		{
			iRoleIdx = GameTypeK32<CGameRoleObj>::GetNextIdx(iRoleIdx);
			continue;
		}

		int iObjID = pRoleObj->GetObjectID();
		if (iObjID != iRoleIdx || iObjID > MAX_ROLE_OBJECT_NUMBER_IN_ZONE)
		{
			TRACESVR("Error: uin:%u, idx:%d iObjID:%d, msgid:%d lineNo:%d \n", 
				pRoleObj->GetUin(), iRoleIdx, iObjID, uiMsgID, iLineNum);
			iCheckID = -3;
		}

		iRoleIdx = GameTypeK32<CGameRoleObj>::GetNextIdx(iRoleIdx);
	}
	return iCheckID;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
