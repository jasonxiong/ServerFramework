
#include "Int64Utility.hpp"
#include "GameProtocol.hpp"
#include "ConfigHelper.hpp"
#include "ModuleHelper.hpp"
#include "LogAdapter.hpp"
#include "SharedMemory.hpp"
#include "StringUtility.hpp"
#include "SectionConfig.hpp"
#include "AppDef.hpp"
#include "ZoneErrorNumDef.hpp"

#include "GameMsgTransceiver.hpp"

using namespace ServerLib;

// 读取CodeQueue配置
// 初始化所有的资源, 包括CodeQueue, TBus等
int CGameMsgTransceiver::Initialize(bool bResumeMode)
{
    int iWorldID = CModuleHelper::GetWorldID();
    int iZoneID = CModuleHelper::GetZoneID();
    int iInstanceID = CModuleHelper::GetInstanceID();

	// 初始化Zone/Lotus CodeQueue消息通道
	int iRet;
	iRet = m_stCodeQueueManager.LoadCodeQueueConfig(APP_CONFIG_FILE, "Zone");
	if (iRet < 0)
	{
		LOGERROR("LoadCodeQueueConfig failed, iRet:%d\n", iRet);
		return -1;
	}

	m_stCodeQueueManager.Initialize(bResumeMode);
	if (iRet < 0)
	{
		LOGERROR("Code Queue Manager Initialize failed, iRet:%d\n", iRet);
		return -2;
	}
    
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // 初始化ZMQ BUS 通道
    uint64_t ullZone2WorldClientID = GetServerBusID(iWorldID, GAME_SERVER_ZONE, iInstanceID, iZoneID);
    uint64_t ullZone2WorldServerID = GetServerBusID(iWorldID, GAME_SERVER_WORLD);

    const char* pszRealBusAddr = GetZmqBusAddress(ullZone2WorldClientID, ullZone2WorldServerID);
    if(!pszRealBusAddr)
    {
        LOGERROR("Fail to get zmq addr, client id %"PRIu64", server id %"PRIu64"\n", ullZone2WorldClientID, ullZone2WorldServerID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //初始化ZMQ BUS 通道
    iRet = m_oZone2WorldClient.ZmqInit(pszRealBusAddr, EN_ZMQ_SOCKET_PAIR, EN_ZMQ_PROC_TCP, EN_ZMQ_SERVICE_CLIENT);
    if(iRet)
    {
        LOGERROR("Fail to init zone2world zmq bus, ret %d\n", iRet);
        return iRet;
    }

    LOGDEBUG("Success to init zone2world zmq bus, zone %d, world %d\n", iZoneID, iWorldID);

    return 0;
}

// 发送和接收消息
int CGameMsgTransceiver::SendOneMsg(const char* pszMsg, int iMsgLength, EGameServerID enMsgPeer, int iInstanceID)
{

    int iRet = -10;

    switch (enMsgPeer)
    {
    case GAME_SERVER_LOTUSZONE:
        {
			iRet = m_stCodeQueueManager.SendOneMsg(pszMsg, iMsgLength, iInstanceID);
            break;
        }

    case GAME_SERVER_WORLD:
        {
            iRet = m_oZone2WorldClient.ZmqSend(pszMsg, iMsgLength, 0);
            break;
        }

    default:
        {
            break;
        }
    }

    if (iRet < 0)
    {
        LOGERROR("Send Zone Msg Failed: Peer = %d, MsgLen = %d, iRet = %d\n",
                    enMsgPeer, iMsgLength, iRet);
    }

    return iRet;
}

int CGameMsgTransceiver::RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, EGameServerID enMsgPeer, int iInstanceID)
{
    int iRet = 0;

    switch (enMsgPeer)
    {
    case GAME_SERVER_LOTUSZONE:
        {
            iRet = m_stCodeQueueManager.RecvOneMsg(pszMsg, iMaxOutMsgLen, riMsgLength, iInstanceID);
            break;
        }

    case GAME_SERVER_WORLD:
        {
            iRet = m_oZone2WorldClient.ZmqRecv(pszMsg, iMaxOutMsgLen, riMsgLength, 0);
            break;
        }

    default:
        {
            break;
        }
    }
    
    //todo jasonxiong 后面考虑添加监控
    /*
    if (iRet < 0 || riMsgLength <= 0)
    {
        char szMsgAlarm[1024] = {'\0'};
		SAFE_SPRINTF(szMsgAlarm, sizeof(szMsgAlarm), "MsgAlarm: Receive Msg Instance = %d, enMsgPeer = %d.\n", iInstanceID, enMsgPeer);
        TNM2_REPORT_WARNING(SS_MSG_SEND_FAILED,  szMsgAlarm);
    }
    */

    return iRet;
}

