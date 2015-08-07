#ifndef __WORLD_ROLE_STATUS_HPP__
#define __WORLD_ROLE_STATUS_HPP__

#include "ObjAllocator.hpp"
#include "WorldGlobalDefine.hpp"

using namespace ServerLib;

//世界中保持的玩家状态信息
class CWorldRoleStatusWObj : public CObj
{
public:
	virtual ~CWorldRoleStatusWObj();
	virtual int Initialize();

public:
	void SetRoleInfo(const GameUserInfo& rstUserInfo);
	GameUserInfo& GetRoleInfo();

	void SetZoneID(unsigned short ushZoneID);
	unsigned short& GetZoneID();

	void SetRoleID(const RoleID& stRoleID);
	const RoleID& GetRoleID();

	void SetEnterType(char cEnterType)
	{
		m_cEnterType = cEnterType;
	}
	
	char GetEnterType()const
	{
		return m_cEnterType;
	}

	void SetRoleStatus(unsigned uStatus)
	{
		m_uStatus = uStatus;
	};

	unsigned GetRoleStatus()
	{
		return m_uStatus;
	};

public:

    DECLARE_DYN

private:

	RoleID m_stRoleID;

    unsigned short m_ushZoneID;

	unsigned int m_uStatus;

	GameUserInfo m_stUserInfo;

	char m_cEnterType;
};

#endif

