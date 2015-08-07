#ifndef __ROLE_DB_HANDLER_SET_HPP__
#define __ROLE_DB_HANDLER_SET_HPP__

#include "HandlerSet.hpp"
#include "RoleDBFetchRoleHandler.hpp"
#include "RoleDBUpdateRoleHandler.hpp"
#include "RoleDBCreateRoleHandler.hpp"
#include "RoleDBListRoleHandler.hpp"

// RoleDB应用中的消息处理者管理器
class CRoleDBHandlerSet : public CHandlerSet
{
private:
    // 该集合管理的所有消息处理者
    CRoleDBFetchRoleHandler m_FetchRoleHandler;
    CRoleDBUpdateRoleHandler m_UpdateRoleHandler;
    CRoleDBCreateRoleHandler m_CreateRoleHandler;
    CRoleDBListRoleHandler m_ListRoleHandler;

private:
    int RegisterAllHandlers();
    int OpenRoleDB();

private:
    // 消息处理者处理消息时需要访问数据库
    DBClientWrapper m_oDBClient;

	int m_iThreadIdx;

public:
    CRoleDBHandlerSet();
    ~CRoleDBHandlerSet();

    // 初始化该集合中的消息处理者和数据库配置并连接数据库
    int Initialize(const int iThreadIdx);
};

#endif // __ROLE_DB_HANDLER_SET_HPP__
