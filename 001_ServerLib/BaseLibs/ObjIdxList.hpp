#ifndef __OBJIDX_LIST_HPP__
#define __OBJIDX_LIST_HPP__

#include "LogAdapter.hpp"
using namespace ServerLib;

struct TObjIdxListNode
{
    int m_iPrevIdx; // 上一个节点
    int m_iNextIdx; // 下一个节点

public:
    void Initialize()
    {
        m_iPrevIdx = -1;
        m_iNextIdx = -1;
    }
};

struct TObjIdxListHead
{
    int m_iNumber;      // 链表元素数量
    int m_iFirstIdx;    // 头部结点
    int m_iLastIdx;     // 尾部节点

public:
    void Initialize()
    {
        m_iNumber = 0;
        m_iFirstIdx = -1;
        m_iLastIdx = -1;
    }
};

typedef TObjIdxListNode* (GETOBJLISTNODE)(int iIdx);

// 将Idx加入链表中
int AddObjIdxList(TObjIdxListHead& rstListHead, int iIdx, GETOBJLISTNODE* pGetObjListNode);

// 将Idx从链表中删除
int DelObjIdxList(TObjIdxListHead& rstListHead, int iIdx, GETOBJLISTNODE* pGetObjListNode);

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
