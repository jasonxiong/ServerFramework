
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "BattlefieldObj.hpp"
#include "CombatUnitObj.hpp"
#include "CombatUtility.hpp"

#include "AStarPathUtility.hpp"

using namespace ServerLib;
using namespace GameConfig;

// 寻路临时路径点数组
static int g_iTempPathNumber;
static const int MAX_TEMP_PATH_NUMBER = 1024;

// 路径块
TSceneBlock CAStarPathUtility::m_astSceneBlock[MAX_PATH_NODE];

// A* 最小堆
CPathMinHeap CAStarPathUtility::m_stPathMinHeap;

//当前使用地图宽度
int CAStarPathUtility::m_iMapWidth = 0;

//当前使用地图高度
int CAStarPathUtility::m_iMapHeight = 0;

int CAStarPathUtility::m_iUnitSize = 0;

#define BLOCK_X(pBlock) ( ((pBlock) - &m_astSceneBlock[0]) % (m_iMapWidth) )
#define BLOCK_Y(pBlock) ( ((pBlock) - &m_astSceneBlock[0]) / (m_iMapWidth) )

//A* 算法寻路
bool CAStarPathUtility::FindAStarPath(CBattlefieldObj& stBattlefieldObj, const CScenePathManager& stPathManager, const TUNITPOSITION& stStartPos, 
                                      const TUNITPOSITION& stEndPos, int iSize, std::vector<TUNITPOSITION>& vPath)
{

    m_iMapWidth = stPathManager.GetBattlefieldWidth();
    m_iMapHeight = stPathManager.GetBattlefieldHeight();
    m_iUnitSize = iSize;

    if (stStartPos.iPosX < 0 || stStartPos.iPosX >= m_iMapWidth
        || stStartPos.iPosY < 0 || stStartPos.iPosY >= m_iMapHeight
        || stEndPos.iPosX < 0 || stEndPos.iPosX  >= m_iMapWidth
        || stEndPos.iPosY  < 0 || stEndPos.iPosY  >= m_iMapHeight)
    {
        return false;
    }

    //这边直接格子坐标就是最终点的坐标
    TSceneBlock *pstStartBlock = &m_astSceneBlock[stStartPos.iPosY*m_iMapWidth+stStartPos.iPosX];
    TSceneBlock *pstEndBlock = &m_astSceneBlock[stEndPos.iPosY*m_iMapWidth+stEndPos.iPosX];

    m_stPathMinHeap.Initialize();

    //初始化Bock数组中的块
    for(int y=0; y<m_iMapHeight; ++y)
    {
        for(int x=0; x<m_iMapWidth; ++x)
        {
            m_astSceneBlock[(y)*m_iMapWidth+x].iSceneBlock = (y)*m_iMapWidth+x;
        }
    }

    // 依次处理当前节点的周围4个节点, 直到到达最后节点
    TSceneBlock *pstCenterBlock = pstStartBlock;
    while (1)
    {
        if (m_stPathMinHeap.IsHeapFull())
        {
            break;
        }

        // 将当前节点加入封闭列表
        pstCenterBlock->stAStar.bClosed = true;

        // 到达终点所属的块
        if (pstCenterBlock->iSceneBlock == pstEndBlock->iSceneBlock)
        {
            break;
        }

        int iCenterX = BLOCK_X(pstCenterBlock);
        int iCenterY = BLOCK_Y(pstCenterBlock);

        if (iCenterX - 1 >= 0)
        {
            int iRet = AStarCountNode(stBattlefieldObj, stPathManager, iCenterX-1, iCenterY, stEndPos.iPosX, stEndPos.iPosY, pstCenterBlock);
            if (iRet < 0)
            {
                break;
            }
        }

        if (iCenterX + 1 < m_iMapWidth)
        {
            int iRet = AStarCountNode(stBattlefieldObj, stPathManager, iCenterX+1, iCenterY, stEndPos.iPosX, stEndPos.iPosY, pstCenterBlock);
            if (iRet < 0)
            {
                break;
            }
        }

        if (iCenterY - 1 >= 0)
        {
            int iRet = AStarCountNode(stBattlefieldObj, stPathManager, iCenterX, iCenterY-1, stEndPos.iPosX, stEndPos.iPosY, pstCenterBlock);
            if (iRet < 0)
            {
                break;
            }
        }

        if (iCenterY + 1 < m_iMapHeight)
        {
            int iRet = AStarCountNode(stBattlefieldObj, stPathManager, iCenterX, iCenterY+1, stEndPos.iPosX, stEndPos.iPosY, pstCenterBlock);
            if (iRet < 0)
            {
                break;
            }
        }

        // 取开放列表中路径最小的作为当前节点
        pstCenterBlock = m_stPathMinHeap.PopHeap();
        if (pstCenterBlock == NULL)
        {
            pstStartBlock->stAStar.bClosed = false;
            break;
        }
    }

    pstStartBlock->stAStar.bClosed = false;

    // A* 算法结束, 进行路径优化

    // 去掉终点块
    if (pstCenterBlock == pstEndBlock)
    {
        pstCenterBlock = pstCenterBlock->stAStar.pstCenterNode;
    }

    // 统计路径点个数
    g_iTempPathNumber = 0;
    TSceneBlock *pstPathBlock = pstCenterBlock;
    while (pstPathBlock && pstPathBlock != pstStartBlock)
    {
        g_iTempPathNumber++;
        pstPathBlock = pstPathBlock->stAStar.pstCenterNode;
    }

    if (g_iTempPathNumber >= MAX_TEMP_PATH_NUMBER - 1)
    {
        return false;
    }

    //封装最终的路径
    vPath.clear();

    // 将路径点反向连起来
    pstPathBlock = pstCenterBlock;
    for (int i = g_iTempPathNumber - 1; i >= 0; i--)
    {
        vPath.insert(vPath.begin(), TUNITPOSITION(BLOCK_X(pstPathBlock),BLOCK_Y(pstPathBlock)));

        pstPathBlock = pstPathBlock->stAStar.pstCenterNode;
    }

    // 添加终点
    vPath.push_back(stEndPos);

    return true;
}

