#ifndef __ACCOUNTDB_THREAD_MANAGER_HPP__
#define __ACCOUNTDB_THREAD_MANAGER_HPP__

#include <string>

#include "GameConfigDefine.hpp"

#include "AccountDBThread.hpp"

class CAccountDBThreadManager
{
public:
	int Initialize(bool bResume);
	//push code to thread
	int PushCode(unsigned int uHashValue, const unsigned char* pMsg, int iCodeLength);

	//push code to thread
	int PushCodeToThread(const unsigned int iThreadIdx, const unsigned char* pMsg, int iCodeLength);

	//pop code
	int PopCode(const int iThreadIdx, unsigned char* pMsg, int iMaxLength, int& riLength);
	
public:
	CAccountDBThread* GetThread(int iThreadIdx);

public:
	static const int THREAD_NUMER = GameConfig::ACCOUNT_TABLE_SPLIT_FACTOR;

private:
	CAccountDBThread m_aThreads[CAccountDBThreadManager::THREAD_NUMER];
};


#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
