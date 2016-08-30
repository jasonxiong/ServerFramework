#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "GameProtocol.hpp"
#include "AppDef.hpp"
#include "NowTime.hpp"
#include "SectionConfig.hpp"
#include "StringUtility.hpp"
#include "ObjStatistic.hpp"
#include "MsgStatistic.hpp"
#include "PerformanceStatistic.hpp"
#include "NameDBApp.hpp"
#include "AppUtility.hpp"

using namespace ServerLib;

int CNameDBApp::ms_iAppCmd = APPCMD_NOTHING_TODO;
CNameDBInfoConfig CNameDBApp::m_stNameDBConfigManager;

CNameDBApp::CNameDBApp()
{
    m_bResumeMode = false;
}

CNameDBApp::~CNameDBApp()
{

}

CSSProtocolEngine* CNameDBApp::GetProtocolEngine(int iThreadIdx)
{
    return m_stThreadManager.GetThread(iThreadIdx)->GetProtocolEngine();
}

int CNameDBApp::Initialize(bool bResume, int iWorldID)
{
    int iRes = 0;

    // 读取配置
    iRes = LoadConfig();
    if (iRes < 0)
    {
        return -2;
    }

    m_bResumeMode = bResume;

    iRes = m_stMsgTransceiver.Initialize(bResume, iWorldID);
    if (iRes < 0)
    {
        return -4;
    }

    //initialize thread pool
    iRes = m_stThreadManager.Initialize(m_bResumeMode);
    if (iRes != 0)
    {
        TRACESVR("Error: init thread manager fail. rt:%d\n", iRes);
        return -7;
    }

    return 0;
}

void CNameDBApp::Run()
{
    TRACESVR("NameDBServer starts running ...\n");

    //TODO -Werror=unused-but-set-variable
    //int iRes = 0;

    while (true)
    {
        NowTimeSingleton::Instance()->RefreshNowTime();
        NowTimeSingleton::Instance()->RefreshNowTimeVal();

        if (ms_iAppCmd == APPCMD_STOP_SERVICE)
        {
            TRACESVR("Receive Command: APPCMD_STOP_SERVICE\n");
            return;
        }

        if (ms_iAppCmd == APPCMD_RELOAD_CONFIG)
        {
            TRACESVR("Receive Command: APPCMD_RELOAD_CONFIG. Do nothing now\n");
            ms_iAppCmd = APPCMD_NOTHING_TODO;
        }

        const int MAX_MSG_COUNT_PER_COUNT = 100;
        int iRecvMsgCount = 0;

        while (true)
        {
            int iRecvNewMsgCount = 0;
            int iSendNewMsgCount = 0;

            //TODO -Werror=unused-but-set-variable
            //iRes = HandleMsgIn(iRecvNewMsgCount);
            HandleMsgIn(iRecvNewMsgCount);

            //process thread msg
            //TODO -Werror=unused-but-set-variable
            //iRes = HandleThreadMsg(iSendNewMsgCount);
            HandleThreadMsg(iSendNewMsgCount);

            // 没有数据, 或者数据超出处理限制, 暂停处理数据
            iRecvMsgCount += iRecvNewMsgCount;

            // 如果没有消息输入而且没有消息输出， 或者总接受消息超出上限
            if ((iRecvNewMsgCount <= 0 && iSendNewMsgCount <= 0) || iRecvMsgCount >= MAX_MSG_COUNT_PER_COUNT)
            {
                break;
            }

            LOGDEBUG("recv msg:%d, send msg:%d\n", iRecvNewMsgCount, iSendNewMsgCount);
        }

        usleep(APP_NAMEDB_MAX_SLEEP_USEC);
    }
}

int CNameDBApp::HandleMsgIn(int& riRecvMsgCount)
{
    int iRes = ReceiveAndDispatchMsg();
    if (iRes != 0)
    {
        return -1;
    }
    else
    {
        riRecvMsgCount++;
    }

    return 0;
}