// A* 计算节点权值
int CAStarPathUtility::AStarCountNode(CBattlefieldObj& stBattlefieldObj, const CScenePathManager& stPathManager, int iX, int iY, int iEndX, int iEndY, TSceneBlock *pstCenterBlock)
{
    TSceneBlock *pstNeighborBlock = &m_astSceneBlock[(iY) * m_iMapWidth + iX];

    //获取所有需要判断阻挡的点
    std::vector<TUNITPOSITION> vCheckPos;
    for(int x=0; x<=m_iUnitSize; ++x)
    {
        for(int y=0; y<=m_iUnitSize; ++y)
        {
            vCheckPos.push_back(TUNITPOSITION(iX+x, iY+y));
        }
    }

    //先判断阻挡，不能有阻挡
    for(unsigned i=0; i<vCheckPos.size(); ++i)
    {
        // 此块不可行走，忽略
        if(!stPathManager.BattlefieldPosCanWalk(vCheckPos[i]))
        {
            return 0;
        }
    }

    //如果不是目标点，则判断检查位置是否有人
    if(iX!=iEndX || iY!=iEndY)
    {
        for(unsigned i=0; i<vCheckPos.size(); ++i)
        {
            CCombatUnitObj* pstCombatUnitObj = stBattlefieldObj.GetCombatUnitByPos(vCheckPos[i]);
            if(pstCombatUnitObj && pstCombatUnitObj->GetCombatUnitID()!=stBattlefieldObj.GetActionUnitID())
            {
                return 0;
            }
        }
    }

    TAStarNode &stAStar = pstNeighborBlock->stAStar;

    // 已经加入封闭列表，忽略
    if (stAStar.bClosed)
    {
        return 0;
    }

    //省略G参数可以提高速度, 但得不到最优路径, 服务器做怪物短路径寻路不碍事
    int iValueG = pstCenterBlock->stAStar.iValueG + 1;

    // 在开放列表中，但路径比现在优先，忽略
    if (stAStar.bOpened && stAStar.iValueG < iValueG)
    {
        return 0;
    }

    // 更新当前节点GHF值，并指向新的中心节点
    stAStar.iValueG = iValueG;
    // stAStar.iValueH = ABS(iX, iEndX) + ABS(iY, iEndY);
    stAStar.iValueF = stAStar.iValueG + ABS(iX, iEndX) + ABS(iY, iEndY);

    stAStar.pstCenterNode = pstCenterBlock;

    // 加入开放列表
    if (!stAStar.bOpened)
    {
        bool bPushed = m_stPathMinHeap.PushHeap(pstNeighborBlock);
        if (!bPushed)
        {
            return -1;
        }

        stAStar.bOpened = true;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPathMinHeap::CPathMinHeap()
{
    m_iOpenNodes = 0;
    m_iCloseNodes = 0;

    memset(m_astOpenNode, 0, sizeof(m_astOpenNode));
    memset(m_astCloseNode, 0, sizeof(m_astCloseNode));
}

// 判断是否缓冲区满
bool CPathMinHeap::IsHeapFull()
{
    return (m_iCloseNodes >= MAX_PATH_NODE) || (m_iOpenNodes >= MAX_PATH_NODE);
}

void CPathMinHeap::Initialize()
{
    for (int i = 0; i < m_iOpenNodes; i++)
    {
        memset(&(m_astOpenNode[i]->stAStar), 0, sizeof(TAStarNode));
    }

    for (int i = 0; i < m_iCloseNodes; i++)
    {
        memset(&(m_astCloseNode[i]->stAStar), 0, sizeof(TAStarNode));
    }

    m_iOpenNodes = 0;
    m_iCloseNodes = 0;
}

TSceneBlock *CPathMinHeap::PopHeap()
{
    if (m_iOpenNodes <= 0)
    {
        return NULL;
    }

    // 保存最小值
    TSceneBlock *pstMinBlock = m_astOpenNode[0];

    // 比较两个子节点，将小的提升为父节点
    int iParent = 0;
    int iLeftChild, iRightChild;
    for (iLeftChild = 2 * iParent + 1, iRightChild = iLeftChild + 1;
        iRightChild < m_iOpenNodes;
        iLeftChild = 2 * iParent + 1, iRightChild = iLeftChild + 1)
    {
        if (m_astOpenNode[iLeftChild]->stAStar.iValueF < m_astOpenNode[iRightChild]->stAStar.iValueF)
        {
            m_astOpenNode[iParent] = m_astOpenNode[iLeftChild];
            iParent = iLeftChild;
        }
        else
        {
            m_astOpenNode[iParent] = m_astOpenNode[iRightChild];
            iParent = iRightChild;
        }
    }

    // 将最后一个节点填在空出来的节点上, 防止数组空洞
    if (iParent != m_iOpenNodes - 1)
    {
        bool bPushed = InsertHeap(m_astOpenNode[--m_iOpenNodes], iParent);
        if (!bPushed)
        {
            return NULL;
        }
    }

    m_iOpenNodes--;

    if (m_iCloseNodes < MAX_PATH_NODE)
    {
        m_astCloseNode[m_iCloseNodes++] = pstMinBlock;
    }
    else
    {
        return NULL;
    }

    return pstMinBlock;
}

bool CPathMinHeap::PushHeap(TSceneBlock *pstSceneBlock)
{
    if (m_iOpenNodes >= MAX_PATH_NODE)
    {
        return false;
    }

    return InsertHeap(pstSceneBlock, m_iOpenNodes);
}

bool CPathMinHeap::InsertHeap(TSceneBlock *pstSceneBlock, int iPosition)
{
    if (iPosition >= MAX_PATH_NODE)
    {
        return false;
    }

    m_astOpenNode[iPosition] = pstSceneBlock;

    // 依次和父节点比较，如果比父节点小，则上移
    int iChild, iParent;
    for (iChild = iPosition, iParent = (iChild - 1) / 2;
        iChild > 0;
        iChild = iParent, iParent = (iChild - 1) / 2)
    {
        if (m_astOpenNode[iChild]->stAStar.iValueF < m_astOpenNode[iParent]->stAStar.iValueF)
        {
            TSceneBlock *tmp = m_astOpenNode[iParent];
            m_astOpenNode[iParent] = m_astOpenNode[iChild];
            m_astOpenNode[iChild] = tmp;
        }
        else
        {
            break;
        }
    }

    m_iOpenNodes++;

    return true;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
