#ifndef __KICKROLE_WORLD_HANDLER_HPP__
#define __KICKROLE_WORLD_HANDLER_HPP__

#include "Handler.hpp"
#include "WorldGlobalDefine.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"

//处理踢人的消息处理
class CKickRoleWorldHandler : public IHandler
{
protected:
	static GameProtocolMsg ms_stGameMsg;
public:
    virtual ~CKickRoleWorldHandler();

public:
    virtual int OnClientMsg(const void* pstMsg);

private:
    // A1. 处理来自Zone的踢人请求
    int OnZoneRequestKickRole();

private:
    // B2: 处理来自Zone的踢人回复
    int OnZoneResponseKickRole();

private:
    GameProtocolMsg* m_pRequestMsg;
};

#endif
