#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include "SessionObj.hpp"

CSessionObj::CSessionObj()
{
}

CSessionObj::~CSessionObj()
{

}

int CSessionObj::Create(TNetHead_V2& stNetHead, const unsigned short unValue)
{
    memcpy(&m_stNetHead, &stNetHead, sizeof(TNetHead_V2));
    m_unValue = unValue;
    SetActive(true);

    return 0;
}

TNetHead_V2* CSessionObj::GetNetHead()
{
    return &m_stNetHead;
}

unsigned short CSessionObj::GetValue() const
{
    return m_unValue;
}

unsigned int CSessionObj::GetSessionFD() const
{
    return ntohl(m_stNetHead.m_uiSocketFD);
}

void CSessionObj::SetActive(bool bActive)
{
    m_bActive = bActive;
}

bool CSessionObj::IsActive()
{
    return m_bActive;
}

void CSessionObj::GetSockCreatedTime(char* pszTime, unsigned int uiBufLen)
{
	if(!pszTime)
	{
		return;
	}

    unsigned int uiTime = ntohl(m_stNetHead.m_uiSocketTime);
    strncpy(pszTime, ctime((time_t*)&uiTime), uiBufLen - 1);
}

void CSessionObj::GetMsgCreatedTime(char* pszTime, unsigned int uiBufLen)
{
	if(!pszTime)
	{
		return;
	}

    unsigned int uiTime = ntohl(m_stNetHead.m_uiCodeTime);
    strncpy(pszTime, ctime((time_t*)&uiTime), uiBufLen - 1);
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
