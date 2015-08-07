
#ifndef __DELETEROLE_ACCOUNT_HANDLER_HPP__
#define __DELETEROLE_ACCOUNT_HANDLER_HPP__

#include "Handler.hpp"
#include "WorldGlobalDefine.hpp"


class CDeleteRoleAccountHandler : public IHandler
{
protected:
    static GameProtocolMsg m_stWorldMsg;
public:
    virtual ~CDeleteRoleAccountHandler();

public:
    virtual int OnClientMsg(const void* pMsg);

private:
    int OnRequestDeleteRoleAccount();
    int OnResponseDeleteRoleAccount();

    int DeleteDataFromCache(const GameProtocolMsg& rstWorldMsg);

private:
    GameProtocolMsg* m_pMsg;



};

#endif