int CNameDBApp::ReceiveAndDispatchMsg()
{
    int iBufLen = sizeof(m_szCodeBuf);
    int iCodeLen;

    // 初始化code buffer和code length
    iCodeLen = 0;

    // 接受网络数据
    int iRet = m_stMsgTransceiver.RecvOneMsg(m_szCodeBuf, iBufLen, iCodeLen, GAME_SERVER_WORLD);
    if ((iRet < 0) || (0 == iCodeLen))
    {
        return -1;
    }

    char* pszRealBuff = m_szCodeBuf + sizeof(unsigned short);
    int iRealBuffLen = iCodeLen - sizeof(unsigned short);

    //todo jasonxiong 这个地方涉及到了多解析一次数据的问题，后面考虑采用方法解决
    //但是这边需要先取出accountID,所以还必须多这一次解析，后面考虑进行优化
    static GameProtocolMsg stMsg;
    bool bRet = stMsg.ParseFromArray(pszRealBuff, iRealBuffLen);
    if(!bRet)
    {
        TRACESVR("fail to parse code buff to proto!\n");
        return -1;
    }

    TRACESVR("Receive code OK, len: %d, hash id = %u, Msg = %d\n", iCodeLen, stMsg.m_stmsghead().m_uin(), stMsg.m_stmsghead().m_uimsgid());

    iRet = m_stThreadManager.PushCode(stMsg.m_stmsghead().m_uin(), (const unsigned char*)m_szCodeBuf, iCodeLen);
    if (iRet != 0)
    {
        TRACESVR("Error: push into thread fail. rt:%d, hashID:%u, msgid = %u\n", iRet, stMsg.m_stmsghead().m_uin(), stMsg.m_stmsghead().m_uimsgid());
        return -5;
    }

    return 0;
}

void CNameDBApp::SetAppCmd(int iAppCmd)
{
    ms_iAppCmd = iAppCmd;
}

int CNameDBApp::LoadLogConfig(const char* pszConfigFile, const char* pszLogName)
{
    char szLogPath[64] = {0};
    char szLogName[64] = {0};
    int iLogLevel;

    if (!pszConfigFile)
    {
        // 使用默认配置
        SAFE_STRCPY(szLogPath, "../log/", sizeof(szLogPath));
        SAFE_STRCPY(szLogName, pszLogName, sizeof(szLogName));
        iLogLevel = LOG_LEVEL_ANY;
    }
    else
    {
        // 读取配置文件
        CSectionConfig stConfigFile;
        int iRet = stConfigFile.OpenFile(pszConfigFile);
        if (iRet == 0)
        {
            stConfigFile.GetItemValue("Param", "LogPath", szLogPath, sizeof(szLogPath), "../log/");
            stConfigFile.GetItemValue("Param", "LogName", szLogName, sizeof(szLogName), pszLogName);
            stConfigFile.GetItemValue("Param", "LogLevel", iLogLevel, LOG_LEVEL_WARNING);
            stConfigFile.CloseFile();
        }
        else
        {
            // 读取失败, 使用默认配置
            SAFE_STRCPY(szLogPath, "../log/", sizeof(szLogPath));
            SAFE_STRCPY(szLogName, pszLogName, sizeof(szLogName));
            iLogLevel = LOG_LEVEL_ANY;
        }
    }

    #ifdef _DEBUG_
    iLogLevel = LOG_LEVEL_ANY;
    #endif

    // server log
    TLogConfig stServerLogConfig;
    SAFE_STRCPY(stServerLogConfig.m_szPath, szLogPath, sizeof(stServerLogConfig.m_szPath)-1);
    SAFE_STRCPY(stServerLogConfig.m_szBaseName, szLogName, sizeof(stServerLogConfig.m_szBaseName)-1);
    SAFE_STRCPY(stServerLogConfig.m_szExtName, ".log", sizeof(stServerLogConfig.m_szExtName)-1);
    stServerLogConfig.m_iAddDateSuffixType = EADST_DATE_YMD;
    stServerLogConfig.m_iLogLevel = iLogLevel;

    stServerLogConfig.m_shLogType = ELT_LOG_AND_FFLUSH;
    stServerLogConfig.m_iChkSizeSteps = DEFAULT_CHECK_SIZE_STEPS;
    stServerLogConfig.m_iMaxFileSize = DEFAULT_MAX_FILE_SIZE;
    stServerLogConfig.m_iMaxFileCount = DEFAULT_MAX_FILE_COUNT * 2;

    //初始化log
    int iRt = m_stLogManager.Initialize(CNameDBThreadManager::THREAD_NUMER, stServerLogConfig);
    if (iRt != 0)
    {
        return -5;
    }

    ServerLogSingleton::Instance()->ReloadLogConfig(stServerLogConfig);

    // 配置统计日志
    TLogConfig stStatLogConfig;
    char szStatPath[MAX_FILENAME_LENGTH];
    memset(szStatPath, 0, sizeof(szStatPath));

    sprintf(szStatPath, "%s%s", szLogPath, "/stat/");
    strncpy(stStatLogConfig.m_szPath, szStatPath, sizeof(stStatLogConfig.m_szPath) - 1);

    strncpy(stStatLogConfig.m_szExtName, ".log", sizeof(stStatLogConfig.m_szExtName)-1);
    stStatLogConfig.m_iAddDateSuffixType = EADST_DATE_YMD;
    stStatLogConfig.m_iLogLevel = LOG_LEVEL_ANY;
    stStatLogConfig.m_shLogType = ELT_LOG_USE_FPRINTF;
    stStatLogConfig.m_iChkSizeSteps = DEFAULT_CHECK_SIZE_STEPS;
    stStatLogConfig.m_iMaxFileSize = DEFAULT_MAX_FILE_SIZE;
    stStatLogConfig.m_iMaxFileCount = DEFAULT_MAX_FILE_COUNT;

    // 消息统计
    sprintf(stStatLogConfig.m_szBaseName, "%s%s", "m_", pszLogName);
    MsgStatisticSingleton::Instance()->ReloadLogConfig(stStatLogConfig);
    MsgStatisticSingleton::Instance()->Reset();

    // 对象统计
    sprintf(stStatLogConfig.m_szBaseName, "%s%s", "o_", pszLogName);
    ObjectStatisticSingleton::Instance()->ReloadLogConfig(stStatLogConfig);
    ObjectStatisticSingleton::Instance()->Reset();

    // 性能统计
    sprintf(stStatLogConfig.m_szBaseName, "%s%s", "p_", pszLogName);
    PerformanceStatisticSingleton::Instance()->Initialize(stStatLogConfig.m_szPath, stStatLogConfig.m_szBaseName);
    PerformanceStatisticSingleton::Instance()->Reset();

    return 0;
}

