#include "AccountDBLogManager.hpp"
#include "StringUtility.hpp"
#include "ConfigHelper.hpp"
#include "AccountDBApp.hpp"
#include "DBClientWrapper.hpp"

#include "AccountDBHandlerSet.hpp"

int CAccountDBHandlerSet::RegisterAllHandlers()
{
    int iRes = 0;

    iRes = RegisterHandler(MSGID_ACCOUNTDB_ADDACCOUNT_REQUEST, &m_stAddAccountHandler);
    if (iRes != 0)
    {
        return -1;
    }
    m_stAddAccountHandler.SetThreadIdx(m_iThreadIdx);

    iRes = RegisterHandler(MSGID_ACCOUNTDB_DELETE_REQUEST, &m_stDeleteAccountHandler);
    if (iRes != 0)
    {
        return -2;
    }
    m_stDeleteAccountHandler.SetThreadIdx(m_iThreadIdx);

    iRes = RegisterHandler(MSGID_ACCOUNTDB_FETCH_REQUEST, &m_stFetchAccountHandler);
    if(iRes != 0)
    {
        return -3;
    }
    m_stFetchAccountHandler.SetThreadIdx(m_iThreadIdx);

    iRes = RegisterHandler(MSGID_ACCOUNTDB_UPDATE_REQUEST, &m_stUpdateAccountHandler);
    if(iRes != 0)
    {
        return -4;
    }
    m_stUpdateAccountHandler.SetThreadIdx(m_iThreadIdx);

    return 0;
}

CAccountDBHandlerSet::CAccountDBHandlerSet() :
    m_stAddAccountHandler(&m_oDBClient),
    m_stDeleteAccountHandler(&m_oDBClient),
    m_stFetchAccountHandler(&m_oDBClient),
    m_stUpdateAccountHandler(&m_oDBClient)
{
    
}

CAccountDBHandlerSet::~CAccountDBHandlerSet()
{

}

int CAccountDBHandlerSet::Initialize(const int iThreadIdx)
{
    int iRet = 0;

    m_iThreadIdx = iThreadIdx;
	
    iRet = OpenAccountDB();
    if (iRet != 0)
    {
        return iRet;
    }

    iRet = RegisterAllHandlers();
    if (iRet != 0)
    {
        return -1;
    }

    return 0;
}

int CAccountDBHandlerSet::OpenAccountDB()
{
    m_oDBClient.Init(true);

    return 0;
}
