#include "GameProtocol.hpp"
#include "ObjAllocator.hpp"
#include "ZoneObjectAllocator.hpp"
#include "ConfigManager.hpp"
#include "ZoneObjectHelper.hpp"
#include "GameRole.hpp"
#include "GameUnitID.hpp"
#include "GameSession.hpp"
#include "ConfigHelper.hpp"
#include "ModuleHelper.hpp"
#include "FightUnitObj.hpp"
#include "BattlefieldObj.hpp"
#include "CombatUnitObj.hpp"
#include "UnitBufferObj.hpp"
#include "CombatTrapObj.hpp"

template <typename TYPE_Obj> 
CObjAllocator* AllocateShmObj(CSharedMemory& rstSharedMemory, const int iObjCount, bool bResume)
{
    CObjAllocator* pAllocator = NULL;

    if (!bResume)
    {
	    pAllocator = CObjAllocator::CreateByGivenMemory(
		    (char*)rstSharedMemory.GetFreeMemoryAddress(),
		    rstSharedMemory.GetFreeMemorySize(),
		    sizeof(TYPE_Obj),
		    iObjCount,
		    TYPE_Obj::CreateObject);
        LOGDEBUG("MEMLOG Free:%zu,size:%zu,num:%d,all:%zu\n",
            rstSharedMemory.GetFreeMemorySize(),
            sizeof(TYPE_Obj),
            iObjCount,
            iObjCount * sizeof(TYPE_Obj));
    }
    else
    {
        pAllocator = CObjAllocator::ResumeByGivenMemory(
            (char*)rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            sizeof(TYPE_Obj),
            iObjCount,
            TYPE_Obj::CreateObject);
    }

    if (!pAllocator)
    {
        LOGERROR("Failed to allocate object segment\n");
        return NULL;
    }

    if (bResume)
    {
        // 恢复使用中的obj
        int iUsedIdx = pAllocator->GetUsedHead();
        while (iUsedIdx != -1)
        {
            CObj* pObj = pAllocator->GetObj(iUsedIdx);
            pObj->Resume();
            iUsedIdx = pAllocator->GetIdx(iUsedIdx)->GetNextIdx();
        }
    }

    rstSharedMemory.UseShmBlock(CObjAllocator::CountSize(sizeof(TYPE_Obj), iObjCount));

	int iRet = GameType<TYPE_Obj>::RegisterAllocator(pAllocator);
	if(iRet < 0)
	{
		return NULL;
	}

	return pAllocator;
}

template <typename TYPE_Obj> 
CObjAllocator* AllocateShmObjK32(CSharedMemory& rstSharedMemory, const int iObjCount, bool bResume)
{
	CObjAllocator* pAllocator = NULL;
    
    if (!bResume)
    {
        pAllocator = CObjAllocator::CreateByGivenMemory(
		    (char*)rstSharedMemory.GetFreeMemoryAddress(),
		    rstSharedMemory.GetFreeMemorySize(),
		    sizeof(TYPE_Obj),
		    iObjCount,
		    TYPE_Obj::CreateObject);
        LOGDEBUG("MEMLOG Free:%zu,size:%zu,num:%d,all:%zu\n",
            rstSharedMemory.GetFreeMemorySize(),
            sizeof(TYPE_Obj),
            iObjCount,
            iObjCount * sizeof(TYPE_Obj));
    }
    else
    {
        pAllocator = CObjAllocator::ResumeByGivenMemory(
            (char*)rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            sizeof(TYPE_Obj),
            iObjCount,
            TYPE_Obj::CreateObject);
    }

    if (!pAllocator)
    {
        LOGERROR("Failed to allocate object segment\n");
        return NULL;
    }

    if (bResume)
    {
        // 恢复使用中的obj
        int iUsedIdx = pAllocator->GetUsedHead();
        while (iUsedIdx != -1)
        {
            CObj* pObj = pAllocator->GetObj(iUsedIdx);
            pObj->Resume();
            iUsedIdx = pAllocator->GetIdx(iUsedIdx)->GetNextIdx();
        }
    }

    rstSharedMemory.UseShmBlock(CObjAllocator::CountSize(sizeof(TYPE_Obj), iObjCount));

    CHashMap_K32* pHashMap = NULL; //节点数量为对象数量两倍

    if (!bResume)
    {
        pHashMap = CHashMap_K32::CreateHashMap(
            (char*)rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            iObjCount << 1);
    }
    else
    {
        pHashMap = CHashMap_K32::ResumeHashMap(
            (char*)rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            iObjCount << 1);
    }

    if (!pHashMap)
    {
        LOGERROR("Failed to allocate hashmap\n");
        return NULL;
    }

    rstSharedMemory.UseShmBlock(CHashMap_K32::CountSize(iObjCount << 1));

	int iRet = GameTypeK32<TYPE_Obj>::RegisterHashAllocator(pAllocator, pHashMap);
	if(iRet < 0)
	{
		return NULL;
	}

	return pAllocator;
}

