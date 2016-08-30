#ifndef __ZONE_TICK_HPP__
#define __ZONE_TICK_HPP__

#include "GameConfigDefine.hpp"
#include "GameProtocol.hpp"

// Zone在线超时时间 (10分钟)
const int UNACTIVE_ZONE_TIMEOUT     = 10 * 60;

// 检查间隔 (5秒)
const int ZONE_TICK_INTERVAL        = 5;

typedef struct tagZoneStatus
{
	int m_iZoneID;              // ZoneID
	int m_iLastActiveTime;      // 上次活动时间
	int m_iOnlineNumber;        // 在线人数
	unsigned char m_ucActive;   // 是否活跃
	unsigned char m_ucState;	// 繁忙状态
}TZoneStatus;

using namespace GameConfig;

class CZoneTick
{
public:
    int Initialize(bool bResumeMode);

    int OnTick();
    
public:
    void ActiveRefresh(int iZoneID, int iOnlineNumber = -2);

public:
    const TZoneStatus& GetZoneStatus(int iZoneID);
	int GetActiveZoneNum()
	{
		return m_iActiveZoneNum;
	}
	int GetWorldOnlineNum()
	{
		return m_iWorldOnlineNum;
	}

	// 获得人数最少的ZoneID
	int GetLowestZoneID(bool bHomeZone = false);

public:
    bool IsZoneActive(int iZoneID);
private:
    void OnZoneDead(int iZoneID);

private:
    TZoneStatus m_astZoneStatus[MAX_ZONE_PER_WORLD];
    int m_iLastTickTime;
	int m_iActiveZoneNum;
	int m_iWorldOnlineNum;
private:
    GameProtocolMsg m_stWorldMsg;
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
