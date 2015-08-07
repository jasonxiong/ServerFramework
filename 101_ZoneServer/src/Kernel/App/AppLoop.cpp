#include <unistd.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "NowTime.hpp"
#include "TimeUtility.hpp"
#include "HandlerFactory.hpp"
#include "AppDef.hpp"
#include "AppLoop.hpp"
#include "ShmObjectCreator.hpp"
#include "AppUtility.hpp"
#include "ConfigHelper.hpp"
#include "ZoneOssLog.hpp"
#include "ZoneMsgHelper.hpp"
#include "FreqCtrl.hpp"
#include "ZoneObjectHelper.hpp"
#include "GMCommandManager.hpp"

class CGameRoleObj;

using namespace ServerLib;

//初始化AppCmd
int CAppLoop::ms_iAppCmd = APPCMD_NOTHING_TODO;
CTimeValue CAppLoop::m_stLotusMsgMaxProsessTime;
CTimeValue CAppLoop::m_stWorldMsgMaxProcessTime;
CTimeValue CAppLoop::m_stTickMaxProcessTime;


#ifdef _DEBUG_
const int  APP_ZONE_MAX_SLEEP_USEC = 5 * 1000;
#else
const int  APP_ZONE_MAX_SLEEP_USEC = 5 * 1000;
#endif

CAppLoop::CAppLoop()
{
    m_bResumeMode = false;
}

void CAppLoop::SetAppCmd(int iAppCmd)
{
    ms_iAppCmd = iAppCmd;
}

int CAppLoop::ReloadConfig()
{
    //m_stPinDrawLucky.WriteData();

    // GameZoneServer.tcm
    CAppUtility::LoadLogConfig(APP_CONFIG_FILE, "GameZoneServer");

    CModuleHelper::LoadModuleConfig();

    // GMList.ini
    CModuleHelper::GetConfigManager()->Initialize(true);

    // OssLogConfig.xml
    CZoneOssLog::Initialize();

    TRACESVR("Reload Config Success.\n");

    return 0;
}

int CAppLoop::LoadConfig()
{
    // GameZoneServer.tcm
    CAppUtility::LoadLogConfig(APP_CONFIG_FILE, "GameZoneServer");

    CModuleHelper::LoadModuleConfig();

    // OssLogConfig.xml
    CZoneOssLog::Initialize();

    TRACESVR("Load Config Success.\n");

    return 0;
}

int CAppLoop::Initialize(bool bResume, int iWorldID, int iZoneID, int iInstanceID)
{
    if (bResume)
    {
        SetServerStatus(GAME_SERVER_STATUS_RESUME);
    }
    else
    {
        SetServerStatus(GAME_SERVER_STATUS_INIT);
    }

    //读取配置
    LoadConfig();

    m_bResumeMode = bResume;

    int iRet = -1;

    // 模块注册
    CModuleHelper::RegisterServerID(iWorldID, iZoneID, iInstanceID);
    CModuleHelper::RegisterUnitEventManager(&m_stUnitEventManager);
    CModuleHelper::RegisterConfigManager(&m_stConfigManager);
    CModuleHelper::RegisterZoneProtocolEngine(&m_stZoneProtocolEngine);
    CModuleHelper::RegisterSessionManager(&m_stSessionManager);
    CModuleHelper::RegisterZoneMsgTransceiver(&m_stZoneMsgTransceiver);

    m_stHandlerList.RegisterAllHandler();
    m_stZoneProtocolEngine.SetService(this);

    CGMCommandManager::Instance()->Init();

    // 消息收发器
    iRet = m_stZoneMsgTransceiver.Initialize(m_bResumeMode);
    if (iRet < 0)
    {
        TRACESVR("ZoneBus initialized failed: iRet = %d\n", iRet);
        exit(3);
    }

    // 协议编解码
    iRet = m_stZoneProtocolEngine.Initialize(m_bResumeMode);
    if (iRet < 0)
    {
        TRACESVR("Protocol Engine initialize failed: iRet = %d\n", iRet);
        exit(4);
    }

    // Excel表格配置加载
    iRet = m_stConfigManager.Initialize(m_bResumeMode);
    if (iRet < 0)
    {
        TRACESVR("ConfigManager initialize failed %d\n", iRet);
        exit(5);
    }

    // 对象池初始化
    iRet = m_stAllocator.Initialize(m_bResumeMode);
    if(iRet < 0)
    {
        TRACESVR("Allocator initialize failed, iRet %d\n", iRet);
        exit(6);
    }

    // 登录Session
    iRet = m_stSessionManager.Initialize(m_bResumeMode);
    if (iRet < 0)
    {
        TRACESVR("SessionManager initialize failed\n");
        exit(7);
    }

    // 角色日志
    TLogConfig& rstPlayerLogConfig = PlayerLogSingleton::Instance()->GetLogConfig();
    rstPlayerLogConfig.m_iLogLevel = LOG_LEVEL_ANY;

    //TLogConfig stServerLogConfig = ServerLogSingleton::Instance()->GetLogConfig();
    //stServerLogConfig.m_iMaxFileExistDays = 7;
    //ServerLogSingleton::Instance()->ReloadLogConfig(stServerLogConfig);

    //TLogConfig stPlayerLogConfig = PlayerLogSingleton::Instance()->GetLogConfig();
    //stPlayerLogConfig.m_iMaxFileExistDays = 3;
    //PlayerLogSingleton::Instance()->ReloadLogConfig(stPlayerLogConfig);

    // 频率控制器
    CFreqencyCtroller<0>::Instance()->SetParameters(1, 100, 100);
    CFreqencyCtroller<0>::Instance()->CheckFrequency(0);

    // Tick机制
    iRet = m_stAppTick.Initialize(bResume);
    if (iRet < 0)
    {
        TRACESVR("AppTick initialize failed!\n");
        exit(18);
    }

    //Lua脚本系统加载
    //LuaScriptFramework oLuaFramework;
    //iRet = oLuaFramework.Init();
    //if(iRet)
    //{
    //    TRACESVR("fail to init lua framework, ret %d!\n", iRet);
    //    return iRet;
    //}

    // 放在最后
    SetServerStatus(GAME_SERVER_STATUS_IDLE);

    return 0;
}

