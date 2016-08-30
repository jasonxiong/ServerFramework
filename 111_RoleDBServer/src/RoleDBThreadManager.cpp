#include "RoleDBThreadManager.hpp"
#include "LogAdapter.hpp"


int CRoleDBThreadManager::Initialize(bool bResume)
{
	for (int i = 0; i < THREAD_NUMER; i++)
	{
		int iRt = 0;
		//create all threads
		iRt = m_aThreads[i].Initialize(bResume, i);

		if (iRt != 0)
		{
			LOGERROR("Error: init thread %d fail. rt:%d\n", i, iRt);
			return -1;
		}
	}

	return 0;
}

CRoleDBThread* CRoleDBThreadManager::GetThread(int iThreadIdx)
{
	if (iThreadIdx < 0 || iThreadIdx >= THREAD_NUMER)
	{
		return NULL;
	}

	return &m_aThreads[iThreadIdx];
}

int CRoleDBThreadManager::PushCode(const unsigned int uiUin, const unsigned char* pMsg, int iCodeLength)
{
	if (!pMsg)
	{
		return -1;
	}

	int iThreadIdx = uiUin % ROLE_TABLE_SPLIT_FACTOR;
	return m_aThreads[iThreadIdx].PushCode(pMsg, iCodeLength);
}

//push code to thread
int CRoleDBThreadManager::PushCodeToThread(const unsigned int iThreadIdx, const unsigned char* pMsg, int iCodeLength)
{
	if (!pMsg)
	{
		return -1;
	}

	return m_aThreads[iThreadIdx].PushCode(pMsg, iCodeLength);
}

int CRoleDBThreadManager::PopCode(const int iThreadIdx, unsigned char* pMsg, int iMaxLength, int& riLength)
{
	if (!pMsg)
	{
		return -1;
	}

	if (iThreadIdx < 0 || iThreadIdx >= CRoleDBThreadManager::THREAD_NUMER)
	{
		return -3;
	}

	return m_aThreads[iThreadIdx].PopCode(pMsg, iMaxLength, riLength);
}
	
		

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
