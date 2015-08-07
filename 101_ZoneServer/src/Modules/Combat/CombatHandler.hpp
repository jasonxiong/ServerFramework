#ifndef __COMBAT_HANDLER_HPP__
#define __COMBAT_HANDLER_HPP__

#include "SessionDefine.hpp"
#include "Handler.hpp"
#include "RepThingsManager.hpp"

class CGameRoleObj;

class CCombatHandler : public IHandler
{
protected:
	 static GameProtocolMsg ms_stZoneMsg;
public:
	virtual ~CCombatHandler();

public:
	virtual int OnClientMsg();

private:

    //处理开始战斗的请求
    int OnRequestBeginCombat();

    //处理主角移动的请求
    int OnRequestMovePosition();

    //处理主角行动的请求
    int OnRequestCombatAction();

    //更新伙伴AI的请求
    int OnRequestUpdatePartnerAI();

    //发起PVP战斗的请求
    int OnRequestStartPVPCombat();

    //接受PVP战斗的请求
    int OnRequestAcceptPVPCombat();
    
    //设置战斗阵型的请求
    int OnRequestSetCombatForm();

private:

    // 发送失败回复
    int SendFailedResponse(unsigned uiMsgID, const unsigned int uiResultID, const TNetHead_V2& rstNetHead);
};

#endif
