#ifndef __REP_THINGS_HANDLER_HPP__
#define __REP_THINGS_HANDLER_HPP__

#include "SessionDefine.hpp"
#include "Handler.hpp"
#include "RepThingsManager.hpp"

class CGameRoleObj;

class CRepThingsHandler : public IHandler
{
protected:
	 static GameProtocolMsg ms_stZoneMsg;
public:
	virtual ~CRepThingsHandler();

public:
	virtual int OnClientMsg();

private:

    //玩家对背包物品的操作
    int OnRequestOperaRepItem();

    //玩家拉取背包的详细信息
    int OnRequestGetRepInfo();

    //玩家穿戴战斗单位装备的操作
    int OnRequestWearEquipment();

    //玩家出售背包物品
    int OnRequestSellItem();

private:

    // 发送失败回复
     static int SendFailedResponse(unsigned uiMsgID, const unsigned int uiResultID, const TNetHead_V2& rstNetHead);
};

#endif
