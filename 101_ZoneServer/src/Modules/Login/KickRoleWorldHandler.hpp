// 处理踢人事件

#ifndef __KICKROLE_WORLD_HANDLER_HPP__
#define __KICKROLE_WORLD_HANDLER_HPP__

#include "Handler.hpp"
#include "ZoneErrorNumDef.hpp"

class CGameSessionObj;

class CKickRoleWorldHandler : public IHandler
{
protected:
	static GameProtocolMsg ms_stZoneMsg;
public:
    virtual ~CKickRoleWorldHandler();

public:
    virtual int OnClientMsg();

public:
    // 返回成功回复
    static int SendSuccessfullResponse(const World_KickRole_Request& rstResp);

    // 返回失败回复
    static int SendFailedResponse(const World_KickRole_Request& rstResp);

private:
    // 处理来自World的下线通知, 被踢下线
    int OnRequestKickRole();

    // 处理来自World的下线回复, 踢人下线
    int OnResponseKickRole();

	//处理来自封号的踢人下线
	int OnForbidKickRole();

private:
    // 从World下拉玩家数据
    int FetchRoleFromWorldServer(const RoleID& stRoleID, char cEnterType);    
};

#endif
