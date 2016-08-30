#include "pugixml.hpp"

#include "StringUtility.hpp"
#include "LogAdapter.hpp"
#include "RoleDBErrorNumDef.hpp"

#include "RoleDBInfoConfig.hpp"

using namespace pugi;
using namespace ServerLib;

CRoleDBInfoConfig::CRoleDBInfoConfig()
{
    m_iRoleDBNumber = 0;
    memset(m_astRoleDBInfo, 0, sizeof(m_astRoleDBInfo));

    return;
}

CRoleDBInfoConfig::~CRoleDBInfoConfig()
{
    return;
}

int CRoleDBInfoConfig::LoadRoleDBInfoConfig(const char* pszFilePath)
{
    if(!pszFilePath)
    {
        TRACESVR("Failed to load role db info config, invalid parameter!\n");
        return -1;
    }

    LOGDEBUG("Load ROLEDB info config!\n");

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
        if(m_iRoleDBNumber >= TOTAL_ROLEDB_NUMBER)
        {
            TRACESVR("Failed to load role db info, number reach max %d\n", TOTAL_ROLEDB_NUMBER);
            return -3;
        }

        ONEROLEDBINFO& stOneRoleDB = m_astRoleDBInfo[m_iRoleDBNumber];

        stOneRoleDB.iDBMajorVersion = oOneInfoNode.child("m_iMajorVersion").text().as_uint();
        stOneRoleDB.iDBMinVersion = oOneInfoNode.child("m_iMinVersion").text().as_uint();
        SAFE_STRCPY(stOneRoleDB.szDBEngine, oOneInfoNode.child("m_szDBMSName").text().as_string(), sizeof(stOneRoleDB.szDBEngine)-1);
        SAFE_STRCPY(stOneRoleDB.szDBHost, oOneInfoNode.child("m_szDBMSConnectionInfo").text().as_string(), sizeof(stOneRoleDB.szDBHost)-1);
        SAFE_STRCPY(stOneRoleDB.szUserName, oOneInfoNode.child("m_szDBMSUser").text().as_string(), sizeof(stOneRoleDB.szUserName)-1);
        SAFE_STRCPY(stOneRoleDB.szUserPasswd, oOneInfoNode.child("m_szDBMSPassword").text().as_string(), sizeof(stOneRoleDB.szUserPasswd)-1);
        SAFE_STRCPY(stOneRoleDB.szDBName, oOneInfoNode.child("m_szDBMSCurDatabaseName").text().as_string(), sizeof(stOneRoleDB.szDBName)-1);

        ++m_iRoleDBNumber;
    }

    return T_SERVER_SUCESS;
}

const ONEROLEDBINFO* CRoleDBInfoConfig::GetOneRoleDBInfoByIndex(int iDBIndex)
{
    if(iDBIndex>=m_iRoleDBNumber || iDBIndex<0)
    {
        TRACESVR("Failed to get one role db info, invalid index %d\n", iDBIndex);
        return NULL;
    }

    return &m_astRoleDBInfo[iDBIndex];
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
