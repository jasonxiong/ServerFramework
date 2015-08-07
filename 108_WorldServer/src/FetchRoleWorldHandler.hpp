#ifndef __FETCHROLE_WORLD_HANDLER_HPP__
#define __FETCHROLE_WORLD_HANDLER_HPP__

#include "Handler.hpp"
#include "WorldGlobalDefine.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"

class CFetchRoleWorldHandler : public IHandler
{
public:
	virtual ~CFetchRoleWorldHandler();

public:
	virtual int OnClientMsg(const void* pMsg);

public:
    // 返回角色查询数据
    int SendFetchRoleResponse(const RoleID& stRoleID, int iReqID);

    // 返回失败查询
    int SendFailedFetchRoleResponse(const RoleID& stRoleID, int iReqID);

private:
    // 查询请求处理入口
	int OnRequestFetchRoleWorld();

    //查询请求返回到World的处理入口
    int OnResponseFetchRoleWorld();

private:
	GameProtocolMsg* m_pMsg;

	GameProtocolMsg m_stWorldMsg;
};

#endif


