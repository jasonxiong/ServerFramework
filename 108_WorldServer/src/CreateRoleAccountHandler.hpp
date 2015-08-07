#ifndef __CREATEROLE_ACCOUNT_HANDLER_HPP__
#define __CREATEROLE_ACCOUNT_HANDLER_HPP__

#include "Handler.hpp"
#include "WorldGlobalDefine.hpp"
#include "GameProtocol.hpp"
#include "ConfigManager.hpp"

class CCreateRoleAccountHandler : public IHandler
{
public:
    virtual ~CCreateRoleAccountHandler();

public:
    virtual int OnClientMsg(const void* pMsg);

private:
    int OnRequestCreateRoleAccount();
    int OnResponseCreateRoleAccount();
    int SendCreateRoleRequestToDBSvr();
    int SendCreateRoleResponseToAccount();

    int InitBirthInfo(World_CreateRole_Request& rstRequest);

private:
    int InitFightUnitInfo(const SRoleBirthConfig& stConfig, World_CreateRole_Request& rstRequest);

    //初始化一个战斗单位
    int InitOneFightUnitInfo(int iConfigID, OneFightUnitInfo& stOneFightUnit);

private:
    GameProtocolMsg* m_pMsg;
    GameProtocolMsg m_stWorldMsg;
};

#endif


