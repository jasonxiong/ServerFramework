
#include "GameProtocol.hpp"
#include "ZoneObjectHelper.hpp"
#include "ModuleHelper.hpp"
#include "GameUnitID.hpp"
#include "ModuleHelper.hpp"
#include "GameRole.hpp"
#include "TimeUtility.hpp"

#include "UnitUtility.hpp"

int CUnitUtility::m_iUpdateRoleIdx = -1;

void create_dump(void)
{
//#ifdef _DEBUG_ || _DEBUG || DEBUG_ || DEBUG
    static int dump_times = 0;
    if (dump_times >= 1)
    {
        return;
    }
    ++dump_times;
    int * invalid = NULL;
    if(!fork()) {
            // Crash the app in your favorite way here
        abort();  //和 kill(getpid(), SIGABRT);应该一样的
        *invalid = 42;    //应该不会到这里来了吧。
    }
//#endif
}

TUNITINFO* CUnitUtility::GetUnitInfo(const int iUnitID)
{
    CGameUnitID* pGameUnitID = GameType<CGameUnitID>::Get(iUnitID);
    if (!pGameUnitID)
    {
        return NULL;
    }

    switch(pGameUnitID->GetBindObjectType())
    {
    case EUT_ROLE:
        if (!GetRoleObj(iUnitID))
        {
            return NULL;
        }
        break;

    default:
        break;
    }

    return pGameUnitID->GetBindUnitInfo();
}

CGameRoleObj* CUnitUtility::GetRoleObj(const int iUnitID)
{
    CGameUnitID* pGameUnitID = GameType<CGameUnitID>::Get(iUnitID);
    if (!pGameUnitID)
    {
        return NULL;
    }

    int iObjectIdx = pGameUnitID->GetBindObjectIdx();

    if (pGameUnitID->GetBindObjectType() != EUT_ROLE)
    {
        return NULL;
    }

    CGameRoleObj* pRoleObj = GameTypeK32<CGameRoleObj>::GetByIdx(iObjectIdx);

    return pRoleObj;
}

bool CUnitUtility::IsEqualRole(const RoleID& rstRoleID1, const RoleID& rstRoleID2)
{
    return (rstRoleID1.uin()==rstRoleID2.uin() && rstRoleID1.uiseq()==rstRoleID2.uiseq());
}

// 获取下一个角色索引
int CUnitUtility::IterateRoleIdx()
{
    if (m_iUpdateRoleIdx < 0)
    {
        // 定位起始单位
        m_iUpdateRoleIdx = GameTypeK32<CGameRoleObj>::GetUsedHead();
    }
    else
    {
        // 定位下一个单位
        m_iUpdateRoleIdx = GameTypeK32<CGameRoleObj>::GetNextIdx(m_iUpdateRoleIdx);
    }

    return m_iUpdateRoleIdx;
}

int CUnitUtility::AllocateUnitID()
{
    int iUnitID = GameType<CGameUnitID>::Create();

    // 不使用0号UnitID
    if (iUnitID == 0)
    {
        iUnitID = AllocateUnitID();
    }

    if (iUnitID < 0)
    {
        TRACESVR("Too many Units in Zone!!\n");
    }

    TRACESVR("Allocate UnitID = %d\n", iUnitID);

    return iUnitID;
}

void CUnitUtility::FreeUnitID(const int iUnitID)
{
    CGameUnitID* pGameUnitID = GameType<CGameUnitID>::Get(iUnitID);
    if (!pGameUnitID)
    {
        return;
    }

    switch (pGameUnitID->GetBindObjectType())
    {
    case EUT_ROLE:
        {
            if (iUnitID == m_iUpdateRoleIdx)
            {
                IterateRoleIdx();
            }
            break;
        }
    default:
        {
            break;
        }
    }

    GameType<CGameUnitID>::Del(iUnitID);

    TRACESVR("Free UnitID = %d\n", iUnitID);
}

