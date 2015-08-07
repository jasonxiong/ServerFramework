#include "GameProtocol.hpp"

#include "RegAuthHandlerSet.hpp"

using namespace ServerLib;

void CRegAuthHandlerSet::InitAllHandlers()
{
    m_pRegisterAccountHandler = NULL;
    m_pAccountDBAddHandler = NULL;
    m_pAccountDBFetchHandler = NULL;

    m_pAuthAccountHandler = NULL;
    m_pUpdateAccountHandler = NULL;
    m_pClientClosedHandler = NULL;
}

void CRegAuthHandlerSet::FreeAllHandlers()
{
    DELETEOBJ(m_pRegisterAccountHandler);

    DELETEOBJ(m_pAccountDBAddHandler);

    DELETEOBJ(m_pAccountDBFetchHandler);

    DELETEOBJ(m_pAuthAccountHandler);

    DELETEOBJ(m_pUpdateAccountHandler);

    DELETEOBJ(m_pClientClosedHandler);

    return;
}

int CRegAuthHandlerSet::AllocAllHandlers()
{
   m_pRegisterAccountHandler = new CRegisterAccountHandler();
   if(!m_pRegisterAccountHandler)
   {
       return -1;
   }

   m_pAccountDBAddHandler = new CAccountDBAddHandler();
   if(!m_pAccountDBAddHandler)
   {
       return -2;
   }

   m_pAccountDBFetchHandler = new CAccountDBFetchHandler();
   if(!m_pAccountDBFetchHandler)
   {
       return -3;
   }

   m_pAuthAccountHandler = new CAuthAccountHandler();
   if(!m_pAuthAccountHandler)
   {
       return -1;
   }

   m_pUpdateAccountHandler = new CUpdateAccountHandler();
   if(!m_pUpdateAccountHandler)
   {
       return -1;
   }

   m_pClientClosedHandler = new CClientClosedHandler();
   if (!m_pClientClosedHandler)
   {
       return -1;
   }

   return 0;
}

int CRegAuthHandlerSet::RegisterAllHandlers()
{
    //注册创建平台帐号的Handler
    int iRes = RegisterHandler(MSGID_REGAUTH_REGACCOUNT_REQUEST, m_pRegisterAccountHandler, EKMT_CLIENT);
    if (iRes != 0)
    {
        return -1;
    }

    iRes = RegisterHandler(MSGID_ACCOUNTDB_ADDACCOUNT_RESPONSE, m_pAccountDBAddHandler, EKMT_CLIENT);
    if(iRes != 0)
    {
        return -2;
    }

    iRes = RegisterHandler(MSGID_ACCOUNTDB_FETCH_RESPONSE, m_pAccountDBFetchHandler, EKMT_CLIENT);
    if(iRes != 0)
    {
        return -3;
    }

    //注册认证平台帐号的Handler
    iRes = RegisterHandler(MSGID_REGAUTH_AUTHACCOUNT_REQUEST, m_pAuthAccountHandler, EKMT_CLIENT);
    if(iRes != 0)
    {
        return -4;
    }

    //更新平台帐号的Handler
    iRes = RegisterHandler(MSGID_REGAUTH_UPDATE_REQUEST, m_pUpdateAccountHandler, EKMT_CLIENT);
    if(iRes != 0)
    {
        return -1;
    }

    iRes = RegisterHandler(MSGID_ACCOUNTDB_UPDATE_RESPONSE, m_pUpdateAccountHandler);
    if(iRes != 0)
    {
        return -1;
    }

    iRes = RegisterHandler(MSGID_LOGOUTSERVER_REQUEST, m_pClientClosedHandler, EKMT_CLIENT);
    if (iRes != 0)
    {
        return -1;
    }

    iRes = RegisterHandler(MSGID_LOGOUTSERVER_RESPONSE, m_pClientClosedHandler, EKMT_CLIENT);
    if (iRes != 0)
    {
        return -1;
    }

    return 0;
}

CRegAuthHandlerSet::CRegAuthHandlerSet()
{
    InitAllHandlers();
}

CRegAuthHandlerSet::~CRegAuthHandlerSet()
{
    FreeAllHandlers();
}

int CRegAuthHandlerSet::Initialize()
{
    int iRes = AllocAllHandlers();
    if (iRes != 0)
    {
        FreeAllHandlers();
        return -1;
    }

    iRes = RegisterAllHandlers();
    if (iRes != 0)
    {
        FreeAllHandlers();
        return -1;
    }

    return 0;
}

