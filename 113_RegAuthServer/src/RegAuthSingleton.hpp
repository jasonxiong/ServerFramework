#ifndef __REGAUTH_SINGLETON_HPP__
#define __REGAUTH_SINGLETON_HPP__

#include "CSProtocolEngine.hpp"
#include "SSProtocolEngine.hpp"
#include "RegAuthHandlerSet.hpp"
#include "RegAuthMsgTransceiver.hpp"
#include "RegAuthObjectAllocator.hpp"
#include "SessionManager.hpp"
#include "SingletonTemplate.hpp"
#include "ConfigMgr.hpp"
#include "RegAuthApp.hpp"

// RegAuth应用程序中的单件
using namespace ServerLib;

extern CRegAuthApp* g_pRegAuthApp;

#define SSProtocolEngine  CSingleton<S2SProtocolEngine>::Instance()
#define CSProtocolEngine  CSingleton<C2SProtocolEngine>::Instance()
#define RegAuthHandlerSet CSingleton<CRegAuthHandlerSet>::Instance()
#define RegAuthMsgTransceiver  CSingleton<CRegAuthMsgTransceiver>::Instance()
#define RegAuthObjectAllocator CSingleton<CRegAuthObjectAllocator>::Instance()
#define SessionManager  g_pRegAuthApp->GetSessionManager()
#define ConfigMgr  CSingleton<CConfigMgr>::Instance()

#endif // __REGAUTH_SINGLETON_HPP__

