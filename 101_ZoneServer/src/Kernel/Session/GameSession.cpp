
#include "GameProtocol.hpp"
#include "GameRole.hpp"
#include "ModuleHelper.hpp"
#include "SocketUtility.hpp"
#include "ZoneObjectHelper.hpp"

#include "GameSession.hpp"

IMPLEMENT_DYN(CGameSessionObj)

CGameSessionObj::CGameSessionObj()
{
    
}

CGameSessionObj::~CGameSessionObj()
{

}

int CGameSessionObj::Initialize()
{
    m_iCreateTime = time(NULL);
    m_iRoleIdx = -1;
        
	return 0;
}

void CGameSessionObj::SetNetHead(const TNetHead_V2* pNetHead)
{
	m_stNetHead = *pNetHead;
	
	CSocketUtility::IPInt32ToString(ntohl(pNetHead->m_uiSrcIP), m_szClientIP);
}

CGameRoleObj* CGameSessionObj::GetBindingRole()
{
    return GameTypeK32<CGameRoleObj>::GetByIdx(m_iRoleIdx);
}

void CGameSessionObj::SetBindingRole(const CGameRoleObj* pZoneRoleObj)
{
    m_iRoleIdx = pZoneRoleObj->GetObjectID();
}

void CGameSessionObj::SetSessionKey(const char* pszSessionKey)
{
	SAFE_STRCPY(m_szSessionKey, pszSessionKey, sizeof(m_szSessionKey));
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
