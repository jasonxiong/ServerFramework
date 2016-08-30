#ifndef __ACCOUNT_HANDLER_SET_HPP__
#define __ACCOUNT_HANDLER_SET_HPP__

#include "HandlerSet.hpp"
#include "SingletonTemplate.hpp"
#include "CreateRoleRequestHandler.hpp"
#include "CreateRoleResponseHandler.hpp"
#include "DeleteRoleRequestHandler.hpp"
#include "DeleteRoleResponseHandler.hpp"
#include "ListRoleRequestHandler.hpp"
#include "ListRoleResponseHandler.hpp"
#include "ClientClosedHandler.hpp"
#include "AccountAddNewNameHandler.hpp"
#include "ListZoneHandler.hpp"

using namespace ServerLib;

// Account应用中的消息处理者管理器
class CAccountHandlerSet : public CHandlerSet
{
private:
    // 该集合管理的所有消息处理者
    CCreateRoleRequestHandler*  m_pCreateRoleRequestHandler;
    CCreateRoleResponseHandler* m_pCreateRoleResponseHandler;
    CDeleteRoleRequestHandler* m_pDeleteRoleRequestHandler;
    CDeleteRoleResponseHandler* m_pDeleteRoleResponseHandler;
    CListRoleRequestHandler* m_pListRoleRequestHandler;
    CListRoleResponseHandler* m_pListRoleResponseHandler;
    CClientClosedHandler* m_pClientClosedHandler;
    CAccountAddNewNameHandler* m_pAddNewNameHandler;
    CListZoneHandler* m_pListZoneHandler;

private:
    // 本类的对象只能在CSingleton<CRoleDBHandlerSet>类中创建
    friend class CSingleton<CAccountHandlerSet>;
    CAccountHandlerSet();

private:
    // 对于新增的handler，注意要在以下四个函数中做相应修改
    void InitAllHandlers();
    void FreeAllHandlers();
    int AllocAllHandlers();
    int RegisterAllHandlers();

public:
    virtual ~CAccountHandlerSet();

    // 初始化该集合中的消息处理者
    virtual int Initialize();
};

#endif // __ACCOUNT_HANDLER_SET_HPP__


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
