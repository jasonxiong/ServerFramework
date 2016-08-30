#include "RoleDBLogManager.hpp"
#include "StringUtility.hpp"
#include "ConfigHelper.hpp"
#include "RoleDBApp.hpp"
#include "DBClientWrapper.hpp"

#include "RoleDBHandlerSet.hpp"

int CRoleDBHandlerSet::RegisterAllHandlers()
{
    int iRes = 0;

    iRes = RegisterHandler(MSGID_WORLD_FETCHROLE_REQUEST, &m_FetchRoleHandler);
    if (iRes != 0)
    {
        return -1;
    }
    m_FetchRoleHandler.SetThreadIdx(m_iThreadIdx);

    iRes = RegisterHandler(MSGID_WORLD_UPDATEROLE_REQUEST, &m_UpdateRoleHandler);
    if (iRes != 0)
    {
        return -2;
    }
    m_UpdateRoleHandler.SetThreadIdx(m_iThreadIdx);

    iRes = RegisterHandler(MSGID_WORLD_CREATEROLE_REQUEST, &m_CreateRoleHandler);
    if (iRes != 0)
    {
        return -3;
    }
    m_CreateRoleHandler.SetThreadIdx(m_iThreadIdx);

    iRes = RegisterHandler(MSGID_ACCOUNT_LISTROLE_REQUEST, &m_ListRoleHandler);
    if (iRes != 0)
    {
        return -4;
    }
    m_ListRoleHandler.SetThreadIdx(m_iThreadIdx);

    return 0;
}

CRoleDBHandlerSet::CRoleDBHandlerSet() :
    m_FetchRoleHandler(&m_oDBClient),
    m_UpdateRoleHandler(&m_oDBClient),
    m_CreateRoleHandler(&m_oDBClient),
    m_ListRoleHandler(&m_oDBClient)
{
    
}

CRoleDBHandlerSet::~CRoleDBHandlerSet()
{

}

int CRoleDBHandlerSet::Initialize(const int iThreadIdx)
{
    int iRet = 0;

    m_iThreadIdx = iThreadIdx;
	
    iRet = OpenRoleDB();
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

int CRoleDBHandlerSet::OpenRoleDB()
{
    //todo jasonxiong 这个地方目前只对mysql进行初始化
    //目前只支持单个线程单个MYSQL连接
    m_oDBClient.Init(false);

    return 0;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
