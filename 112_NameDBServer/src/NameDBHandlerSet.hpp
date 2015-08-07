#ifndef __NAME_DB_HANDLER_SET_HPP__
#define __NAME_DB_HANDLER_SET_HPP__

#include "HandlerSet.hpp"
#include "AddNameHandler.hpp"
#include "DeleteNameHandler.hpp"

// NameDB应用中的消息处理者管理器
class CNameDBHandlerSet : public CHandlerSet
{
private:
    // 该集合管理的所有消息处理者
    CAddNameHandler m_stAddNameHandler;
    CDeleteNameHandler m_stDeleteNameHandler;

private:
    int RegisterAllHandlers();
    int OpenNameDB();

private:
    // 消息处理者处理消息时需要访问数据库
    DBClientWrapper m_oDBClient;

	int m_iThreadIdx;

public:
    CNameDBHandlerSet();
    ~CNameDBHandlerSet();

    // 初始化该集合中的消息处理者和数据库配置并连接数据库
    int Initialize(const int iThreadIdx);
};

#endif // __NAME_DB_HANDLER_SET_HPP__
