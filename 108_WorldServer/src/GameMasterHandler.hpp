
#ifndef __GAMEMASTER_HANDLER_HPP__
#define __GAMEMASTER_HANDLER_HPP__

#include "Handler.hpp"
#include "WorldGlobalDefine.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"


class CGameMasterHandler : public IHandler
{
public:
    virtual ~CGameMasterHandler();

public:
    virtual int OnClientMsg(const void* pMsg);

private:

    int OnRequestGameMaster();
    int OnResponseGameMaster();

private:
    GameProtocolMsg* m_pRequestMsg;
    GameProtocolMsg m_stWorldMsg;
};

#endif


