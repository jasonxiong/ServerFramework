#ifndef __WORLD_NAME_HANDLER_HPP__
#define __WORLD_NAME_HANDLER_HPP__

#include "Handler.hpp"
#include "WorldGlobalDefine.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"

class CWorldNameHandler : public IHandler
{
public:
	virtual ~CWorldNameHandler();

public:
	virtual int OnClientMsg(const void* pMsg);

private:

    //插入新名字的请求
	int OnRequestAddNewName();

    //插入新名字的返回
    int OnResponseAddNewName();

    int SendAddNewNameResponseToAccount(const std::string& strName, unsigned int uiUin, int iResult, int iType);

    //删除名字的请求处理
    int OnRequestDeleteName();

private:
	GameProtocolMsg* m_pMsg;

	static GameProtocolMsg m_stWorldMsg;
};

#endif


