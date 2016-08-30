#include "GameProtocol.hpp"
#include "StringUtility.hpp"
#include "SectionConfig.hpp"
#include "StringSplitter.hpp"
#include "AppDef.hpp"

#include "ModuleHelper.hpp"

CSessionManager* CModuleHelper::m_pSessionManager = 0;
CGameProtocolEngine* CModuleHelper::m_pZoneProtocolEngine = 0;
CConfigManager* CModuleHelper::m_pConfigManager = 0;
CGameMsgTransceiver* CModuleHelper::m_pZoneMsgTransceiver = 0;
CUnitEventManager* CModuleHelper::m_pstUnitEventManager = NULL;

int CModuleHelper::m_iZoneID = 0;
int CModuleHelper::m_iWorldID = 0;
int CModuleHelper::m_iInstanceID = 0;
int CModuleHelper::m_iCheckSSKeyEnabled = 0;
int CModuleHelper::m_iIsEnablePlayerLog = 0;

int CModuleHelper::m_aiModuleSwitch[MODULE_MAX_ID];

void CModuleHelper::RegisterSessionManager(CSessionManager* pSessionManager)
{
    m_pSessionManager = pSessionManager;
}

void CModuleHelper::RegisterZoneProtocolEngine(CGameProtocolEngine* pZoneProtocolEngine)
{
    m_pZoneProtocolEngine = pZoneProtocolEngine;
}

void CModuleHelper::RegisterZoneMsgTransceiver(CGameMsgTransceiver* pZoneMsgTransceiver)
{
    m_pZoneMsgTransceiver = pZoneMsgTransceiver;
}

void CModuleHelper::RegisterConfigManager(CConfigManager* pConfigManager)
{
    m_pConfigManager = pConfigManager;
}

void CModuleHelper::RegisterServerID(int iWorldID, int iZoneID, int iInstanceID)
{
    m_iWorldID = iWorldID;
    m_iZoneID = iZoneID;
    m_iInstanceID = iInstanceID;
}

// 加载模块开关
int CModuleHelper::LoadModuleConfig()
{
    CSectionConfig stConfigFile;
    int iRet = stConfigFile.OpenFile(APP_CONFIG_FILE);
    if (iRet == 0)
    {
        stConfigFile.GetItemValue("module", "SSKeyChecked", m_iCheckSSKeyEnabled, 0);
        stConfigFile.GetItemValue("Param", "IsEnablePlayerLog", m_iIsEnablePlayerLog, 0);

        const char *sModuleName[MODULE_MAX_ID] =
        {"IsGreenEnabled",
        };

        for (int i = 0; i < (int)MODULE_MAX_ID; i++)
        {
            stConfigFile.GetItemValue("ModuleSwitch", sModuleName[i], m_aiModuleSwitch[i], 1);
        }
    }

    stConfigFile.CloseFile();

    return 0;
}

int CModuleHelper::IsSSKeyCheckEnabled()
{
    return m_iCheckSSKeyEnabled;
}

void CModuleHelper::SetModuleSwitchOn(int iSwitchIndex, int iSwitch)
{
    if (iSwitchIndex < 0 || iSwitchIndex >= MODULE_MAX_ID)
    {
        return ;
    }

    m_aiModuleSwitch[iSwitchIndex] = iSwitch;
}




----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
