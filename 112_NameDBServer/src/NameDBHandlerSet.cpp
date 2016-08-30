#include "NameDBLogManager.hpp"
#include "StringUtility.hpp"
#include "ConfigHelper.hpp"
#include "NameDBApp.hpp"
#include "DBClientWrapper.hpp"

#include "NameDBHandlerSet.hpp"

int CNameDBHandlerSet::RegisterAllHandlers()
{
    int iRes = 0;

    iRes = RegisterHandler(MSGID_ADDNEWNAME_REQUEST, &m_stAddNameHandler);
    if (iRes != 0)
    {
        return -1;
    }
    m_stAddNameHandler.SetThreadIdx(m_iThreadIdx);

    iRes = RegisterHandler(MSGID_DELETENAME_REQUEST, &m_stDeleteNameHandler);
    if (iRes != 0)
    {
        return -2;
    }
    m_stDeleteNameHandler.SetThreadIdx(m_iThreadIdx);

    return 0;
}

CNameDBHandlerSet::CNameDBHandlerSet() :
    m_stAddNameHandler(&m_oDBClient),
    m_stDeleteNameHandler(&m_oDBClient)
{
    
}

CNameDBHandlerSet::~CNameDBHandlerSet()
{

}

int CNameDBHandlerSet::Initialize(const int iThreadIdx)
{
    int iRet = 0;

    m_iThreadIdx = iThreadIdx;
	
    iRet = OpenNameDB();
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

int CNameDBHandlerSet::OpenNameDB()
{
    m_oDBClient.Init(false);

    return 0;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
