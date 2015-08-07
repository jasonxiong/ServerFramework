/**
*@file LinearQueue.hpp
*@author jasonxiong
*@date 2009-12-21
*@version 1.0
*@brief 线性队列类
*
*
*/

#ifndef __LINEAR_QUEUE_HPP__
#define __LINEAR_QUEUE_HPP__

#include "ErrorDef.hpp"

namespace ServerLib
{

template <typename DATA_TYPE, int MAX_NODE_NUMBER>
class CLinearQueue
{
public:
    CLinearQueue();
    ~CLinearQueue();

public:
    //!获取队列首部数据
    const DATA_TYPE* GetHeadData() const;

    //!获取队列尾部数据
    const DATA_TYPE* GetTailData() const;

    //!获取队列长度
    int GetQueueLength() const;

    //!获取队列首部数据，并将其Pop出队列
    DATA_TYPE* PopHeadData();

    //!清空队列
    int Clear();

    //!向队列中Push数据
    int PushData(const DATA_TYPE& rstData);

    //!在接口返回错误时，调用这个函数获取错误号
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    int m_iErrorNO; //!错误码
    DATA_TYPE m_astData[MAX_NODE_NUMBER]; //!<数据数组
    int m_iHeadIdx; //!<队列首部索引
    int m_iTailIdx; //!<队列尾部索引
};

template <typename DATA_TYPE, int MAX_NODE_NUMBER>
CLinearQueue<DATA_TYPE, MAX_NODE_NUMBER>::CLinearQueue()
{
    m_iHeadIdx = -1;
    m_iTailIdx = -1;
    m_iErrorNO = 0;
}

template <typename DATA_TYPE, int MAX_NODE_NUMBER>
CLinearQueue<DATA_TYPE, MAX_NODE_NUMBER>::~CLinearQueue()
{

}

template <typename DATA_TYPE, int MAX_NODE_NUMBER>
int CLinearQueue<DATA_TYPE, MAX_NODE_NUMBER>::Clear()
{
    m_iHeadIdx = -1;
    m_iTailIdx = -1;
    m_iErrorNO = 0;

    return 0;
}

template <typename DATA_TYPE, int MAX_NODE_NUMBER>
const DATA_TYPE* CLinearQueue<DATA_TYPE, MAX_NODE_NUMBER>::GetHeadData() const
{
    //不存在头节点
    if(m_iHeadIdx < 0 || m_iHeadIdx >= MAX_NODE_NUMBER)
    {
        return NULL;
    }

    return &m_astData[m_iHeadIdx];
}

template <typename DATA_TYPE, int MAX_NODE_NUMBER>
const DATA_TYPE* CLinearQueue<DATA_TYPE, MAX_NODE_NUMBER>::GetTailData() const
{
    //不存在尾节点
    if(m_iTailIdx < 0 || m_iTailIdx >= MAX_NODE_NUMBER)
    {
        return NULL;
    }

    return &m_astData[m_iTailIdx];
}

template <typename DATA_TYPE, int MAX_NODE_NUMBER>
int CLinearQueue<DATA_TYPE, MAX_NODE_NUMBER>::GetQueueLength() const
{
    if(m_iHeadIdx == -1)
    {
        return 0;
    }

    if(m_iHeadIdx <= m_iTailIdx)
    {
        return (m_iTailIdx - m_iHeadIdx) + 1;
    }
    else
    {
        return MAX_NODE_NUMBER - (m_iHeadIdx - m_iTailIdx) + 1;
    }
}


template <typename DATA_TYPE, int MAX_NODE_NUMBER>
DATA_TYPE* CLinearQueue<DATA_TYPE, MAX_NODE_NUMBER>::PopHeadData()
{
    //不存在头节点
    if(m_iHeadIdx < 0 || m_iHeadIdx >= MAX_NODE_NUMBER)
    {
        return NULL;
    }

    DATA_TYPE* pstHeadData = &m_astData[m_iHeadIdx];

    //队列只有一个节点了
    if(m_iHeadIdx == m_iTailIdx)
    {
        m_iHeadIdx = -1;
        m_iTailIdx = -1;
    }
    else
    {
        m_iHeadIdx = (m_iHeadIdx + 1) % MAX_NODE_NUMBER;
    }

    return pstHeadData;
}

template <typename DATA_TYPE, int MAX_NODE_NUMBER>
int CLinearQueue<DATA_TYPE, MAX_NODE_NUMBER>::PushData(const DATA_TYPE& rstData)
{
    if((m_iTailIdx + 1) % MAX_NODE_NUMBER == m_iHeadIdx)
    {
        SetErrorNO(EEN_LINEAR_QUEUE__QUEUE_IS_FULL);

        return -1;
    }

    //队列中没有数据
    if(m_iTailIdx == -1)
    {
        m_iHeadIdx = m_iTailIdx = 0;
        m_astData[m_iTailIdx] = rstData;
    }
    else
    {
        m_iTailIdx = (m_iTailIdx + 1) % MAX_NODE_NUMBER;
        m_astData[m_iTailIdx] = rstData;
    }

    return 0;
}

}

#endif //__LINEAR_QUEUE_HPP__
///:~
