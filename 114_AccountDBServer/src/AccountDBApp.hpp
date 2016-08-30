#ifndef __ROLE_DB_APP_HPP__
#define __ROLE_DB_APP_HPP__

#include "SSProtocolEngine.hpp"
#include "AccountDBPublic.hpp"
#include "AccountDBMsgTransceiver.hpp"
#include "AccountDBHandlerSet.hpp"
#include "AccountDBThreadManager.hpp"
#include "AccountDBLogManager.hpp"
#include "AccountDBInfoConfig.hpp"

template<typename CApp>
class CAppType
{
private:
    static CApp* m_pApp;
public:
    static int RegApp(CApp* pApp)
    {
        if (!pApp)
        {
            return -1;
        }

        m_pApp = pApp;
        return 0;
    }

    static CApp* GetApp()
    {
        return m_pApp;
    }
};

template<typename CApp>
CApp* CAppType<CApp>::m_pApp = NULL;

// AccountDB应用程序
class CAccountDBApp
{
public:
    CAccountDBApp();
    virtual ~CAccountDBApp();

    int Initialize(bool bResume, int iWorldID);
    void Run();
    int HandleMsgIn(int& riRecvMsgCount);
    //process thread msg
    int HandleThreadMsg(int& riSendMsgCount);

public:
    CSSProtocolEngine* GetProtocolEngine(int iThreadIdx);

public:
    static void SetAppCmd(int iAppCmd);

private:
    //receive msg and dispatch
    int ReceiveAndDispatchMsg();

private:
    bool m_bResumeMode; // 共享内存恢复模式，如果程序上次运行所创建的共享内存未被删除则直接attach上去
    static int ms_iAppCmd;

private:
    int LoadLogConfig(const char* pszConfigFile, const char* pszLogName);
    int ReloadConfig();
    int LoadConfig();

public:
    CServerLogAdapter* GetLogAdapter(const int iThreadIdx) {return m_stLogManager.GetLogAdapter(iThreadIdx);};

public:

    //ACCOUNTDB相关的数据库信息的配置
    static CAccountDBInfoConfig m_stAccountDBConfigManager;

    //UniqUinDB相关的数据库信息的配置，用于注册帐号时分配唯一性UIN
    static CAccountDBInfoConfig m_stUniqUinDBConfigManager;

private:
    CAccountDBMsgTransceiver m_stMsgTransceiver;

private:
    char m_szCodeBuf[MAX_MSGBUFFER_SIZE];

private:
    CAccountDBThreadManager m_stThreadManager;
    CAccountDBLogManager m_stLogManager;
};

#endif // __ROLE_DB_APP_HPP__

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
