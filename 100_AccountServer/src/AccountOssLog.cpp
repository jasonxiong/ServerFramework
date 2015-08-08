#include <time.h>

#include "GameProtocol.hpp"
#include "TimeUtility.hpp"
#include "ConfigMgr.hpp"
#include "StringUtility.hpp"
#include "SessionObj.hpp"
#include "AccountSingleton.hpp"

#include "AccountOssLog.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//运营OSS日志接口

void CAccountOssLog::TraceCreateAccount(unsigned int uin, unsigned int uiSessionFd)
{
	CSessionObj* pstSessionObj = SessionManager->GetSession(uiSessionFd);
	if(!pstSessionObj)
	{
		return;
	}

    //uin | OSS_LOG_TYPE_CREATE_ACCOUNT | time | client_ip
    TRACEBILL("%u|%d|%d|%s\n", uin, OSS_LOG_TYPE_CREATE_ACCOUNT, CTimeUtility::GetNowTime(), pstSessionObj->GetClientIP());

	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int CAccountOssLog::Initialize()
{

    return 0;
}