void CAppLoop::OnTick()
{
    m_stAppTick.OnTick();
}

int CAppLoop::Run()
{
    int iRet = -1;

    TRACESVR("Run, run, run, never stop!\n");
    TRACESVR("WorldID: %d, ZoneID = %d\n", CModuleHelper::GetWorldID(), CModuleHelper::GetZoneID());
    TRACESVR("Mode: %s\n", m_bResumeMode ? "Resume" : "Initialize");
    TRACESVR("==============================================================================\n");

    static char szBuffer[MAX_MSGBUFFER_SIZE];
    int iLength = sizeof(szBuffer);
    int iBusyCount = 0;

    //init tick time
    NowTimeSingleton::Instance()->EnablePseudoTime();
    CTimeUtility::m_uiTimeTick = time(NULL);
    CTimeUtility::m_stTimeValueTick.RefreshTime();

    timeval stTimeLoopStart;
    timeval stTimeLoopEnd;

    while(true)
    {
        NowTimeSingleton::Instance()->RefreshNowTime();
        NowTimeSingleton::Instance()->RefreshNowTimeVal();

        stTimeLoopStart = NowTimeSingleton::Instance()->GetNowTimeVal();
        CTimeUtility::m_stTimeValueTick = CTimeValue(stTimeLoopStart);
        CTimeUtility::m_uiTimeTick = stTimeLoopStart.tv_sec;

        if (ms_iAppCmd == APPCMD_STOP_SERVICE)
        {
            bool bStop = Stop();
            if (bStop)
            {
                break;
            }
        }

        if (ms_iAppCmd == APPCMD_RELOAD_CONFIG)
        {
            TRACESVR("Receive Command: APPCMD_RELOAD_CONFIG\n");
            ReloadConfig();
            ms_iAppCmd = APPCMD_NOTHING_TODO;
        }

        if (ms_iAppCmd == APPCMD_QUIT_SERVICE)
        {
            TRACESVR("Receive Command: APPCMD_QUIT_SERVICE\n");
            break;
        }

        // 处理所有的消息

        int iNewMsgCount = 0;
        int riMsgLength=0;

        CTimeValue astTimeVal[4];
        CTimeValue stDiff;
        astTimeVal[0].RefreshTime();
        // 停服期间, 不处理Lotus消息
        if (GetServerStatus() != GAME_SERVER_STATUS_STOP)
        {
            for (int i = 0; i < m_stZoneMsgTransceiver.GetCodeQueueNum(); i++)
            {
                // Lotus 消息
                iLength = sizeof(szBuffer);

                while(iNewMsgCount < 1000)
                {
                    riMsgLength = 0;
                    iRet = m_stZoneMsgTransceiver.RecvOneMsg(szBuffer, iLength, riMsgLength, GAME_SERVER_LOTUSZONE, i);
                    if(iRet <0 || riMsgLength<=0)
                    {
                        //接收不到更多消息
                        break;
                    }

                    m_stZoneProtocolEngine.OnRecvCode(szBuffer, riMsgLength, GAME_SERVER_LOTUSZONE, i);
                    iNewMsgCount++;
                }
            }
        }

        astTimeVal[1].RefreshTime();
        stDiff = astTimeVal[1] - astTimeVal[0];
        if (stDiff > m_stLotusMsgMaxProsessTime)
        {
            m_stLotusMsgMaxProsessTime = stDiff;
        }

        // World 消息
        iLength = sizeof(szBuffer);
        riMsgLength=0;
        iRet = m_stZoneMsgTransceiver.RecvOneMsg(szBuffer, iLength, riMsgLength, GAME_SERVER_WORLD);
        if ((iRet >= 0) && (riMsgLength > 0))
        {
            m_stZoneProtocolEngine.OnRecvCode(szBuffer, riMsgLength, GAME_SERVER_WORLD);
            iNewMsgCount++;
        }

        astTimeVal[2].RefreshTime();
        stDiff = astTimeVal[2] - astTimeVal[1];
        if (stDiff > m_stWorldMsgMaxProcessTime)
        {
            m_stWorldMsgMaxProcessTime = stDiff;
        }

        EGameServerStatus enServerStatus = GetServerStatus();

        // 设置服务器状态
        if (iNewMsgCount == 0)
        {
            if (enServerStatus != GAME_SERVER_STATUS_PRESTOP
                    && enServerStatus != GAME_SERVER_STATUS_STOP)
            {
                SetServerStatus(GAME_SERVER_STATUS_IDLE);
            }
            iBusyCount = 0;
        }
        else
        {
            if (enServerStatus != GAME_SERVER_STATUS_PRESTOP
                && enServerStatus != GAME_SERVER_STATUS_STOP)
            {
                SetServerStatus(GAME_SERVER_STATUS_BUSY);
                iBusyCount++;
            };
        }
        m_stAppTick.OnTick();

        NowTimeSingleton::Instance()->RefreshNowTimeVal();
        stTimeLoopEnd = NowTimeSingleton::Instance()->GetNowTimeVal();
        int64_t iLeftUsec = stTimeLoopStart.tv_sec * 1000000 + stTimeLoopStart.tv_usec + APP_ZONE_MAX_SLEEP_USEC
                            - stTimeLoopEnd.tv_sec * 1000000 - stTimeLoopEnd.tv_usec;

        if (iLeftUsec > 0)
        {
            usleep(iLeftUsec);
        }
        else if (iBusyCount > 1000)
        {
            iBusyCount = 0;
            usleep(10);
        }

        astTimeVal[3].RefreshTime();
        stDiff = astTimeVal[3] - astTimeVal[2];
        if (stDiff > m_stTickMaxProcessTime)
        {
            m_stTickMaxProcessTime = stDiff;
        }
    }

    return 0;
}

