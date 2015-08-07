#ifndef __UPDATE_ROLEINFO_HANDLER__HPP__
#define __UPDATE_ROLEINFO_HANDLER__HPP__

#include <assert.h>
#include <string.h>

#include "Handler.hpp"
#include "ZoneErrorNumDef.hpp"
#include "GameRole.hpp"


using namespace ServerLib;

class CUpdateRoleInfo_Handler: public IHandler
{
protected:
	static GameProtocolMsg m_stZoneMsg;
public:
	virtual ~CUpdateRoleInfo_Handler();

public:
	virtual int OnClientMsg();

public:
	// 同步玩家数据到数据库
    static int UpdateRoleInfo(CGameRoleObj* pRoleObj, unsigned char ucNeedResponse);

	int OnUpdateRoleInfoResponse();
};

#endif
