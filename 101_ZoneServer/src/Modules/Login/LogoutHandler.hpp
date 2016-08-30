#ifndef __LOGOUT_HANDLER__HPP__
#define __LOGOUT_HANDLER__HPP__


#include "Handler.hpp"
#include "ZoneErrorNumDef.hpp"

class CGameRoleObj;
class CGameSessionObj;

class CLogoutHandler : public IHandler
{
protected:
	static GameProtocolMsg m_stZoneMsg;
public:
    virtual ~CLogoutHandler();

public:
    virtual int OnClientMsg();

private:
    virtual int SecurityCheck();

public:
    // 玩家下线入口
    static int LogoutRole(CGameRoleObj* pRoleObj, int iReason);

    // 下线三部曲
    // 1. 准备阶段
    static int LogoutPrepare(CGameRoleObj* pRoleObj);

    // 2. 同步数据

    // 3. 删除数据, 断开连接
    static int LogoutAction(CGameRoleObj* pRoleObj);

    // 通知world下线
    static int NotifyLogoutToWorld(const RoleID& stRoleID);

private:
    int OnRequestLogout();

    static void NotifyRoleLogout(CGameRoleObj* pstRoleObj);

    static int SendFailedResponse(const unsigned int uiResultID, const TNetHead_V2& rstNetHead);

    static int SendSuccessfulResponse(CGameRoleObj* pRoleObj);
};
#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
