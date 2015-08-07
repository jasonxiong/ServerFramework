#include "GameProtocol.hpp"

#include "AccountHandlerSet.hpp"

void CAccountHandlerSet::InitAllHandlers()
{
    m_pCreateRoleRequestHandler = NULL;
    m_pCreateRoleResponseHandler = NULL;
    m_pDeleteRoleRequestHandler = NULL;
    m_pDeleteRoleResponseHandler = NULL;
    m_pListRoleRequestHandler = NULL;
    m_pListRoleResponseHandler = NULL;
    m_pClientClosedHandler = NULL;
    m_pAddNewNameHandler = NULL;
    m_pListZoneHandler = NULL;
}

void CAccountHandlerSet::FreeAllHandlers()
{
    if (m_pCreateRoleRequestHandler)
    {
        delete m_pCreateRoleRequestHandler;
        m_pCreateRoleRequestHandler = NULL;
    }

    if (m_pCreateRoleResponseHandler)
    {
        delete m_pCreateRoleResponseHandler;
        m_pCreateRoleResponseHandler = NULL;
    }

    if (m_pDeleteRoleRequestHandler)
    {
        delete m_pDeleteRoleRequestHandler;
        m_pDeleteRoleRequestHandler = NULL;
    }

    if (m_pDeleteRoleResponseHandler)
    {
        delete m_pDeleteRoleResponseHandler;
        m_pDeleteRoleResponseHandler = NULL;
    }

    if (m_pListRoleRequestHandler)
    {
        delete m_pListRoleRequestHandler;
        m_pListRoleRequestHandler = NULL;
    }

    if (m_pListRoleResponseHandler)
    {
        delete m_pListRoleResponseHandler;
        m_pListRoleResponseHandler = NULL;
    }

    if (m_pClientClosedHandler)
    {
        delete m_pClientClosedHandler;
        m_pClientClosedHandler = NULL;
    }

    if(m_pAddNewNameHandler)
    {
        delete m_pAddNewNameHandler;
        m_pAddNewNameHandler = NULL;
    }

    if(m_pListZoneHandler)
    {
        delete m_pListZoneHandler;
        m_pListZoneHandler = NULL;
    }

    return;
}

int CAccountHandlerSet::AllocAllHandlers()
{
    m_pCreateRoleRequestHandler = new CCreateRoleRequestHandler();
    if (!m_pCreateRoleRequestHandler)
    {
        return -1;
    }

    m_pCreateRoleResponseHandler = new CCreateRoleResponseHandler();
    if (!m_pCreateRoleResponseHandler)
    {
        return -1;
    }

    m_pDeleteRoleRequestHandler = new CDeleteRoleRequestHandler();
    if (!m_pDeleteRoleRequestHandler)
    {
        return -1;
    }

    m_pDeleteRoleResponseHandler = new CDeleteRoleResponseHandler();
    if (!m_pDeleteRoleResponseHandler)
    {
        return -1;
    }

    m_pListRoleRequestHandler = new CListRoleRequestHandler();
    if (!m_pListRoleRequestHandler)
    {
        return -1;
    }

    m_pListRoleResponseHandler = new CListRoleResponseHandler();
    if (!m_pListRoleResponseHandler)
    {
        return -1;
    }

    m_pClientClosedHandler = new CClientClosedHandler();
    if (!m_pClientClosedHandler)
    {
        return -1;
    }

    m_pAddNewNameHandler = new CAccountAddNewNameHandler();
    if(!m_pAddNewNameHandler)
    {
        return -5;
    }

    m_pListZoneHandler = new CListZoneHandler();
    if(!m_pListZoneHandler)
    {
        return -6;
    }

    return 0;
}

int CAccountHandlerSet::RegisterAllHandlers()
{
    int iRes = RegisterHandler(MSGID_ACCOUNT_CREATEROLE_REQUEST, m_pCreateRoleRequestHandler, EKMT_CLIENT);
    if (iRes != 0)
    {
        return -1;
    }

    iRes = RegisterHandler(MSGID_ACCOUNT_CREATEROLE_RESPONSE, m_pCreateRoleResponseHandler);
    if (iRes != 0)
    {
        return -2;
    }

    iRes = RegisterHandler(MSGID_ACCOUNT_DELETEROLE_REQUEST, m_pDeleteRoleRequestHandler, EKMT_CLIENT);
    if (iRes != 0)
    {
        return -3;
    }

    iRes = RegisterHandler(MSGID_ACCOUNT_DELETEROLE_RESPONSE, m_pDeleteRoleResponseHandler);
    if (iRes != 0)
    {
        return -4;
    }

    iRes = RegisterHandler(MSGID_ACCOUNT_LISTROLE_REQUEST, m_pListRoleRequestHandler, EKMT_CLIENT);
    if (iRes != 0)
    {
        return -5;
    }

    iRes = RegisterHandler(MSGID_ACCOUNT_LISTROLE_RESPONSE, m_pListRoleResponseHandler);
    if (iRes != 0)
    {
        return -6;
    }

    iRes = RegisterHandler(MSGID_LOGOUTSERVER_REQUEST, m_pClientClosedHandler, EKMT_CLIENT);
    if (iRes != 0)
    {
        return -7;
    }

    iRes = RegisterHandler(MSGID_LOGOUTSERVER_RESPONSE, m_pClientClosedHandler);
    if (iRes != 0)
    {
        return -8;
    }

    iRes = RegisterHandler(MSGID_ADDNEWNAME_RESPONSE, m_pAddNewNameHandler);
    if(iRes != 0)
    {
        return -9;
    }
    
    iRes = RegisterHandler(MSGID_ACCOUNT_LISTZONE_REQUEST, m_pListZoneHandler, EKMT_CLIENT);
    if(iRes != 0)
    {
        return -10;
    }

    iRes = RegisterHandler(MSGID_ACCOUNT_LISTZONE_RESPONSE, m_pListZoneHandler);
    if(iRes != 0)
    {
        return -11;
    }

    return 0;
}

CAccountHandlerSet::CAccountHandlerSet()
{
    InitAllHandlers();
}

CAccountHandlerSet::~CAccountHandlerSet()
{
    FreeAllHandlers();
}

int CAccountHandlerSet::Initialize()
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

