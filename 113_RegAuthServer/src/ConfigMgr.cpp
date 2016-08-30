#include <fstream>
#include <sstream>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ConfigMgr.hpp"
#include "SectionConfig.hpp"
#include "StringUtility.hpp"
#include "RegAuthPublic.hpp"

using namespace std;
using namespace ServerLib;

int CConfigMgr::m_iWhiteListEnabled = 0;

int CConfigMgr::LoadAllConf()
{
    int iRet = -1;
    CSectionConfig stConfigFile;
    iRet = stConfigFile.OpenFile("../conf/ServerList.tcm");
    if(iRet)
    {
        TRACESVR("Failed to get server list config file, ret %d\n", iRet);
        return -1;
    }

    //获取数量
    stConfigFile.GetItemValue("ServerList", "ServerNum", m_iWulinServerNum, 0);

    static char szConfigKey[64] = {0};
    for(int i=0; i<m_iWulinServerNum; ++i)
    {
        SAFE_SPRINTF(szConfigKey, sizeof(szConfigKey)-1, "ServerID_%d", i);
        stConfigFile.GetItemValue("ServerList", szConfigKey, m_astServerList[i].iServerID, 0);

        SAFE_SPRINTF(szConfigKey, sizeof(szConfigKey)-1, "AccountServer_%d", i);
        stConfigFile.GetItemValue("ServerList", szConfigKey,  m_astServerList[i].szAccountIP, sizeof(m_astServerList[i].szAccountIP), "");

        SAFE_SPRINTF(szConfigKey, sizeof(szConfigKey)-1, "ZoneServer_%d", i);
        stConfigFile.GetItemValue("ServerList", szConfigKey,  m_astServerList[i].szZoneIP, sizeof(m_astServerList[i].szZoneIP), "");
    }

    stConfigFile.CloseFile();

    iRet = m_stUinPrivManager.Initialize(MAX_UINPRIV_NUMBER, "../../../Common/UinPriv.shm");
    if (iRet)
    {
        TRACESVR("failed to init UinPriv config\n");
        return -1;
    }

    return 0;
}

const WulinServerConfig* CConfigMgr::GetServerInfo(int iServerID)
{
    for(int i=0; i<m_iWulinServerNum; ++i)
    {
        if(m_astServerList[i].iServerID == iServerID)
        {
            return &m_astServerList[i];
        }
    }

    return NULL;
}

int CConfigMgr::IsWhiteListEnabled()
{
    return m_iWhiteListEnabled;
}

TUinPrivInfo* CConfigMgr::GetUinPrivInfo(unsigned int uiUin)
{
    return m_stUinPrivManager.GetUinPrivInfo(uiUin);
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
