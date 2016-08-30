#ifndef __APP_TICK_HPP__
#define __APP_TICK_HPP__

#include "UnitTick.hpp"
#include "ZoneTick.hpp"
#include "TimeValue.hpp"
#include "WorldOnlineStatTick.hpp"

using namespace ServerLib;

// 半秒定时器
#define TICK_TIMEVAL_BASE        500
#define STATISTIC_TIME 60
class CAppTick
{
public:
    CAppTick();

    int Initialize(bool bResumeMode);
    int CountObjStat();
    int OnTick();

private:
    CUnitTick m_stUnitTick;
    CZoneTick m_stZoneTick;
    CWorldOnlineStatTick m_stOnlineStatTick;
    CTimeValue m_tvLastBaseTick;
    int m_iLastMsgStatTick;
};



#endif



----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
