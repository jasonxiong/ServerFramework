#ifndef __REGAUTH_HANDLER_SET_HPP__
#define __REGAUTH_HANDLER_SET_HPP__

#include "HandlerSet.hpp"
#include "SingletonTemplate.hpp"
#include "RegisterAccountHandler.hpp"
#include "AuthAccountHandler.hpp"
#include "UpdateAccountHandler.hpp"
#include "AccountDBAddHandler.hpp"
#include "AccountDBFetchHandler.hpp"
#include "ClientClosedHandler.hpp"

using namespace ServerLib;

// RegAuth应用中的消息处理者管理器
class CRegAuthHandlerSet : public CHandlerSet
{
private:
    // 该集合管理的所有消息处理者
    CRegisterAccountHandler* m_pRegisterAccountHandler;
    CAccountDBAddHandler* m_pAccountDBAddHandler;
    CAccountDBFetchHandler* m_pAccountDBFetchHandler;

    CAuthAccountHandler* m_pAuthAccountHandler;
    CUpdateAccountHandler* m_pUpdateAccountHandler;
    CClientClosedHandler* m_pClientClosedHandler;

private:
    // 本类的对象只能在CSingleton<CRoleDBHandlerSet>类中创建
    friend class CSingleton<CRegAuthHandlerSet>;
    CRegAuthHandlerSet();

private:
    // 对于新增的handler，注意要在以下四个函数中做相应修改
    void InitAllHandlers();
    void FreeAllHandlers();
    int AllocAllHandlers();
    int RegisterAllHandlers();

public:
    virtual ~CRegAuthHandlerSet();

    // 初始化该集合中的消息处理者
    virtual int Initialize();
};

#endif // __REGAUTH_HANDLER_SET_HPP__

