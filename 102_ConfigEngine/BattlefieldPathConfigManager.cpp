

#include <string>
#include <dirent.h>
#include <fstream>
#include <iostream>

#include "pugixml.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "ConfigHelper.hpp"

#include "BattlefieldPathConfigManager.hpp"

using namespace pugi;

CScenePathManager CBattlefieldPathConfigManager::m_astBattlefiledPath[MAX_BATTLEFIELD_PATH_CONFIG_NUMBER];     //阻挡配置信息

int CBattlefieldPathConfigManager::m_iBattlefieldPathNum = 0;

CBattlefieldPathConfigManager::CBattlefieldPathConfigManager()
{
    m_iBattlefieldPathNum = 0;

    memset(m_astBattlefiledPath, 0, sizeof(m_astBattlefiledPath));
}

CBattlefieldPathConfigManager::~CBattlefieldPathConfigManager()
{

}

//加载战场地编信息配置
int CBattlefieldPathConfigManager::LoadAllBattlefield()
{
    m_iBattlefieldPathNum = 0;

    char szResPath[256] = {0};

    //获取地编文件的路径
    GetCommonResourcePath(szResPath, sizeof(szResPath), "map/");

    //遍历该目录下的所有配置文件
    DIR* pstDir = opendir(szResPath);

    int iRet = T_SERVER_SUCESS;
    struct dirent* pstEntry = NULL;
    while((pstEntry=readdir(pstDir)) != NULL)
    {
        if(!(pstEntry->d_type&DT_REG))
        {
            continue;
        }
        //只处理普通文件
        std::string strConfigFile(std::string(szResPath)+std::string(pstEntry->d_name));

        if(strConfigFile.find(std::string("tmx"),0) != std::string::npos)
        {
            //TMX格式为xml的阻挡信息
            iRet = LoadOneBattlefieldPath(strConfigFile);
            if(iRet)
            {
                LOGERROR("Failed to load battlefield path file %s, ret %d\n", strConfigFile.c_str(), iRet);

                return iRet;
            }
        }
    }

    closedir(pstDir);

    LOGDEBUG("Success to load all battlefield  path files!\n");

    return T_SERVER_SUCESS;
}

//获取地图的阻挡信息
const CScenePathManager* CBattlefieldPathConfigManager::GetBattlefieldPathConfigManager(int iMapID)
{
    for(int i=0; i<m_iBattlefieldPathNum; ++i)
    {
        if(m_astBattlefiledPath[i].GetMapID() == iMapID)
        {
            return &(m_astBattlefiledPath[i]);
        }
    }

    return NULL;
}

//加载单个地编信息配置
int CBattlefieldPathConfigManager::LoadOneBattlefieldPath(const std::string& strConfigFile)
{
    if(m_iBattlefieldPathNum >= MAX_BATTLEFIELD_PATH_CONFIG_NUMBER)
    {
        LOGERROR("Failed to load battlefield config, num reach max %d\n", MAX_BATTLEFIELD_PATH_CONFIG_NUMBER);
        return -1;
    }

    static xml_document stXmlDoc;
    xml_parse_result pstResult = stXmlDoc.load_file(strConfigFile.c_str());
    if(!pstResult)
    {
        LOGERROR("Failed to open map file : %s\n", strConfigFile.c_str());
        stXmlDoc.reset();
        return -2;
    }

    //读取地图文件配置的属性
    int iMapID = 0;
    xml_node stProperties = stXmlDoc.child("map").child("properties");
    for(xml_node stOneProperty=stProperties.child("property"); stOneProperty; stOneProperty=stOneProperty.next_sibling())
    {
        const char* pstrName = stOneProperty.attribute("name").as_string();
        if(!SAFE_STRCMP(pstrName,"id", 2))
        {
            //是地图ID
            iMapID = stOneProperty.attribute("value").as_int();
        }
    }

    //读取阻挡层的信息: map->layer->name "map_block"
    xml_node stMap = stXmlDoc.child("map");
    for(xml_node stOneLayer = stMap.child("layer"); stOneLayer; stOneLayer=stOneLayer.next_sibling())
    {
        if(SAFE_STRCMP(stOneLayer.attribute("name").as_string(),"mapblock",sizeof("mapblock")-1))
        {
            //不是mapblock层
            continue;
        }

        //找到阻挡层，读取阻挡信息
        int iMapWidth = stOneLayer.attribute("width").as_int();
        int iMapHeight = stOneLayer.attribute("height").as_int();
        CScenePathManager& stPathManager = m_astBattlefiledPath[m_iBattlefieldPathNum];
        int iRet = stPathManager.Initialize(iMapID, iMapWidth, iMapHeight, stOneLayer);
        if(iRet)
        {
            LOGERROR("Failed to load battlefield path config, file name %s\n", strConfigFile.c_str());
            stXmlDoc.reset();
            return iRet;
        }
    
        ++m_iBattlefieldPathNum;

        break;
    }

    stXmlDoc.reset();

    LOGDEBUG("Success to load one battlefield config %s\n", strConfigFile.c_str());

    return T_SERVER_SUCESS;
}

