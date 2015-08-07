#ifndef __ROLEDBTHREADMANAGER_HPP
#define __ROLEDBTHREADMANAGER_HPP

#include "GameConfigDefine.hpp"

#include "RoleDBThread.hpp"

class CRoleDBThreadManager
{
public:
	int Initialize(bool bResume);
	//push code to thread
	int PushCode(const unsigned int uiUin, const unsigned char* pMsg, int iCodeLength);

	//push code to thread
	int PushCodeToThread(const unsigned int iThreadIdx, const unsigned char* pMsg, int iCodeLength);

	//pop code
	int PopCode(const int iThreadIdx, unsigned char* pMsg, int iMaxLength, int& riLength);
	
public:
	CRoleDBThread* GetThread(int iThreadIdx);

public:
	static const int THREAD_NUMER = GameConfig::ROLE_TABLE_SPLIT_FACTOR;

private:
	CRoleDBThread m_aThreads[CRoleDBThreadManager::THREAD_NUMER];
};


#endif
