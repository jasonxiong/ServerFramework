#ifndef __ASTAR_PATH_UTILITY_HPP__
#define __ASTAR_PATH_UTILITY_HPP__

#include <vector>

#include "GameConfigDefine.hpp"
#include "ScenePathManager.hpp"

// A* 算法参数
struct tagSceneBlock;
#pragma pack(1)
typedef struct tagAStarNode
{
    unsigned short iValueG;
    unsigned short iValueF;   // FGH值

	char bClosed:1;  // 是否在封闭链表
	char bOpened:1;  // 是否在开放链表

    tagSceneBlock *pstCenterNode; // 中心节点

}TAStarNode;

// 地图块
typedef struct tagSceneBlock
{
    unsigned short iSceneBlock;      // 地图块的编号

    TAStarNode stAStar;   
}TSceneBlock;

#pragma pack()

// 路径点最小堆，优化A*算法的开放列表

#define MAX_PATH_NODE ((int)(MAX_MAP_BLOCK_WIDTH_NUM * MAX_MAP_BLOCK_HEIGHT_NUM))

class CPathMinHeap
{
public:
    CPathMinHeap();

    void Initialize();

    // 弹出路径最小的点
    TSceneBlock *PopHeap();

    // 压入一个路径点
    bool PushHeap(TSceneBlock *pstSceneBlock);

	// 判断是否缓冲区满
	bool IsHeapFull();

private:
    bool InsertHeap(TSceneBlock *pstSceneBlock, int iPosition);

private:
    int m_iOpenNodes;
    int m_iCloseNodes;
    TSceneBlock *m_astOpenNode[MAX_PATH_NODE];
    TSceneBlock *m_astCloseNode[MAX_PATH_NODE];
};

//A*寻路工具类
class CBattlefieldObj;
class CAStarPathUtility
{
public:
    
    //A* 算法寻路
    static bool FindAStarPath(CBattlefieldObj& stBattlefieldObj, const CScenePathManager& stPathManager, const TUNITPOSITION& stStartPos, 
                              const TUNITPOSITION& stEndPos, int iSize, std::vector<TUNITPOSITION>& vPath);

private:

    // A* 计算节点权值
    static int AStarCountNode(CBattlefieldObj& stBattlefieldObj, const CScenePathManager& stPathManager, int iX, int iY, int iEndX, int iEndY, TSceneBlock *pstCenterBlock);

private:

    // 路径块
    static TSceneBlock m_astSceneBlock[MAX_PATH_NODE];

    // A* 最小堆
    static CPathMinHeap m_stPathMinHeap;

    //当前使用地图宽度
    static int m_iMapWidth;

    //当前使用地图高度
    static int m_iMapHeight;

    //寻路的战斗单位的大小
    static int m_iUnitSize;
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
