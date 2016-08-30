
#include "CommonDef.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"

#include "ScenePathManager.hpp"

using namespace ServerLib;
using namespace pugi;

CScenePathManager::CScenePathManager()
{
    m_iMapID = 0;

    m_iMapBlockWidthNum = 0;
    m_iMapBlockHeigthNum = 0;
}

CScenePathManager::~CScenePathManager()
{

}

//初始化战斗地图阻挡信息,是XML格式的文件
int CScenePathManager::Initialize(int iMapID, int iBlockWidthNum, int iBlockHeightNum, pugi::xml_node& stBlockLayer)
{
    m_iMapID = iMapID;

    //设置地图的Block数量大小
    m_iMapBlockWidthNum = iBlockWidthNum;
    m_iMapBlockHeigthNum = iBlockHeightNum;

    //读取地图的阻挡信息
    //1表示有阻挡，不可行走
    m_aBlockInfo.reset();
    xml_node stOneBlock = stBlockLayer.child("data").child("tile");
    int iIndex = 0;
    for(; stOneBlock; stOneBlock=stOneBlock.next_sibling())
    {
        if(stOneBlock.attribute("gid").as_int())
        {
            //是阻挡
            m_aBlockInfo.set(iIndex);
        }

        ++iIndex;
    }

    return T_SERVER_SUCESS;
}

//获取地图编号的ID
int CScenePathManager::GetMapID()
{
    return m_iMapID;
}

//获取战场的宽度
int CScenePathManager::GetBattlefieldWidth() const
{
    return m_iMapBlockWidthNum;
}

//获取战场的高度
int CScenePathManager::GetBattlefieldHeight() const
{
    return m_iMapBlockHeigthNum;
}

//是否路径可以连续行走
bool CScenePathManager::CanContinueWalk(const TUNITPOSITION& stStartPos, const TUNITPATH& stPath) const
{
    if(stPath.iPosNum == 0)
    {
        return true;
    }

    //先检查起点到第一个点
    if(!CanContinueWalk(stStartPos,stPath.astPos[0]))
    {
        return false;
    }

    for(int i=0; i<stPath.iPosNum-1; ++i)
    {
        if(!CanContinueWalk(stPath.astPos[i], stPath.astPos[i+1]))
        {
            return false;
        }
    }

    return true;
}

//两点之间是否可以连续行走
bool CScenePathManager::CanContinueWalk(const TUNITPOSITION& stStartPos, const TUNITPOSITION& stEndPos) const
{
    //先检查两点是否连续
    if(ABS(stStartPos.iPosX,stEndPos.iPosX) == 0)
    {
        if(ABS(stStartPos.iPosY,stEndPos.iPosY) > 1)
        {
            return false;
        }
    }
    else if(ABS(stStartPos.iPosX,stEndPos.iPosX) == 1)
    {
        if(ABS(stStartPos.iPosY,stEndPos.iPosY) != 0)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    if(!BattlefieldPosCanWalk(stStartPos) || !BattlefieldPosCanWalk(stEndPos))
    {
        return false;
    }
        
    return true;
}

//战场上的某点是否可以行走
bool CScenePathManager::BattlefieldPosCanWalk(const TUNITPOSITION& stPos) const
{
    //检查位置是否合法
    if(stPos.iPosX < 0 || stPos.iPosX>=m_iMapBlockWidthNum
       || stPos.iPosY < 0 || stPos.iPosY>=m_iMapBlockHeigthNum)
    {
        return false;
    }

    if(m_aBlockInfo.test(stPos.iPosY*m_iMapBlockWidthNum+stPos.iPosX))
    {
        //是阻挡
        return false;
    }

    return true;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
