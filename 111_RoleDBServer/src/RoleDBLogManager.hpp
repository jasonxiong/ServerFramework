#ifndef __ROLEDBLOGMANAGER_HPP
#define __ROLEDBLOGMANAGER_HPP

#include "RoleDBThreadManager.hpp"
#include "LogAdapter.hpp"


#ifdef _QQGAME_NO_LOG_
#define TRACE_THREAD(iThreadIdx, format, ...)
#else
//!主控日志，无日志等级，强制打
#define TRACE_THREAD(iThreadIdx, format, ...) ((CRoleDBApp*)CAppType<CRoleDBApp>::GetApp())->GetLogAdapter(iThreadIdx)->m_stLogFile.WriteLogEx(LOG_LEVEL_ANY, ((CRoleDBApp*)CAppType<CRoleDBApp>::GetApp())->GetLogAdapter(iThreadIdx)->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)	
#endif																

#ifdef _QQGAME_NO_LOG_													
#define DEBUG_THREAD(iThreadIdx, format, ...)								
#else															
//!主控日志，DEBUG日志，LOG_LEVEL_DETAIL等级
#define DEBUG_THREAD(iThreadIdx, format, ...) ((CRoleDBApp*)CAppType<CRoleDBApp>::GetApp())->GetLogAdapter(iThreadIdx)->m_stLogFile.WriteLogEx(LOG_LEVEL_DETAIL, ((CRoleDBApp*)CAppType<CRoleDBApp>::GetApp())->GetLogAdapter(iThreadIdx)->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

class CRoleDBLogManager
{
public:
	int Initialize(const int iLogNum, const TLogConfig& rLogConfig);
	CServerLogAdapter* GetLogAdapter(const int iThreadIdx);
	
private:
	int m_iLogNum;
	CServerLogAdapter* m_pLogAdapter;
};


#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
