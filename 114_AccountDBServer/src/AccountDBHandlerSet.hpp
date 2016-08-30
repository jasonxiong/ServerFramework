#ifndef __ROLE_DB_HANDLER_SET_HPP__
#define __ROLE_DB_HANDLER_SET_HPP__

#include "HandlerSet.hpp"
#include "AddAccountHandler.hpp"
#include "DeleteAccountHandler.hpp"
#include "FetchAccountHandler.hpp"
#include "UpdateAccountHandler.hpp"

// AccountDB应用中的消息处理者管理器
class CAccountDBHandlerSet : public CHandlerSet
{
private:
    // 该集合管理的所有消息处理者
    CAddAccountHandler m_stAddAccountHandler;
    CDeleteAccountHandler m_stDeleteAccountHandler;
    CFetchAccountHandler m_stFetchAccountHandler;
    CUpdateAccountHandler m_stUpdateAccountHandler;

private:
    int RegisterAllHandlers();
    int OpenAccountDB();

private:
    // 消息处理者处理消息时需要访问数据库
    DBClientWrapper m_oDBClient;

	int m_iThreadIdx;

public:
    CAccountDBHandlerSet();
    ~CAccountDBHandlerSet();

    // 初始化该集合中的消息处理者和数据库配置并连接数据库
    int Initialize(const int iThreadIdx);
};

#endif // __ROLE_DB_HANDLER_SET_HPP__

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
