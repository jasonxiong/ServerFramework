#ifndef __LOGIN_HANDLER_HPP__
#define __LOGIN_HANDLER_HPP__

#include "Handler.hpp"
#include "ZoneErrorNumDef.hpp"

class CGameRoleObj;
class CGameSessionObj;

class CLoginHandler : public IHandler
{
protected:
	 static GameProtocolMsg ms_stZoneMsg;
public:
	virtual ~CLoginHandler();

public:
	virtual int OnClientMsg();

private:
    virtual int SecurityCheck();

public:
    // 登录入口
     static int LoginRole(const RoleID& stRoleID, TNetHead_V2* pNetHead, const Zone_LoginServer_Request& rstRequest);

    // 登录成功
     static int LoginOK(unsigned int uiUin, bool bNeedResponse = true);

    // 登录失败
     static int LoginFailed(const TNetHead_V2& rstNetHead);
     
     //通过World将相同uin的已经登录的号踢下线
     static int KickRoleFromWorldServer(const RoleID& stRoleID, int iFromSessionID);

private:
    int OnRequestLoginServer();

    //通知角色登录
    static int NotifyRoleLogin(CGameRoleObj* pstRoleObj);

    // 将本线上的玩家踢下线
    int KickRoleFromZoneServer();
    
private:

    // 发送失败回复
     static int SendFailedResponse(const unsigned int uiResultID, const TNetHead_V2& rstNetHead);

     // 发送成功回复
     static int SendSuccessfulResponse(CGameRoleObj* pLoginRoleObj);
};

#endif