template <typename TYPE_Obj>
CObjAllocator* AllocateShmObjK64(CSharedMemory& rstSharedMemory, const int iObjCount, bool bResume)
{
    CObjAllocator* pAllocator = NULL;
    
    if (!bResume)
    {
        pAllocator = CObjAllocator::CreateByGivenMemory(
            (char*)rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            sizeof(TYPE_Obj),
            iObjCount,
            TYPE_Obj::CreateObject);
    }
    else
    {
        pAllocator = CObjAllocator::ResumeByGivenMemory(
            (char*)rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            sizeof(TYPE_Obj),
            iObjCount,
            TYPE_Obj::CreateObject);
    }

    if (!pAllocator)
    {
        LOGERROR("Failed to allocate object segment\n");
        return NULL;
    }

    if (bResume)
    {
        // 恢复使用中的obj
        int iUsedIdx = pAllocator->GetUsedHead();
        while (iUsedIdx != -1)
        {
            CObj* pObj = pAllocator->GetObj(iUsedIdx);
            pObj->Resume();
            iUsedIdx = pAllocator->GetIdx(iUsedIdx)->GetNextIdx();
        }
    }

    rstSharedMemory.UseShmBlock(CObjAllocator::CountSize(sizeof(TYPE_Obj), iObjCount));

    CHashMap_K64* pHashMap = NULL; //节点数量为对象数量两倍

    if (!bResume)
    {
        pHashMap = CHashMap_K64::CreateHashMap(
            (char*) rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            iObjCount << 1);
    }
    else
    {
        pHashMap = CHashMap_K64::ResumeHashMap(
            (char*) rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            iObjCount << 1);
    }

    if (!pHashMap)
    {
        LOGERROR("Failed to allocate hashmap\n");
        return NULL;
    }

    rstSharedMemory.UseShmBlock(CHashMap_K64::CountSize(iObjCount << 1));

    int iRet = GameTypeK64<TYPE_Obj>::RegisterHashAllocator(pAllocator, pHashMap);
    if(iRet < 0)
    {
        return NULL;
    }

    return pAllocator;
}


CZoneObjectAllocator::CZoneObjectAllocator()
{
    m_iObjTotalSize = 0;
}

