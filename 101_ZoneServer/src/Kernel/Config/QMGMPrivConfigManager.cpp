#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "pugixml.hpp"
#include "AppDef.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"

#include "QMGMPrivConfigManager.hpp"

using namespace ServerLib;
using namespace pugi;

QMGMUserPrivConfig CQMGMPrivConfigManager::m_stGMPrivConfig;

CQMGMPrivConfigManager::CQMGMPrivConfigManager()
{
    memset(&m_stGMPrivConfig, 0, sizeof(m_stGMPrivConfig));
}

CQMGMPrivConfigManager::~CQMGMPrivConfigManager()
{

}

int CQMGMPrivConfigManager::LoadGMPrivConfig()
{
    LOGDEBUG("Load GM Priv config!\n");

    xml_document oXmlDoc;
    xml_parse_result oResult = oXmlDoc.load_file(GM_PRIV_CONFIG_FILE);
    if(!oResult)
    {
        TRACESVR("Failed to open xml config file : %s\n", GM_PRIV_CONFIG_FILE);
        return -1;
    }
    
    //先读取有效的IP号段的配置
    xml_node oIpSectionNodes = oXmlDoc.child("GMPriv").child("IPList");
    GMValidIPList& stIPList = m_stGMPrivConfig.stValidIpSection;
    stIPList.iIPSectionNum = 0;
    for(xml_node oOneIpSection= oIpSectionNodes.child("ValidIP"); oOneIpSection; oOneIpSection = oOneIpSection.next_sibling())
    {
        if(stIPList.iIPSectionNum >= MAX_VALID_GM_IP_SECTION)
        {
            //超过允许的IP Section上限
            TRACESVR("Failed to load GM IP section, number reach max %d\n", MAX_VALID_GM_IP_SECTION);
            return -2;
        }

        GMIPSection& stOneSection = stIPList.stIPSections[stIPList.iIPSectionNum];

        stOneSection.uIPBegin = inet_network(oOneIpSection.attribute("begin").as_string());
        stOneSection.uIPEnd = inet_network(oOneIpSection.attribute("end").as_string());

        ++stIPList.iIPSectionNum;
    }

    //加载有效的GM用户uin的配置
    xml_node oValidUinNodes = oXmlDoc.child("GMPriv").child("UinLists");
    GMValidUserList& stUserList = m_stGMPrivConfig.stValidUsers;
    stUserList.iValidUserNum = 0;
    for(xml_node oOneUser=oValidUinNodes.child("UinList"); oOneUser; oOneUser = oOneUser.next_sibling())
    {
        if(stUserList.iValidUserNum >= MAX_VALID_GM_USER_NUM)
        {
            //超过允许的GM用户的上限
            TRACESVR("Failed to load GM Valid User List, number reach max %d\n", MAX_VALID_GM_USER_NUM);
            return -3;
        }

        stUserList.uValidUin[stUserList.iValidUserNum] = oOneUser.attribute("uin").as_int();

        ++stUserList.iValidUserNum;
    }

    return T_SERVER_SUCESS;
}

//检查玩家的IP是否在GM权限的IP号段内
bool CQMGMPrivConfigManager::CheckIsGMIP(unsigned int uClientIP)
{
    GMValidIPList& stValidIPList = m_stGMPrivConfig.stValidIpSection;
    for(int i=0; i<stValidIPList.iIPSectionNum; ++i)
    {
        if(stValidIPList.stIPSections[i].uIPBegin<=uClientIP && stValidIPList.stIPSections[i].uIPEnd>=uClientIP)
        {
            return true;
        }
    }

    return false;
}

bool CQMGMPrivConfigManager::CheckIsGMUin(unsigned int uin)
{
    GMValidUserList& stValidUserList = m_stGMPrivConfig.stValidUsers;
    for(int i=0; i<stValidUserList.iValidUserNum; ++i)
    {
        if(stValidUserList.uValidUin[i] == uin)
        {
            return true;
        }
    }

    return false;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
