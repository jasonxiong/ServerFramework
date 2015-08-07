#include "pugixml.hpp"

#include "StringUtility.hpp"
#include "LogAdapter.hpp"
#include "NameDBErrorNumDef.hpp"

#include "NameDBInfoConfig.hpp"

using namespace pugi;
using namespace ServerLib;

CNameDBInfoConfig::CNameDBInfoConfig()
{
    m_iNameDBNumber = 0;
    memset(m_astNameDBInfo, 0, sizeof(m_astNameDBInfo));

    return;
}

CNameDBInfoConfig::~CNameDBInfoConfig()
{
    return;
}

int CNameDBInfoConfig::LoadNameDBInfoConfig(const char* pszFilePath)
{
    if(!pszFilePath)
    {
        TRACESVR("Failed to load role db info config, invalid parameter!\n");
        return -1;
    }

    LOGDEBUG("Load NAMEDB info config!\n");

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
        if(m_iNameDBNumber >= TOTAL_NAMEDB_NUMBER)
        {
            TRACESVR("Failed to load role db info, number reach max %d\n", TOTAL_NAMEDB_NUMBER);
            return -3;
        }

        ONENAMEDBINFO& stOneNameDB = m_astNameDBInfo[m_iNameDBNumber];

        stOneNameDB.iDBMajorVersion = oOneInfoNode.child("m_iMajorVersion").text().as_uint();
        stOneNameDB.iDBMinVersion = oOneInfoNode.child("m_iMinVersion").text().as_uint();
        SAFE_STRCPY(stOneNameDB.szDBEngine, oOneInfoNode.child("m_szDBMSName").text().as_string(), sizeof(stOneNameDB.szDBEngine)-1);
        SAFE_STRCPY(stOneNameDB.szDBHost, oOneInfoNode.child("m_szDBMSConnectionInfo").text().as_string(), sizeof(stOneNameDB.szDBHost)-1);
        SAFE_STRCPY(stOneNameDB.szUserName, oOneInfoNode.child("m_szDBMSUser").text().as_string(), sizeof(stOneNameDB.szUserName)-1);
        SAFE_STRCPY(stOneNameDB.szUserPasswd, oOneInfoNode.child("m_szDBMSPassword").text().as_string(), sizeof(stOneNameDB.szUserPasswd)-1);
        SAFE_STRCPY(stOneNameDB.szDBName, oOneInfoNode.child("m_szDBMSCurDatabaseName").text().as_string(), sizeof(stOneNameDB.szDBName)-1);

        ++m_iNameDBNumber;
    }

    return T_SERVER_SUCESS;
}

const ONENAMEDBINFO* CNameDBInfoConfig::GetOneNameDBInfoByIndex(int iDBIndex)
{
    if(iDBIndex>=m_iNameDBNumber || iDBIndex<0)
    {
        TRACESVR("Failed to get one role db info, invalid index %d\n", iDBIndex);
        return NULL;
    }

    return &m_astNameDBInfo[iDBIndex];
}