int CNameDBApp::ReloadConfig()
{
    LoadLogConfig(APP_CONFIG_FILE, "NameDBServer");

    //load accountDB config
    int iRet = m_stNameDBConfigManager.LoadNameDBInfoConfig(NAMEDBINFO_CONFIG_FILE);
    if(iRet)
    {
        TRACESVR("Failed to load account db info config, ret %d\n", iRet);
        return iRet;
    }

    TRACESVR("Reload Config Success.\n");

    return 0;
}

int CNameDBApp::LoadConfig()
{
    LoadLogConfig(APP_CONFIG_FILE, "NameDBServer");

    //load roledb config
    int iRet = m_stNameDBConfigManager.LoadNameDBInfoConfig(NAMEDBINFO_CONFIG_FILE);
    if(iRet)
    {
        TRACESVR("Failed to load account db info config, ret %d\n", iRet);
        return iRet;
    }

    TRACESVR("Load Config Success.\n");

    return 0;
}

int CNameDBApp::HandleThreadMsg(int& riSendMsgCount)
{
    int iRet = 0;
    int iMaxLength = sizeof(m_szCodeBuf);
    int riMsgLength = 0;

    int i = 0;
    for (i = 0; i < CNameDBThreadManager::THREAD_NUMER; i++)
    {
        iRet = m_stThreadManager.PopCode(i, (unsigned char*)m_szCodeBuf, iMaxLength, riMsgLength);
        if ((iRet >= 0) && (riMsgLength > 0))
        {
            // 发送网络数据
            int iRet = m_stMsgTransceiver.SendOneMsg(m_szCodeBuf, riMsgLength, GAME_SERVER_WORLD);
            if (iRet < 0)
            {
                TRACESVR("Error: send fail. rt:%d\n", iRet);
                return -1;
            }
            else
            {
                riSendMsgCount++;
            }
        }
    }

    return 0;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
