#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "GameRole.hpp"
#include "ZoneObjectHelper.hpp"

#include "RttUtility.hpp"

uint64_t CRttUtility::Convert32to64(unsigned int high, unsigned int low)
{
    uint64_t ui64 = high;

    ui64 = (ui64 << 32) + low;

    return ui64;
}

uint64_t CRttUtility::GetNowMilliSeconds()
{
    CTimeValue tvNow;
    tvNow.RefreshTime();
    uint64_t ui64MilliSeconds = tvNow.GetTimeValue().tv_sec;

    ui64MilliSeconds = ui64MilliSeconds * 1000 + tvNow.GetTimeValue().tv_usec / 1000;

    return ui64MilliSeconds;
}

uint64_t CRttUtility::GetMilliSeconds(CTimeValue& rstTimeValue)
{
    uint64_t ui64MilliSeconds = rstTimeValue.GetTimeValue().tv_sec;

    ui64MilliSeconds = ui64MilliSeconds * 1000 + rstTimeValue.GetTimeValue().tv_usec / 1000;

    return ui64MilliSeconds;
}

int CRttUtility::UpdateRTT(const GameCSMsgHead& rstMsgHead)
{
    //todo jasonxiong 这个函数内部实际上什么都没有干

    /*
    unsigned int uiUin = rstMsgHead.m_uiUin;
    unsigned int uiClientTimeHigh = rstMsgHead.m_uiTimeStamp_High;
    unsigned int uiClientTimeLow = rstMsgHead.m_uiTimeStamp_Low;
	
    uint64_t ui64Server = GetNowMilliSeconds();
    uint64_t ui64Client = Convert32to64(uiClientTimeHigh, uiClientTimeLow);

    CGameRoleObj* pRoleObj = CUnitUtility::GetRoleByUin(uiUin);
    if (!pRoleObj)
    {
        return 0;
    }

    INT64 iRtt = ui64Server - ui64Client;
    if (iRtt < 0)
    {
        TRACESVR("Client Time Ahead: Uin = %u, Delta = %lld\n", uiUin, iRtt);
		return -1;
    }
    */

	return 0;
}
