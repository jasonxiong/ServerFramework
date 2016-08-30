#include "AppTick.hpp"
#include "LogAdapter.hpp"
#include "ZoneOssLog.hpp"
#include "TimeUtility.hpp"
#include "ObjStatistic.hpp"
#include "PerformanceStatistic.hpp"
#include "ModuleHelper.hpp"
#include "ZoneObjectHelper.hpp"
#include "AppLoop.hpp"
#include "BattlefieldObj.hpp"
#include "CombatUnitObj.hpp"
#include "UnitBufferObj.hpp"
#include "CombatTrapObj.hpp"

CAppTick::CAppTick()
{
}

int CAppTick::Initialize(bool bResume)
{
    m_OnlineStatTick.Initialize(bResume);

	if (bResume)
	{
		return 0;
	}

    m_iLastStaticsTick = time(NULL);

	return 0;
}

static const char * szObjectTypeName[] = {"CGameRoleObj", "CFightUnitObj", "CNPCTradeObj", "CBattlefieldObj", "CCombatUnitObj", "CUnitBufferObj", "CCombatTrapObj"}; 

const int iObjectTypeNum = sizeof(szObjectTypeName)/sizeof(szObjectTypeName[0]);
int CAppTick::CountObjStat()
{
	int i = 0;

	ObjectStatisticSingleton::Instance()->SetObjectStatName(szObjectTypeName, iObjectTypeNum);

	ObjectStatisticSingleton::Instance()->AddObjectStat(i++, GameTypeK32<CGameRoleObj>::GetUsedObjNumber());

	ObjectStatisticSingleton::Instance()->AddObjectStat(i++, GameType<CFightUnitObj>::GetUsedObjNumber());
	ObjectStatisticSingleton::Instance()->AddObjectStat(i++, GameType<CBattlefieldObj>::GetUsedObjNumber());
	ObjectStatisticSingleton::Instance()->AddObjectStat(i++, GameType<CCombatUnitObj>::GetUsedObjNumber());
	ObjectStatisticSingleton::Instance()->AddObjectStat(i++, GameType<CUnitBufferObj>::GetUsedObjNumber());
	ObjectStatisticSingleton::Instance()->AddObjectStat(i++, GameType<CCombatTrapObj>::GetUsedObjNumber());

	//清0
	CTimeValue stZero(0, 0);
	CAppLoop::m_stLotusMsgMaxProsessTime = stZero;
	CAppLoop::m_stWorldMsgMaxProcessTime = stZero;
	CAppLoop::m_stTickMaxProcessTime = stZero;

	return 0;
}

int CAppTick::OnTick()
{
	CModuleHelper::GetUnitEventManager()->NotifyTick();

	m_OnlineStatTick.OnTick();

	// 一分钟Tick
	int iSlapTime = CTimeUtility::m_uiTimeTick - m_iLastStaticsTick;
	if (iSlapTime >= 60)
    {
		CountObjStat();		
		ObjectStatisticSingleton::Instance()->Print();
        ObjectStatisticSingleton::Instance()->Reset();				
		
        MsgStatisticSingleton::Instance()->Print();
        MsgStatisticSingleton::Instance()->Reset();

		PerformanceStatisticSingleton::Instance()->Print();
		PerformanceStatisticSingleton::Instance()->Reset();

        m_iLastStaticsTick = CTimeUtility::m_uiTimeTick;
    }
    
	return 0;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
