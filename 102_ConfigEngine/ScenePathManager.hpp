#ifndef __SCENE_PATH_MANAGER_HPP__
#define __SCENE_PATH_MANAGER_HPP__

//战场路径管理器，主要包含地图的大小、网格和阻挡等信息

#include <bitset>

#include "pugixml.hpp"

#include "GameConfigDefine.hpp"

//服务器支持的最大格子宽度数目
const unsigned int MAX_MAP_BLOCK_WIDTH_NUM = 64;

//服务器支持的最大格子高度数目
const unsigned int MAX_MAP_BLOCK_HEIGHT_NUM = 18;

class CScenePathManager
{
public:
    CScenePathManager();

    ~CScenePathManager();

public:

    //初始化战斗地图阻挡信息,是XML格式的文件
    int Initialize(int iMapID, int iBlockWidthNum, int iBlockHeightNum, pugi::xml_node& stBlockLayer);

    //获取战场的宽度
    int GetBattlefieldWidth() const;

    //获取战场的高度
    int GetBattlefieldHeight() const;

    //获取地图编号的ID
    int GetMapID();

public:

    //是否路径可以连续行走
    bool CanContinueWalk(const TUNITPOSITION& stStartPos, const TUNITPATH& stPath) const;

    //战场上两点之间是否可以连续行走
    bool CanContinueWalk(const TUNITPOSITION& stStartPos, const TUNITPOSITION& stEndPos) const;

    //战场上的某点是否可以行走
    bool BattlefieldPosCanWalk(const TUNITPOSITION& stPos) const;

private:

    //地图阻挡信息的编号ID
    int m_iMapID;

    //地图的格子宽度数目
    int m_iMapBlockWidthNum;

    //地图的格子高度数目
    int m_iMapBlockHeigthNum;

    //正常的阻挡信息
    std::bitset<MAX_MAP_BLOCK_WIDTH_NUM * MAX_MAP_BLOCK_HEIGHT_NUM> m_aBlockInfo;
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
