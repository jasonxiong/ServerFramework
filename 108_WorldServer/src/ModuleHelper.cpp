#include "SectionConfig.hpp"
#include "AppDefW.hpp"

#include "ModuleHelper.hpp"

CWorldProtocolEngineW* CModuleHelper::m_pWorldProtocolEngine = 0;
CAppLoopW* CModuleHelper::m_pAppLoop = 0;

CConfigManager* CModuleHelper::m_pConfigManager = 0;
int CModuleHelper::m_iWorldID;
CZoneTick* CModuleHelper::m_pZoneTick;
int CModuleHelper::m_iFullWorldNum = 0;
int CModuleHelper::m_iBusyWorldNum = 0;
int CModuleHelper::m_iIsZoneBalanceEnabled = 0;

CWorldProtocolEngineW* CModuleHelper::GetWorldProtocolEngine()
{
	return m_pWorldProtocolEngine;
};

void CModuleHelper::RegisterWorldProtocolEngine(CWorldProtocolEngineW* pWorldProtocolEngine)
{
	m_pWorldProtocolEngine = pWorldProtocolEngine;
};

CAppLoopW* CModuleHelper::GetAppLoop()
{
    return m_pAppLoop;
}

void CModuleHelper::RegisterAppLoop(CAppLoopW* pAppLoop)
{
    m_pAppLoop = pAppLoop;
}

void CModuleHelper::RegisterConfigManager(CConfigManager* pConfigManager)
{
    m_pConfigManager = pConfigManager;
}

void CModuleHelper::LoadConfig()
{
    CSectionConfig stConfigFile;
    int iRet = stConfigFile.OpenFile(APP_CONFIG_FILE);
    if (iRet == 0)
    {
		stConfigFile.GetItemValue("WorldStatus", "FullWorldNum", m_iFullWorldNum, (MAX_ROLE_OBJECT_NUMBER_IN_ZONE * 4 / 5));

		stConfigFile.GetItemValue("WorldStatus", "BusyWorldNum", m_iBusyWorldNum, (MAX_ROLE_OBJECT_NUMBER_IN_ZONE / 2));
        
		stConfigFile.GetItemValue("ModuleSwitch", "IsZoneBalanceEnabled", m_iIsZoneBalanceEnabled, 0);

		stConfigFile.CloseFile();
    }

	else
	{
		m_iFullWorldNum = MAX_ROLE_OBJECT_NUMBER_IN_ZONE * 4 / 5;
		m_iBusyWorldNum = MAX_ROLE_OBJECT_NUMBER_IN_ZONE / 2;
	}
}

int CModuleHelper::GetFullWorldNum()
{
	return m_iFullWorldNum;
}

int CModuleHelper::GetBusyWorldNum()
{
	return m_iBusyWorldNum;
}

bool CModuleHelper::IsRealGM(unsigned int uiUin)
{
	if (uiUin < MAX_TEST_UIN)
	{
		return false;
	}

	//todo jasonxiong 暂时先去掉这部分逻辑，后面再统一添加
	// GM帐号不做排行榜处理
	/*
	bool bGM = GetConfigManager()->GetWhiteListConfigManager().IsInGMList(uiUin);
	if (bGM)
	{
		return true;
	}
	*/

	return false;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
