#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include "Int64Utility.hpp"
#include "StringUtility.hpp"
#include "LogAdapter.hpp"
#include "TimeValue.hpp"
#include "Random.hpp"
#include "TimeUtility.hpp"
#include "StringSplitter.hpp"
#include "ServerBusConfigManager.hpp"

#include "ConfigHelper.hpp"

using namespace ServerLib;

EGameServerStatus g_enServerStatus;

int GAME_GET_WORLD_ID(uint64_t busid)
{
    // TBUS地址为: world:16.zone:16.function:16.instance:16
    return (int)(busid >> 48);
}

int GAME_GET_ZONE_ID(uint64_t busid)
{
    // TBUS地址为: world:16.zone:16.function:16.instance:16
    return (int)((busid>>32)&0xFFFF);
}

int GAME_GET_FUNC_ID(uint64_t busid)
{
    // TBUS地址为: world:16.zone:16.function:16.instance:16
    return (int)((busid<<32)>>48);
}

int GAME_GET_INST_ID(uint64_t busid)
{
    // TBUS地址为: world:16.zone:16.function:16.instance:16
    return (int)((busid<<48)>>48);
}

key_t GenerateServerShmKey(EGameServerID enServerID, int iKeyIndex)
{
    if (enServerID == 0)
    {
        return 10000 + iKeyIndex;
    }

    return enServerID * 100000 + iKeyIndex;
}

//需要读取ZMQ的配置
const char* GetZmqBusAddress(uint64_t ullClientBusID, uint64_t ullServerBusID)
{
    static bool bIsConfigLoaded = false;
    static CServerBusConfigManager oServerBusConfigManager;

    if(!bIsConfigLoaded)
    {
        //还没有加载ZMQ的配置，则先加载配置
        int iRet = oServerBusConfigManager.LoadServerBusConfig();
        if(iRet)
        {
            TRACESVR("Fail to load server zmq bus config, ret %d\n", iRet);
            return NULL;
        }

        bIsConfigLoaded = true;
    }

    const ServerBusConfig* pstBusConfig = oServerBusConfigManager.GetServerBusConfig(ullClientBusID, ullServerBusID);
    if(!pstBusConfig)
    {
        TRACESVR("Fail to get server zmq bus config, client id %"PRIu64", server id %"PRIu64".\n", ullClientBusID, ullServerBusID);
        return NULL;
    }

    return pstBusConfig->szBusAddr;
}

int GetCommonResourcePath(char* pszPathBuffer, const int iPathLen, const char* pszResName)
{
    ASSERT_AND_LOG_RTN_INT(pszPathBuffer);
    ASSERT_AND_LOG_RTN_INT(pszResName);

    int iLen = SAFE_SPRINTF(pszPathBuffer, iPathLen, "../../../Common/%s", pszResName);

    ASSERT_AND_LOG_RTN_INT(iLen > 0 && iLen < iPathLen);

    return 0;
}

#pragma pack(1)
typedef struct tagLargeWorldGUID
{
    uint16_t m_ushWorldID:12;
    uint16_t m_ushZoneID: 4;
}TLargeWorldGUID;
#pragma pack()

/*
int CreateThingGUID(THINGGUID& rstGUID, int iWorldID, int iZoneID)
{
    static const int MAX_THING_CREATED_PER_SECOND = 65535;
    static int m_iLastCreateTime = 0;
    static unsigned short m_usThingCreatedSequence = 0;

    // 初始化物品流水号
    CTimeValue tvNow;
    tvNow.RefreshTime();
    int iCreateTime = tvNow.GetTimeValue().tv_sec;
    m_usThingCreatedSequence++;

    if (iCreateTime == m_iLastCreateTime)
    {
        if (m_usThingCreatedSequence >= MAX_THING_CREATED_PER_SECOND)
        {
            // 物品创建太频繁!
            TRACESVR("Alert: Thing Created Too Frequently!\n");
            m_usThingCreatedSequence = 0;
            return -1;
        }
    }
    else
    {
        m_iLastCreateTime = iCreateTime;
        m_usThingCreatedSequence = 0;
    }

    TLargeWorldGUID stLargeWorldID;
    stLargeWorldID.m_ushWorldID = iWorldID;
    stLargeWorldID.m_ushZoneID = iZoneID;

    memcpy(&rstGUID.m_ucWorldID, &stLargeWorldID, sizeof(uint16_t));

    rstGUID.m_iCreateTime = iCreateTime;
    rstGUID.m_usSequence = m_usThingCreatedSequence;

    return 0;
}
*/

// 根据GUID获取WorldID和ZoneID
/*
extern void GetWorldZoneIDFromGUID(const THINGGUID& rstGUID, int& iWorldID, int& iZoneID)
{
    TLargeWorldGUID stLargeWorldID;
    memcpy(&stLargeWorldID, &rstGUID.m_ucWorldID, sizeof(uint16_t));

    iWorldID = stLargeWorldID.m_ushWorldID;
    iZoneID = stLargeWorldID.m_ushZoneID;
} 
*/ 


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
