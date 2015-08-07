#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "ZoneObjectHelper.hpp"

#include "SessionManager.hpp"

int CSessionManager::Initialize(bool bResume)
{
    return 0;
}

CGameSessionObj* CSessionManager::CreateSession(TNetHead_V2* pNetHead, const RoleID& stRoleID)
{
	if(!pNetHead)
	{
		return NULL;
	}

    if (GameTypeK32<CGameSessionObj>::GetUsedObjNumber() >= MAX_GAMESESSION_NUMBER)
    {
        return NULL;
    }

    int iSessionID = ntohl(pNetHead->m_uiSocketFD);

    // 检查是否已经Active
    if (FindSessionByID(iSessionID))
    {
        return NULL;
    }

    // 分配新的Session
    CGameSessionObj* pSession = GameTypeK32<CGameSessionObj>::CreateByKey(iSessionID);
    if (pSession)
    {
        LOGDETAIL("CreateSession: ID = %d, uin %u\n", iSessionID, stRoleID.uin());

        pSession->SetID(iSessionID);
        pSession->SetRoleID(stRoleID);
        pSession->SetNetHead(pNetHead);
    }

    return pSession;
}

void CSessionManager::DeleteSession(int iSessionID)
{
    GameTypeK32<CGameSessionObj>::DeleteByKey(iSessionID);

    LOGDETAIL("DeleteSession: ID = %d\n", iSessionID);
}

CGameSessionObj* CSessionManager::FindSessionByID(const int iSessionID)
{
    return (CGameSessionObj*)GameTypeK32<CGameSessionObj>::GetByKey(iSessionID);
}

CGameSessionObj* CSessionManager::FindSessionByRoleID(const RoleID& stRoleID)
{
    int iIdx = GameTypeK32<CGameSessionObj>::GetUsedHead();
    while (iIdx >= 0)
    {
        CGameSessionObj* pSession = GameTypeK32<CGameSessionObj>::GetByIdx(iIdx);
        if (pSession && (pSession->GetRoleID().uin()==stRoleID.uin()) && (pSession->GetRoleID().uiseq()==stRoleID.uiseq()))
        {
            return pSession;
        }

        iIdx = GameTypeK32<CGameSessionObj>::GetNextIdx(iIdx);
    }

    return NULL;
}
