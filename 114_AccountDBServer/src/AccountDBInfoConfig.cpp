#include "pugixml.hpp"

#include "StringUtility.hpp"
#include "LogAdapter.hpp"
#include "AccountDBErrorNumDef.hpp"

#include "AccountDBInfoConfig.hpp"

using namespace pugi;
using namespace ServerLib;

CAccountDBInfoConfig::CAccountDBInfoConfig()
{
    m_iAccountDBNumber = 0;
    memset(m_astAccountDBInfo, 0, sizeof(m_astAccountDBInfo));

    return;
}

CAccountDBInfoConfig::~CAccountDBInfoConfig()
{
    return;
}

int CAccountDBInfoConfig::LoadAccountDBInfoConfig(const char* pszFilePath)
{
    if(!pszFilePath)
    {
        TRACESVR("Failed to load role db info config, invalid parameter!\n");
        return -1;
    }

    LOGDEBUG("Load ACCOUNTDB info config!\n");

    xml_document oXmlDoc;

    xml_parse_result oResult = oXmlDoc.load_file(pszFilePath);
    if(!oResult)
    {
        TRACESVR("Failed to open xml config file : %s\n", pszFilePath);
        return -2;
    }
    
    xml_node oDBInfoNodes = oXmlDoc.child("ALLDBMSINFOS");

    for(xml_node oOneInfoNode=oDBInfoNodes.child("DBMSInfo"); oOneInfoNode; oOneInfoNode=oOneInfoNode.next_sibling())
    {
        if(m_iAccountDBNumber >= TOTAL_ACCOUNTDB_NUMBER)
        {
            TRACESVR("Failed to load role db info, number reach max %d\n", TOTAL_ACCOUNTDB_NUMBER);
            return -3;
        }

        ONEACCOUNTDBINFO& stOneAccountDB = m_astAccountDBInfo[m_iAccountDBNumber];

        stOneAccountDB.iDBMajorVersion = oOneInfoNode.child("m_iMajorVersion").text().as_uint();
        stOneAccountDB.iDBMinVersion = oOneInfoNode.child("m_iMinVersion").text().as_uint();
        SAFE_STRCPY(stOneAccountDB.szDBEngine, oOneInfoNode.child("m_szDBMSName").text().as_string(), sizeof(stOneAccountDB.szDBEngine)-1);
        SAFE_STRCPY(stOneAccountDB.szDBHost, oOneInfoNode.child("m_szDBMSConnectionInfo").text().as_string(), sizeof(stOneAccountDB.szDBHost)-1);
        SAFE_STRCPY(stOneAccountDB.szUserName, oOneInfoNode.child("m_szDBMSUser").text().as_string(), sizeof(stOneAccountDB.szUserName)-1);
        SAFE_STRCPY(stOneAccountDB.szUserPasswd, oOneInfoNode.child("m_szDBMSPassword").text().as_string(), sizeof(stOneAccountDB.szUserPasswd)-1);
        SAFE_STRCPY(stOneAccountDB.szDBName, oOneInfoNode.child("m_szDBMSCurDatabaseName").text().as_string(), sizeof(stOneAccountDB.szDBName)-1);

        ++m_iAccountDBNumber;
    }

    return T_SERVER_SUCESS;
}

const ONEACCOUNTDBINFO* CAccountDBInfoConfig::GetOneAccountDBInfoByIndex(int iDBIndex)
{
    if(iDBIndex>=m_iAccountDBNumber || iDBIndex<0)
    {
        TRACESVR("Failed to get one role db info, invalid index %d\n", iDBIndex);
        return NULL;
    }

    return &m_astAccountDBInfo[iDBIndex];
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