// 绑定Unit到Object
int CUnitUtility::BindUnitToObject(int iUnitID, int iObjectIdx, unsigned char ucObjectType)
{
    CGameUnitID* pGameUnitID = GameType<CGameUnitID>::Get(iUnitID);
    ASSERT_AND_LOG_RTN_INT(pGameUnitID);

    pGameUnitID->BindObject(iObjectIdx, ucObjectType);

    return 0;
}

// 创建一个单位
CObj* CUnitUtility::CreateUnit(unsigned char ucUnitType, unsigned int uiKey)
{
    // 创建单位ID
    int iUnitID = CUnitUtility::AllocateUnitID();
    if (iUnitID <= 0)
    {
        TRACESVR("Cannot create unit: type = %d, Alloc unitid fail!\n", ucUnitType);
        return NULL;
    }

    // 按类型创建单位
    CObj* pObj = NULL;
    switch (ucUnitType)
    {
    case EUT_ROLE:
        {
            pObj = (CObj*)GameTypeK32<CGameRoleObj>::CreateByKey(uiKey);
            CGameRoleObj* pRoleObj = (CGameRoleObj*)pObj;
            if (pRoleObj)
            {
                pRoleObj->GetRoleInfo().stUnitInfo.iUnitID = iUnitID;
                pRoleObj->GetRoleInfo().stUnitInfo.ucUnitType = EUT_ROLE;
                pRoleObj->SetUin(uiKey);
            }

            break;
        }
    default:
        {
            break;
        }
    }

    if (pObj)
    {
        CUnitUtility::BindUnitToObject(iUnitID, pObj->GetObjectID(), ucUnitType);
    }
    else
    {
        CUnitUtility::FreeUnitID(iUnitID);
        TRACESVR("Cannot create unit: type = %d, Alloc obj fail!\n", ucUnitType);
    }

    return pObj;
}

// 删除一个单位, 释放UnitID, 销毁对象ID.
// 对象删除是一个非常危险的行为, 因此要先将单位置为EUS_DELETED状态, 在AppTick中删除
int CUnitUtility::DeleteUnit(TUNITINFO* pUnitInfo)
{
    int iUnitID = pUnitInfo->iUnitID;
    switch (pUnitInfo->ucUnitType)
    {
    case EUT_ROLE:
        {
            CGameRoleObj* pRoleObj = CUnitUtility::GetRoleObj(iUnitID);
            ASSERT_AND_LOG_RTN_INT(pRoleObj);

            GameTypeK32<CGameRoleObj>::DeleteByKey(pRoleObj->GetRoleID().uin());
            break;
        }

    default:
        {
            break;
        }
    }


    // 删除UnitID
    CUnitUtility::FreeUnitID(iUnitID);

    return 0;
}

// 获取角色单位
CGameRoleObj* CUnitUtility::GetRoleByUin(const int uiUin)
{
    return GameTypeK32<CGameRoleObj>::GetByKey(uiUin);
}

CGameRoleObj* CUnitUtility::GetRoleByID(const RoleID& rstRoleID)
{
    CGameRoleObj* pRoleObj = GetRoleByUin(rstRoleID.uin());
    if (!pRoleObj || pRoleObj->GetRoleID().uiseq() != rstRoleID.uiseq())
    {
        return NULL;
    }

    return pRoleObj;
}

// 设置单位状态
void CUnitUtility::SetUnitStatus(TUNITINFO* pstUnit, EUnitStatus enStatus)
{
    ASSERT_AND_LOG_RTN_VOID(pstUnit);

    pstUnit->uiUnitStatus |= enStatus;
}

// 清除单位状态
void CUnitUtility::ClearUnitStatus(TUNITINFO* pstUnit, EUnitStatus enStatus)
{
    ASSERT_AND_LOG_RTN_VOID(pstUnit);

    pstUnit->uiUnitStatus &= ~(enStatus);
}

