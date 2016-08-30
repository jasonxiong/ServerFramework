#ifndef __FETCH_ROLE_WORLD_HANDLER_HPP__
#define __FETCH_ROLE_WORLD_HANDLER_HPP__
#include "Handler.hpp"
#include "ZoneErrorNumDef.hpp"

class CFetchRoleWorldHandler : public IHandler
{
protected:
	static GameProtocolMsg m_stZoneMsg;
public:
    virtual ~CFetchRoleWorldHandler();

public:
    virtual int OnClientMsg();

private:
    int OnFetchRole();

    ////////////////////////////////////////////////////////
    // 登录角色
    int LoginRole();

    // 初始化角色数据
    int InitRoleData();
    
    // 登录后初始化
    int InitRoleAfterLogin();
		
private:

    int QueryMailNumber();
    int FetchGameFriend();
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
