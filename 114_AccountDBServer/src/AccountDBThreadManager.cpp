
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "HashUtility.hpp"

#include "AccountDBThreadManager.hpp"

int CAccountDBThreadManager::Initialize(bool bResume)
{
	for (int i = 0; i < THREAD_NUMER; i++)
	{
		int iRt = 0;

		//create all threads
		iRt = m_aThreads[i].Initialize(bResume, i);

		if (iRt != 0)
		{
			TRACESVR("Error: init thread %d fail. rt:%d\n", i, iRt);
			return -1;
		}
	}

	return 0;
}

CAccountDBThread* CAccountDBThreadManager::GetThread(int iThreadIdx)
{
	if (iThreadIdx < 0 || iThreadIdx >= THREAD_NUMER)
	{
		return NULL;
	}

	return &m_aThreads[iThreadIdx];
}

int CAccountDBThreadManager::PushCode(unsigned int uHashValue, const unsigned char* pMsg, int iCodeLength)
{
	if (!pMsg)
	{
		return -1;
	}

	int iThreadIdx = uHashValue % ACCOUNT_TABLE_SPLIT_FACTOR;
	return m_aThreads[iThreadIdx].PushCode(pMsg, iCodeLength);
}

//push code to thread
int CAccountDBThreadManager::PushCodeToThread(const unsigned int iThreadIdx, const unsigned char* pMsg, int iCodeLength)
{
	if (!pMsg)
	{
		return -1;
	}

	return m_aThreads[iThreadIdx].PushCode(pMsg, iCodeLength);
}

int CAccountDBThreadManager::PopCode(const int iThreadIdx, unsigned char* pMsg, int iMaxLength, int& riLength)
{
	if (!pMsg)
	{
		return -1;
	}

	if (iThreadIdx < 0 || iThreadIdx >= CAccountDBThreadManager::THREAD_NUMER)
	{
		return -3;
	}

	return m_aThreads[iThreadIdx].PopCode(pMsg, iMaxLength, riLength);
}
	
		
