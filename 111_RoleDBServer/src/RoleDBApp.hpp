#ifndef __ROLE_DB_APP_HPP__
#define __ROLE_DB_APP_HPP__

#include "SSProtocolEngine.hpp"
#include "RoleDBPublic.hpp"
#include "RoleDBMsgTransceiver.hpp"
#include "RoleDBHandlerSet.hpp"
#include "RoleDBThreadManager.hpp"
#include "RoleDBLogManager.hpp"
#include "RoleDBInfoConfig.hpp"


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

// RoleDB应用程序
class CRoleDBApp
{
public:
    CRoleDBApp();
    virtual ~CRoleDBApp();

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

    //ROLEDB相关的数据库信息的配置
    static CRoleDBInfoConfig m_stRoleDBConfigManager;

private:
    CRoleDBMsgTransceiver m_stMsgTransceiver;

private:
    char m_szCodeBuf[MAX_MSGBUFFER_SIZE];

private:
    CRoleDBThreadManager m_stThreadManager;
    CRoleDBLogManager m_stLogManager;
};

#endif // __ROLE_DB_APP_HPP__

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
