#ifndef __REGAUTH_APP_HPP__
#define __REGAUTH_APP_HPP__

#include "App.hpp"
#include "CodeQueue.hpp"
#include "ProtocolEngine.hpp"
#include "RegAuthPublic.hpp"
#include "SessionManager.hpp"

using namespace ServerLib;

// RegAuth应用程序
class CRegAuthApp : public CApp
{
private:
    TNetHead_V2 m_stNetHead;

private:
    static const unsigned int MAIN_LOOP_SLEEP = 10; // 主循环中每次循环后会睡眠10微妙
    static const unsigned int TIMES_TO_CLEAR_CACHE = 10000; // 主循环1万次清理一次超时的缓存结点
    unsigned int m_uiLoopTimes; // 主循环次数

    CSessionManager m_stSessionManager;

public:
    CSessionManager* GetSessionManager() {return &m_stSessionManager;}

public:
    CRegAuthApp();
    virtual ~CRegAuthApp();

    virtual int Initialize(bool bResume, int iWorldID);
    virtual void Run();

private:
    virtual int HandleMsgIn(IProtocolEngine* pProtocolEngine, EGameServerID enMsgPeer, int iInstance = 0);

private:
    bool m_bResumeMode; // 共享内存恢复模式，如果程序上次运行所创建的共享内存未被删除则直接attach上去
    static int ms_iAppCmd;
    static int m_iWorldID;

public:
    static void SetAppCmd(int iAppCmd);
    static int GetWorldID() {return m_iWorldID;}

private:
    virtual int ReloadConfig();
    virtual int LoadConfig();

public:
    void ClearCache(); // 清理超时的cache结点
};

#endif // __REGAUTH_APP_HPP__


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
