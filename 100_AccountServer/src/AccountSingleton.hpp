#ifndef __ACCOUNT_SINGLETON_HPP__
#define __ACCOUNT_SINGLETON_HPP__

#include "CSProtocolEngine.hpp"
#include "SSProtocolEngine.hpp"
#include "AccountHandlerSet.hpp"
#include "AccountMsgTransceiver.hpp"
#include "AccountObjectAllocator.hpp"
#include "SessionManager.hpp"
#include "SingletonTemplate.hpp"
#include "ConfigMgr.hpp"
#include "AccountApp.hpp"

// Account应用程序中的单件
using namespace ServerLib;

extern CAccountApp* g_pAccountApp;

#define SSProtocolEngine  CSingleton<S2SProtocolEngine>::Instance()
#define CSProtocolEngine  CSingleton<C2SProtocolEngine>::Instance()
#define AccountHandlerSet CSingleton<CAccountHandlerSet>::Instance()
#define AccountMsgTransceiver  CSingleton<CAccountMsgTransceiver>::Instance()
#define AccountObjectAllocator CSingleton<CAccountObjectAllocator>::Instance()
#define SessionManager  g_pAccountApp->GetSessionManager()
#define ConfigMgr  CSingleton<CConfigMgr>::Instance()

#endif // __ACCOUNT_SINGLETON_HPP__

