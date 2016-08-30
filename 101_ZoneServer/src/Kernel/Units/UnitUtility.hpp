#ifndef __UNIT_UNTILITY_HPP__
#define __UNIT_UNTILITY_HPP__

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneMsgHelper.hpp"

using namespace ServerLib;

class CUnitUtility
{
public:
    // 从UnitID获取单位属性, 单位对象, 以及所属场景
	static TUNITINFO* GetUnitInfo(const int iUnitID);

    static CGameRoleObj* GetRoleObj(const int iUnitID);

    // 获取角色单位
    static CGameRoleObj* GetRoleByUin(const int uiUin);
    static CGameRoleObj* GetRoleByID(const RoleID& rstRoleID);

	static bool IsEqualRole(const RoleID& rstRoleID1, const RoleID& rstRoleID2);

public:
    // 单位状态方法

    // 设置单位状态
    static void SetUnitStatus(TUNITINFO* pstUnit, EUnitStatus enStatus);

    // 清除单位状态
    static void ClearUnitStatus(TUNITINFO* pstUnit, EUnitStatus enStatus);

    // 判断单位状态
    static inline bool IsUnitStatusSet(TUNITINFO* pstUnit, EUnitStatus enStatus)
    {
        ASSERT_AND_LOG_RTN_BOOL(pstUnit);

        return pstUnit->uiUnitStatus & enStatus;
    }

    // 在线状态
    static inline bool IsUnitOnline(TUNITINFO* pstUnit)
    {
        return IsUnitStatusSet(pstUnit, EGUS_ONLINE);
    }

    // 设置删除状态
    static inline void SetDeletedStatus(TUNITINFO* pstUnit)
    {
        ASSERT_AND_LOG_RTN_VOID(pstUnit);

        SetUnitStatus(pstUnit, EGUS_DELETE);
    }

public:
    // 分配和释放UnitID
    static int AllocateUnitID();
    static void FreeUnitID(const int iUnitID);

    // 在GameUnitID中绑定单位和场景
    static int BindUnitToObject(int iUnitID, int iObjectIdx, unsigned char ucObjectType);

    // 创建一个单位对象
    static CObj* CreateUnit(unsigned char ucUnitType, unsigned int uiKey = 0);

	// 删除一个单位, 释放UnitID, 销毁对象ID. 
	// 对象删除是一个非常危险的行为, 因此要先将单位置为EUS_DELETED状态, 在AppTick中删除
    static int DeleteUnit(TUNITINFO* pUnitInfo);

public:
    // 为了精细控制每种单位的遍历进度, 对系统单位分别计数

    // 获取下一个单位索引
    static int IterateRoleIdx();
    static int IterateEggIdx();

private:
    static int m_iUpdateRoleIdx;
};

void create_dump(void);
#endif



----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
