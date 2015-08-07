#ifndef __MODULE_HELPER_HPP__
#define __MODULE_HELPER_HPP__

#include "SessionManager.hpp"
#include "GameProtocolEngine.hpp"
#include "ConfigManager.hpp"
#include "ModuleShortcut.hpp"
#include "UnitEventManager.hpp"
#include "RepThingsFramework.hpp"

using namespace ServerLib;

typedef enum
{
    MODULE_MAX_ID = 1,
}MODULE_ID;

class CModuleHelper
{
public:
    static CSessionManager* GetSessionManager(){return m_pSessionManager;};
    static void RegisterSessionManager(CSessionManager* pSessionManager);

    static CGameProtocolEngine* GetZoneProtocolEngine(){return m_pZoneProtocolEngine;};
    static void RegisterZoneProtocolEngine(CGameProtocolEngine* pZoneProtocolEngine);

    static CGameMsgTransceiver* GetZoneMsgTransceiver() {return m_pZoneMsgTransceiver;}
    static void RegisterZoneMsgTransceiver(CGameMsgTransceiver* pZoneMsgTransceiver);

    static CConfigManager* GetConfigManager(){return m_pConfigManager;}
    static void RegisterConfigManager(CConfigManager *pConfigManager);

    // 服务器ID
    static void RegisterServerID(int iWorldID, int iZoneID, int iInstanceID);

    static int GetWorldID() { return m_iWorldID; };
    static int GetZoneID() { return m_iZoneID; };
    static int GetInstanceID() {return m_iInstanceID;}

    // 加载模块开关
    static int LoadModuleConfig();

    //是否检查官网key
    static int IsSSKeyCheckEnabled();

    static void SetSSKeyCheck(int iValue) { m_iCheckSSKeyEnabled = iValue; }

    // 是否开启Player日志
    static int IsPlayerLogEnabled() {return m_iIsEnablePlayerLog;}

    // 模块开关
    static bool IsModuleSwitchOn(MODULE_ID eModuleID) {return m_aiModuleSwitch[(int)eModuleID];}

    static void SetModuleSwitchOn(int iSwtichIndex, int iSwitch);

    // 场景单位事件管理器
    static CUnitEventManager* GetUnitEventManager() { return m_pstUnitEventManager; };
    static void RegisterUnitEventManager(CUnitEventManager* pstEventManager) { m_pstUnitEventManager = pstEventManager; };
    
private:
    static int m_iWorldID;
    static int m_iZoneID;
    static int m_iInstanceID;

    // 各种配置开关
    static int m_iCheckSSKeyEnabled;
    static int m_iIsEnablePlayerLog;
    static int m_aiModuleSwitch[MODULE_MAX_ID];

private:

    // 各种模块指针
    static CSessionManager* m_pSessionManager;
    static CGameProtocolEngine* m_pZoneProtocolEngine;
    static CGameMsgTransceiver* m_pZoneMsgTransceiver;
    static CConfigManager* m_pConfigManager;
    static CUnitEventManager* m_pstUnitEventManager;
};


#endif