int CZoneObjectAllocator::Initialize(bool bResumeMode)
{
	int iRet = -1;

	CObjAllocator* pAllocator = NULL;

    size_t iSharedMmorySize = GetObjTotalSize();

	iRet = m_stSharedMemory.CreateShmSegmentByKey(GenerateServerShmKey(GAME_SERVER_ZONE, 
				(CModuleHelper::GetZoneID() + 1)*10 + CModuleHelper::GetInstanceID()), 
			iSharedMmorySize);
	if(iRet < 0)
	{
		LOGERROR("Create shm fail: %d\n", iRet);
		return -1;
	}

    pAllocator = AllocateShmObjK32<CGameSessionObj>(m_stSharedMemory, MAX_GAMESESSION_NUMBER, bResumeMode);
    if (!pAllocator)
    {
		LOGERROR("Alloc sessionobj fail!\n");
        return -2;
    }
	
	pAllocator = AllocateShmObjK32<CGameRoleObj>(m_stSharedMemory, MAX_ROLE_OBJECT_NUMBER_IN_ZONE, bResumeMode);
	if(!pAllocator)
	{
		LOGERROR("Alloc role fail!\n");
		return -5;
	}

    pAllocator = AllocateShmObj<CGameUnitID>(m_stSharedMemory, MAX_UNIT_NUMBER_IN_ZONE, bResumeMode);
    if(!pAllocator)
    {
        LOGERROR("Alloc GameUnit fail!\n");
        return -6;
    }

    pAllocator = AllocateShmObj<CFightUnitObj>(m_stSharedMemory, MAX_FIGHT_UNIT_NUMBER_IN_ZONE, bResumeMode);
    if(!pAllocator)
    {
        LOGERROR("Alloc FightUnitObj fail!\n");
        return -7;
    }

    //玩家战斗战场信息
    pAllocator = AllocateShmObj<CBattlefieldObj>(m_stSharedMemory, MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE, bResumeMode);
	if(!pAllocator)
	{
		LOGERROR("Alloc battlefield obj info fail!\n");
		return -11;
	}

    //战斗时战斗单位信息
    pAllocator = AllocateShmObj<CCombatUnitObj>(m_stSharedMemory, MAX_COMBAT_UNIT_OBJECT_NUMBER_IN_ZONE, bResumeMode);
	if(!pAllocator)
	{
		LOGERROR("Alloc combat unit obj fail!\n");
		return -12;
	}

    //战斗时单位的Buff信息
    pAllocator = AllocateShmObj<CUnitBufferObj>(m_stSharedMemory, MAX_COMBAT_BUFF_OBJECT_NUMBER_IN_ZONE, bResumeMode);
	if(!pAllocator)
	{
		LOGERROR("Alloc unit buffer obj fail!\n");
		return -13;
	}

    //战斗战场机关信息
    pAllocator = AllocateShmObj<CCombatTrapObj>(m_stSharedMemory, MAX_COMBAT_TRAP_OBJECT_NUMBER_IN_ZONE, bResumeMode);
    if(!pAllocator)
    {
        LOGERROR("Alloc combat trap obj fail!\n");
        return -15;
    }

    TRACESVR("Obj shared memory: Total allocated %lu, Used %lu, Free %lu\n", iSharedMmorySize, GetObjTotalSize(), m_stSharedMemory.GetFreeMemorySize());

	return 0;
}

