#ifndef __LOGOUT_HANDLER_HPP__
#define __LOGOUT_HANDLER_HPP__

#include "Handler.hpp"
#include "WorldGlobalDefine.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"

class CLogoutHandler : public IHandler
{
protected:
	static GameProtocolMsg m_stWorldMsg;
public:
    CLogoutHandler(void);
public:
    ~CLogoutHandler(void);

public:
    virtual int OnClientMsg(const void* pMsg);

public:
    // 通知Zone服务器角色下线
    static int LogoutRole(int iZoneID, const unsigned int uin);

public:
    // 通告角色下线消息
    static int LogoutNotify(CWorldRoleStatusWObj* pRoleObj);

private:
    int OnLogoutNotify();

private:
    GameProtocolMsg* m_pRequestMsgWGS;
};

#endif


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
