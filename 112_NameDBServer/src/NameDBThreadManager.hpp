#ifndef __NAMEDB_THREAD_MANAGER_HPP__
#define __NAMEDB_THREAD_MANAGER_HPP__

#include <string>

#include "GameConfigDefine.hpp"

#include "NameDBThread.hpp"

class CNameDBThreadManager
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
	CNameDBThread* GetThread(int iThreadIdx);

public:
	static const int THREAD_NUMER = GameConfig::NAME_TABLE_SPLIT_FACTOR;

private:
	CNameDBThread m_aThreads[CNameDBThreadManager::THREAD_NUMER];
};


#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
