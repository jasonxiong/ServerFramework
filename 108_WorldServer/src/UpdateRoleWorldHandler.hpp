#ifndef __UPDATEROLE_WORLD_HANDLER_HPP__
#define __UPDATEROLE_WORLD_HANDLER_HPP__

#include "Handler.hpp"
#include "WorldGlobalDefine.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"

class CUpdateRoleWorldHandler : public IHandler
{
public:
	virtual ~CUpdateRoleWorldHandler();

public:
	virtual int OnClientMsg(const void* pMsg);

private:
    // 更新数据处理入口
	int OnRequestUpdateRoleWorld();

    //更新数据返回的处理入口
    int OnResponseUpdateRoleWorld();

    // 返回失败信息
	int SendUpdateRoleResponseWGS(const RoleID& stRoleID, int iZoneID, unsigned int uiResultID);

private:
	GameProtocolMsg* m_pMsg;

	GameProtocolMsg m_stWorldMsg;
};

#endif



----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
