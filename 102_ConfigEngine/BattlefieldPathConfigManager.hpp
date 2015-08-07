
#ifndef __BATTLEFIELD_PATH_CONFIG_MANAGER_HPP__
#define __BATTLEFIELD_PATH_CONFIG_MANAGER_HPP__

#include "ConfigHelper.hpp"
#include "ScenePathManager.hpp"

//战场地图配置管理器,加载tmx格式的地图文件

const int MAX_BATTLEFIELD_PATH_CONFIG_NUMBER      = 100;  //最多支持100张地图阻挡信息

class CBattlefieldPathConfigManager
{
public:
    CBattlefieldPathConfigManager();
    ~CBattlefieldPathConfigManager();

public:

    //加载战场配置
    int LoadAllBattlefield(); 

    //获取战场的阻挡信息
    static const CScenePathManager* GetBattlefieldPathConfigManager(int iMapID);

private:

    //加载单个阻挡信息配置
    int LoadOneBattlefieldPath(const std::string& strConfigFile);

private:

    static int m_iBattlefieldPathNum;
    static CScenePathManager m_astBattlefiledPath[MAX_BATTLEFIELD_PATH_CONFIG_NUMBER];     //阻挡配置信息
};

#endif
