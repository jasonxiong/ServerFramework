#include "RoleDBLogManager.hpp"
#include "StringUtility.hpp"

int CRoleDBLogManager::Initialize(const int iLogNum, const TLogConfig& rLogConfig)
{
	if (iLogNum <= 0)
	{
		return -1;
	}

	m_iLogNum = iLogNum;
	m_pLogAdapter = new CServerLogAdapter[iLogNum];
	if (!m_pLogAdapter)
	{
		return -3;
	}

	char szBaseName[sizeof(rLogConfig.m_szBaseName)];
	for (int i = 0; i < iLogNum; i++)
	{
		TLogConfig stLogConfig = rLogConfig;
		SAFE_STRCPY(szBaseName, stLogConfig.m_szBaseName, sizeof(stLogConfig.m_szBaseName));
		//加上线程的index到日志名字的末尾
		SAFE_SPRINTF(stLogConfig.m_szBaseName, sizeof(stLogConfig.m_szBaseName), "%s%d", szBaseName, i);
		m_pLogAdapter[i].ReloadLogConfig(stLogConfig);
	}
		
	return 0;
}


CServerLogAdapter* CRoleDBLogManager::GetLogAdapter(const int iThreadIdx)
{
	if (iThreadIdx < 0 || iThreadIdx >= m_iLogNum)
	{
		return NULL;
	}

	return &m_pLogAdapter[iThreadIdx];
}
		

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
