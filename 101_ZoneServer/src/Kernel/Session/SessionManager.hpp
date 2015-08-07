
#ifndef __SESSION_MANAGER_HPP__
#define __SESSION_MANAGER_HPP__


#include "GameSession.hpp"
#include "ConfigHelper.hpp"

class CSessionManager
{
public:
    int Initialize(bool bResume);

public:
	CGameSessionObj* CreateSession(TNetHead_V2* pNetHead, const RoleID& stRoleID);
    void DeleteSession(int iSessionID);

	CGameSessionObj* FindSessionByID(const int iSessionID);
    CGameSessionObj* FindSessionByRoleID(const RoleID& stRoleID);
};

#endif
