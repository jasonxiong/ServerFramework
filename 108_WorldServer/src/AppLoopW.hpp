#ifndef __APP_LOOP_W_HPP__
#define __APP_LOOP_W_HPP__

#include "TimeUtility.hpp"
#include "ConfigHelper.hpp"
#include "WorldProtocolEngineW.hpp"
#include "SharedMemory.hpp"
#include "ModuleHelper.hpp"
#include "WorldObjectAllocatorW.hpp"
#include "AppTick.hpp"
#include "ConfigManager.hpp"
#include "AppDefW.hpp"
#include "WorldMsgTransceiver.hpp"

using namespace ServerLib;

const int MAX_ZONE_SERVER_NUMBER = MAX_ZONE_PER_WORLD + 1;

class CAppLoopW
{
public:
    CAppLoopW();
    virtual ~CAppLoopW();

    virtual int Initialize(bool bResumeMode, int iWorlID);
    virtual int Run();
    static void SetAppCmd(int iAppCmd);

public:
    int SendWorldMsg(unsigned int uiLength, char* pszBuffer, EGameServerID enMsgPeer, int iInstance = 1);

private:
    virtual int ReloadConfig();
    virtual int LoadConfig();

private:

    CWorldMsgTransceiver m_stMsgTransceiver;

    CWorldProtocolEngineW m_stWorldProtocolEngine;

    CSharedMemory m_stSharedMemoryTimer;

    CWorldObjectAllocatorW m_stAllocator;

    CConfigManager m_stConfigManager;

    CAppTick m_stAppTick;

    bool m_bResumeMode;

    timeval m_tvLastCheckTimeout;
    timeval m_tvLastCheckStat;

private:
    static int ms_iAppCmd;
};

#endif



----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
