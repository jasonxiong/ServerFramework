#ifndef __MODULE_HELPER_HPP__
#define __MODULE_HELPER_HPP__

#include "WorldProtocolEngineW.hpp"
#include "AppLoopW.hpp"
#include "ConfigManager.hpp"
#include "ZoneTick.hpp"

using namespace ServerLib;

class CModuleHelper
{
public:
	static CWorldProtocolEngineW* GetWorldProtocolEngine();
	static void RegisterWorldProtocolEngine(CWorldProtocolEngineW* pWorldProtocolEngine);

    static CAppLoopW* GetAppLoop();
    static void RegisterAppLoop(CAppLoopW* pAppLoop);

    static CConfigManager* GetConfigManager(){return m_pConfigManager;};
    static void RegisterConfigManager(CConfigManager* pConfigManager);

    // 服务器ID
    static void RegisterServerID(int iWorldID) {m_iWorldID = iWorldID;};
    static int GetWorldID() {return m_iWorldID;};
    
    // ZoneTick
    static void RegisterZoneTick(CZoneTick* pZoneTick) {m_pZoneTick = pZoneTick;};
    static CZoneTick* GetZoneTick() {return m_pZoneTick;};

    // realname/aas
    static void LoadConfig();

	//服务器负载人数
	static int GetFullWorldNum();
	static int GetBusyWorldNum();
	static bool IsZoneBalanceEnabled() {return m_iIsZoneBalanceEnabled == 1;}

	static bool IsRealGM(unsigned int uiUin);

private:
	static CWorldProtocolEngineW* m_pWorldProtocolEngine;
	static CAppLoopW* m_pAppLoop;
    static CConfigManager* m_pConfigManager;
    static int m_iWorldID;
    static CZoneTick* m_pZoneTick;
	static int m_iFullWorldNum;
	static int m_iBusyWorldNum;
	static int m_iIsZoneBalanceEnabled;
};







#endif



----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
