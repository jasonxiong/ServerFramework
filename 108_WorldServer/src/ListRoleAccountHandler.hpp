#ifndef __LIST_ROLE_ACCOUNT_HANDLER_HPP__
#define __LIST_ROLE_ACCOUNT_HANDLER_HPP__

#include "Handler.hpp"
#include "WorldGlobalDefine.hpp"

//处理Account Server过来的拉取玩家帐号基础信息的协议
class CListRoleAccountHandler : public IHandler
{
protected:
	static GameProtocolMsg m_stWorldMsg;
private:
    GameProtocolMsg* m_pRequestMsg;

public:
    virtual ~CListRoleAccountHandler();

public:
    virtual int OnClientMsg(const void* pMsg);

private:
    int OnAccountListRoleRequest();
    int OnAccountListRoleResponse();
};

#endif // __LIST_ROLE_ACCOUNT_HANDLER_HPP__
