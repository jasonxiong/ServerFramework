#ifndef __ONLINE_STAT_TICK_HPP__
#define __ONLINE_STAT_TICK_HPP__

#include <time.h>

#include "GameProtocol.hpp"

#define ONLINE_STAT_INTERVAL 30 // 30s

class COnlineStatTick
{
public:
    COnlineStatTick();
    int Initialize(bool bResume);
    int OnTick();

private:
    int SendZoneOnlineStat(bool bInit);

private:
    time_t m_stLastTickTime;    
    GameProtocolMsg m_stMsg;
};

#endif
