#include <unistd.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "NowTime.hpp"
#include "TimeUtility.hpp"
#include "AppDefW.hpp"
#include "AppLoopW.hpp"
#include "MsgStatistic.hpp"
#include "AppUtility.hpp"
#include "ShmObjectCreator.hpp"
#include "WorldBillLog.hpp"
#include "WorldMsgHelper.hpp"
#include "Random.hpp"

using namespace ServerLib;

//初始化AppCmd
int CAppLoopW::ms_iAppCmd = APPCMD_NOTHING_TODO;

CAppLoopW::CAppLoopW()
{

}

CAppLoopW::~CAppLoopW()
{
}

int CAppLoopW::ReloadConfig()
{
    // StarWorldServer.tcm
    CAppUtility::LoadLogConfig(APP_CONFIG_FILE, "GameWorldServer");

    //加载游戏配置
    CModuleHelper::LoadConfig();

    CWorldBillLog::Initialize();

    int iRet = m_stConfigManager.LoadZoneConf("../conf/zone_conf.txt");
    if (iRet != 0)
    {
        TRACESVR("failed to load zone config\n");
    }
    else
    {
        CWorldMsgHelper::BroadcastNewZoneConf();
        TRACESVR("Reload Config Success.\n");
    }

    return 0;
}

int CAppLoopW::LoadConfig()
{
    // StarWorldServer.tcm
    CAppUtility::LoadLogConfig(APP_CONFIG_FILE, "GameWorldServer");

    //Load 游戏配置
    CModuleHelper::LoadConfig();

    //Load日志的配置
    CWorldBillLog::Initialize();

    return 0;
}

int CAppLoopW::Initialize(bool bResumeMode, int iWorlID)
{
    // 读取配置
    LoadConfig();

    m_bResumeMode = bResumeMode;

    CModuleHelper::RegisterServerID(iWorlID);
    CModuleHelper::RegisterWorldProtocolEngine(&m_stWorldProtocolEngine);
    CModuleHelper::RegisterAppLoop(this);
    CModuleHelper::RegisterConfigManager(&m_stConfigManager);

    int iRet = m_stAllocator.Initialize(bResumeMode);
    if(iRet < 0)
    {
        TRACESVR("Allocator initialize failed: iRet = %d\n", iRet);
        exit(1);
    }

    iRet = m_stWorldProtocolEngine.Initialize(bResumeMode);
    if (iRet < 0)
    {
        TRACESVR("Protocol Engine initialize failed: iRet = %d\n", iRet);
        exit(2);
    }
    m_stWorldProtocolEngine.SetService(this);
    m_stWorldProtocolEngine.RegisterAllHandler();

    iRet = m_stConfigManager.Initialize(bResumeMode);
    if (iRet < 0)
    {
        TRACESVR("ConfigManager initialize failed: iRet = %d\n", iRet);
        exit(3);
    }

    iRet = m_stMsgTransceiver.Initialize(bResumeMode);
    if (iRet < 0)
    {
        TRACESVR("MsgTransceiver initialize failed: iRet = %d\n", iRet);
        exit(4);
    }

    iRet = m_stAppTick.Initialize(bResumeMode);
    if (iRet < 0)
    {
        TRACESVR("Tick Initialize failed: %d.n\n", iRet);
        exit(6);
    }

    //todo jasonxiong 暂时不需要这部分逻辑，后续需要时再添加
    /*
    // 从ROLEDB中预加载玩家摘要信息
    if (!bResumeMode)
    {
        CWorldRoleSnapUtility::FetchRoleSnapInfo(0, 0);
    }
    */

    TRACESVR("Load Config Success.\n");

    return 0;
}

int CAppLoopW::Run()
{
    unsigned  int uiLoopCount = 0;

    TRACESVR("Run, run, run, never stop!\n");
    TRACESVR("WorldID: %d\n", CModuleHelper::GetWorldID());
    TRACESVR("Mode: %s\n", m_bResumeMode ? "Resume" : "Initialize");
    TRACESVR("==============================================================================\n");

    MsgStatisticSingleton::Instance()->Initialize();
    MsgStatisticSingleton::Instance()->Reset();
    static char szBuffer[MAX_MSGBUFFER_SIZE];
    int iBuffLength = sizeof(szBuffer);
    unsigned int uiNowTime = time(NULL);
    unsigned int uiStatTime = time(NULL);

    while(true)
    {
        NowTimeSingleton::Instance()->RefreshNowTime();
        NowTimeSingleton::Instance()->RefreshNowTimeVal();

        if(ms_iAppCmd == APPCMD_STOP_SERVICE)
        {
            TRACESVR("Receive Command: APPCMD_STOP_SERVICE\n");

            return 0;
        }

        if(ms_iAppCmd == APPCMD_RELOAD_CONFIG)
        {
            TRACESVR("Receive Command: APPCMD_RELOAD_CONFIG. \n");
            ReloadConfig();
            ms_iAppCmd = APPCMD_NOTHING_TODO;
        }

        if (ms_iAppCmd == APPCMD_QUIT_SERVICE)
        {
            TRACESVR("Receive Command: APPCMD_QUIT_SERVICE\n");
            break;
            ms_iAppCmd = APPCMD_NOTHING_TODO;
        }

        // 接收消息处理
        int iRecvMsgCount = 0;
        while(1)
        {
            iBuffLength = sizeof(szBuffer);
            int iMsgLength=0;
            int iInstance = 0;
            EGameServerID enServerID = GAME_SERVER_UNDEFINE;
            int iRet = m_stMsgTransceiver.RecvOneMsg(szBuffer, iBuffLength, iMsgLength, enServerID, iInstance);
            if (iRet < 0 || iMsgLength == 0)
            {
                break;
            }

            m_stWorldProtocolEngine.OnRecvCode(szBuffer, iMsgLength, enServerID, iInstance);
            iRecvMsgCount++;
            if (iRecvMsgCount >= 1000)
            {
                break;
            }
        }

        m_stAppTick.OnTick();

        //统计
        uiNowTime = time(NULL);
        if(uiNowTime - uiStatTime >= 5 * 60) // 5分钟
        {
            MsgStatisticSingleton::Instance()->Print();
            MsgStatisticSingleton::Instance()->Reset();
            uiStatTime = uiNowTime;
        }

        uiLoopCount++;
        usleep(10);
    }

    return 0;
}

int CAppLoopW::SendWorldMsg(unsigned int uiLength, char* pszBuffer, EGameServerID enMsgPeer, int iInstance)
{
    int iRet = m_stMsgTransceiver.SendOneMsg(pszBuffer, uiLength, enMsgPeer, iInstance);
    if(iRet < 0)
    {
        return -1;
    }

    return 0;
}

void CAppLoopW::SetAppCmd(int iAppCmd)
{
    ms_iAppCmd = iAppCmd;

}