// 停服
bool CAppLoop::Stop()
{
    //m_stPinDrawLucky.WriteData();
    // PRESTOP状态
    if ((GAME_SERVER_STATUS_PRESTOP != GetServerStatus())
        && (GAME_SERVER_STATUS_STOP != GetServerStatus()))
    {
        TRACESVR("Receive Command: APPCMD_STOP_SERVICE\n");
        SetServerStatus(GAME_SERVER_STATUS_PRESTOP);

        // 设置停服超时
        m_stStopService.RefreshTime();
        timeval stTimeval = m_stStopService.GetTimeValue();
        stTimeval.tv_sec += SERVER_PRESTOP_TIME;
        m_stStopService.SetTimeValue(stTimeval);

        return false;
    }

    // 在线人数，如果为零则退出
    static int iRoleOnline = 0;
    int iRoleOld = iRoleOnline;
    iRoleOnline = GameTypeK32<CGameRoleObj>::GetUsedObjNumber();
    if (0 == iRoleOnline)
    {
        TRACESVR("Waiting for 0 users...............\n");
        return true;
    }

    CTimeValue stTimeVal;
    stTimeVal.RefreshTime();
    // 超时：设置服务器状态，不再处理某些命令
    if (stTimeVal > m_stStopService)
    {
        SetServerStatus(GAME_SERVER_STATUS_STOP);

        if (iRoleOld != iRoleOnline)
        {
            TRACESVR("Waiting for %d users...............\n", iRoleOnline);
        }

        return false;
    }

    // 控制提示次数：每秒一次
    static int siSeconds = 0;
    if (stTimeVal.GetTimeValue().tv_sec < (siSeconds + 1))
    {
        return false;
    }
    siSeconds = stTimeVal.GetTimeValue().tv_sec;

    // 每10秒提示，小于10秒则每秒提示
    int iDeltaTime = m_stStopService.GetTimeValue().tv_sec - stTimeVal.GetTimeValue().tv_sec;
    if ( (0 != (iDeltaTime % 10)) && (iDeltaTime > 10))
    {
        return false;
    }

    // 提示所有客户端
    //todo jasonxiong 后续考虑修改 -1 为自己的错误码 EQEC_Server_Maintence
    //CPromptMsgHelper::SendPromptMsgToZoneAll(-1, PROMPTMSG_TYPE_POPUP, iDeltaTime);

    return false;
}

