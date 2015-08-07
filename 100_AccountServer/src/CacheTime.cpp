#include <string.h>
#include "CacheTime.hpp"

CCacheTime::CCacheTime()
{
}

void CCacheTime::SetCreatedTime(const time_t* pstTime)
{
	if(!pstTime)
	{
		return;
	}

    m_stCreatedTime = *pstTime;
}

time_t* CCacheTime::GetCreatedTime()
{
    return &m_stCreatedTime;
}

void CCacheTime::GetCreatedTime(char* pszTime, unsigned int uiBufLen)
{
	if(!pszTime)
	{
		return;
	}	

    strncpy(pszTime, ctime(GetCreatedTime()), uiBufLen - 1);
}
