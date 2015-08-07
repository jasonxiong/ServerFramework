
#ifndef __LIST_ZONE_HANDLER_HPP__
#define __LIST_ZONE_HANDLER_HPP__

#include "Handler.hpp"
#include "WorldGlobalDefine.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"

//拉取当前所有可用的分区信息

class CListZoneHandler : public IHandler
{
protected:
	static GameProtocolMsg m_stWorldMsg;
public:
    CListZoneHandler(void);
public:
    ~CListZoneHandler(void);

public:
    virtual int OnClientMsg(const void* pMsg);

private:
    GameProtocolMsg* m_pRequestMsg;
};

#endif

