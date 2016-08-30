
#include "ErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "StringUtility.hpp"
#include "SectionConfig.hpp"
#include "ConfigHelper.hpp"

#include "ServerBusConfigManager.hpp"

using namespace ServerLib;

//默认都从 conf/GameServer.tcm 中读取

#define SERVER_BUS_CONFIG_FILE  "../conf/GameServer.tcm"

int CServerBusConfigManager::LoadServerBusConfig()
{
    CSectionConfig stConfigFile;
    int iRet = stConfigFile.OpenFile(SERVER_BUS_CONFIG_FILE);
    if(iRet)
    {
        LOGERROR("Failed to load server bus config file: %s!\n", SERVER_BUS_CONFIG_FILE);
        return iRet;
    }

    //加载ServerBus的总数量
    stConfigFile.GetItemValue("ServerBus", "ServerBusNum", m_iTotalBusNumber);

    //加载具体ServerBus的配置
    char szBusClientID[SERVER_BUSID_LEN] = {0};
    char szBusServerID[SERVER_BUSID_LEN] = {0};
    char szBusIPAddr[SERVER_BUSID_LEN] = {0};
    int iBusPort = 0;

    //Bus相关配置项的Key
    char szBusClientKey[SERVER_BUSID_LEN] = {0};
    char szBusServerKey[SERVER_BUSID_LEN] = {0};
    char szBusIPAddrKey[SERVER_BUSID_LEN] = {0};
    char szBusPortKey[SERVER_BUSID_LEN] = {0};

    unsigned short usWorldID = 0;
    unsigned short usServerID = 0;
    unsigned short usInstanceID = 0;
    unsigned short usZoneID = 0;

    for(int i=0; i<m_iTotalBusNumber; ++i)
    {
        //封装Bus的Key
        SAFE_SPRINTF(szBusClientKey, SERVER_BUSID_LEN-1, "ServerBusClientID_%d", i);
        SAFE_SPRINTF(szBusServerKey, SERVER_BUSID_LEN-1, "ServerBusServerID_%d", i);
        SAFE_SPRINTF(szBusIPAddrKey, SERVER_BUSID_LEN-1, "ServerBusIP_%d", i);
        SAFE_SPRINTF(szBusPortKey, SERVER_BUSID_LEN-1, "ServerBusPort_%d", i);

        //读取配置中的值
        stConfigFile.GetItemValue("ServerBus", szBusClientKey, szBusClientID, sizeof(szBusClientID)-1, "0.0.0.0");
        stConfigFile.GetItemValue("ServerBus", szBusServerKey, szBusServerID, sizeof(szBusServerID)-1, "0.0.0.0");
        stConfigFile.GetItemValue("ServerBus", szBusIPAddrKey, szBusIPAddr, sizeof(szBusIPAddr)-1, "0.0.0.0");
        stConfigFile.GetItemValue("ServerBus", szBusPortKey, iBusPort, 0);

        //计算通信Bus的ClientID
        sscanf(szBusClientID, "%hu.%hu.%hu.%hu", &usWorldID, &usServerID, &usInstanceID, &usZoneID);
        m_astServerBusConfig[i].ullBusClientID = GetServerBusID(usWorldID, (EGameServerID)usServerID, usInstanceID, usZoneID);

        //计算通信Bus的ServerID
        sscanf(szBusServerID, "%hu.%hu.%hu.%hu", &usWorldID, &usServerID, &usInstanceID, &usZoneID);
        m_astServerBusConfig[i].ullBusServerID = GetServerBusID(usWorldID, (EGameServerID)usServerID, usInstanceID, usZoneID);

        //组装实际通信Bus的地址,格式为 IP:PORT
        sprintf(m_astServerBusConfig[i].szBusAddr, "%s:%d", szBusIPAddr, iBusPort);
    }

    return 0;
}

const ServerBusConfig* CServerBusConfigManager::GetServerBusConfig(uint64_t ullClientID, uint64_t ullServerID)
{
    for(int i=0; i<m_iTotalBusNumber; ++i)
    {
        if(m_astServerBusConfig[i].ullBusClientID == ullClientID &&
           m_astServerBusConfig[i].ullBusServerID == ullServerID)
        {
            return &m_astServerBusConfig[i];
        }
    }

    return NULL;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