size_t CZoneObjectAllocator::GetObjTotalSize()
{
    if (m_iObjTotalSize > 0)
    {
        return m_iObjTotalSize;
    }

    TRACESVR("********************** OBJECT MEMORY STATICS *********************\n");

    // Session
    size_t iSessionSize = CObjAllocator::CountSize(sizeof(CGameSessionObj), MAX_GAMESESSION_NUMBER);
    m_iObjTotalSize += iSessionSize;
    TRACESVR("Session: Number = %d, UnitSize = %lu, Total = %lu\n", 
        MAX_GAMESESSION_NUMBER, sizeof(CGameSessionObj), iSessionSize);

    // Session Hash
    size_t iSessionHashSize = CHashMap_K32::CountSize(MAX_GAMESESSION_NUMBER << 1);
    m_iObjTotalSize += iSessionHashSize;
    TRACESVR("SessionHash: Number = %d, Size = %lu\n", 
        MAX_GAMESESSION_NUMBER << 1, iSessionHashSize);

    // 角色对象
    size_t iRoleSize = CObjAllocator::CountSize(sizeof(CGameRoleObj), MAX_ROLE_OBJECT_NUMBER_IN_ZONE); 
    m_iObjTotalSize += iRoleSize;
    TRACESVR("Role: Number = %d, UnitSize = %lu, Total = %lu\n", 
        MAX_ROLE_OBJECT_NUMBER_IN_ZONE, sizeof(CGameRoleObj), iRoleSize);

    // 角色Hash
    size_t iRoleHashSize = CHashMap_K32::CountSize(MAX_ROLE_OBJECT_NUMBER_IN_ZONE << 1);
    m_iObjTotalSize += iRoleHashSize;
    TRACESVR("RoleHash: Number = %d,  Size = %lu\n", 
        MAX_ROLE_OBJECT_NUMBER_IN_ZONE << 1, iRoleHashSize);

    //场景单位的UnitID
    size_t iSceneUnitSize = CObjAllocator::CountSize(sizeof(CGameUnitID), MAX_UNIT_NUMBER_IN_ZONE); 
    m_iObjTotalSize += iSceneUnitSize;
    TRACESVR("Unit: Number = %d, UnitSize = %lu, Total = %lu\n", 
        MAX_UNIT_NUMBER_IN_ZONE, sizeof(CGameUnitID), iSceneUnitSize);

    //战斗单位的ID
    size_t iFightUnitSize = CObjAllocator::CountSize(sizeof(CFightUnitObj), MAX_FIGHT_UNIT_NUMBER_IN_ZONE); 
    m_iObjTotalSize += iFightUnitSize;
    TRACESVR("FightUnit: Number = %d, UnitSize = %lu, Total = %lu\n", 
        MAX_FIGHT_UNIT_NUMBER_IN_ZONE, sizeof(CFightUnitObj), iFightUnitSize);

    //战斗战场对象
    size_t iBattlefieldObjSize = CObjAllocator::CountSize(sizeof(CBattlefieldObj), MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE);
    m_iObjTotalSize += iBattlefieldObjSize;
    TRACESVR("battlefield obj: Number = %d, UnitSize = %lu, Total = %lu\n",
             MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE, sizeof(CBattlefieldObj), iBattlefieldObjSize);

    //战斗时战斗单位对象
    size_t iCombatUnitObjSize = CObjAllocator::CountSize(sizeof(CCombatUnitObj), MAX_COMBAT_UNIT_OBJECT_NUMBER_IN_ZONE);
    m_iObjTotalSize += iCombatUnitObjSize;
    TRACESVR("combat unit obj: Number = %d, UnitSize = %lu, Total = %lu\n",
             MAX_COMBAT_UNIT_OBJECT_NUMBER_IN_ZONE, sizeof(CCombatUnitObj), iCombatUnitObjSize);

    //战斗Buff对象
    size_t iUnitBufferObjSize = CObjAllocator::CountSize(sizeof(CUnitBufferObj), MAX_COMBAT_BUFF_OBJECT_NUMBER_IN_ZONE);
    m_iObjTotalSize += iUnitBufferObjSize;
    TRACESVR("unit buff obj: Number = %d, UnitSize = %lu, Total = %lu\n",
             MAX_COMBAT_BUFF_OBJECT_NUMBER_IN_ZONE, sizeof(CUnitBufferObj), iUnitBufferObjSize);

    //战斗战场机关对象
    size_t iCombatTrapObjSize = CObjAllocator::CountSize(sizeof(CCombatTrapObj), MAX_COMBAT_TRAP_OBJECT_NUMBER_IN_ZONE);
    m_iObjTotalSize += iCombatTrapObjSize;
    TRACESVR("combat trap obj: Number = %d, UnitSize = %lu, Total = %lu\n",
             MAX_COMBAT_TRAP_OBJECT_NUMBER_IN_ZONE, sizeof(CCombatTrapObj), iCombatTrapObjSize);

    // 总计
    TRACESVR("Total Memory: %luB, %luMB\n", m_iObjTotalSize, m_iObjTotalSize/1024/1024);
    TRACESVR("*****************************************************************\n");

    return m_iObjTotalSize;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
