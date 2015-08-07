/**
*@file HashMap.hpp
*@author jasonxiong
*@date 2009-12-08
*@version 1.0
*@brief 利用Hash索引来实现的简单Map
*
*	节点个数需要预先指定大小，暂不支持在节点用完时动态扩展的能力
*/

#ifndef __HASH_MAP_HPP__
#define __HASH_MAP_HPP__

#include "ErrorDef.hpp"

namespace ServerLib
{

const int DEFAULT_HASH_MAP_NODE_SIZE = 1024; //!<默认HashMap可容纳节点个数
const int DEFAULT_HASH_MAP_SIZE = 1024; //!<默认Hash表大小，这个值越大冲突越小

typedef enum enmHashNodeUseFlag
{
    EHNF_NOT_USED = 0, //!<Hash节点未使用
    EHNF_USED = 1, //!<Hash节点已使用
} ENMHASHNODEUSEFLAG;

template <typename KEY_TYPE>
class CDefaultHashKeyCmp
{
public:
    CDefaultHashKeyCmp() {}
    ~CDefaultHashKeyCmp() {}

public:
    bool operator() (const KEY_TYPE& rstKey1, const KEY_TYPE& rstKey2);
};

template <typename KEY_TYPE>
bool CDefaultHashKeyCmp<KEY_TYPE>::operator() (const KEY_TYPE& rstKey1, const KEY_TYPE& rstKey2)
{
    if(memcmp((const void*)&rstKey1, (const void*)&rstKey2, sizeof(rstKey1)) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//!三个模板值，Key是主键类型，T是数据类型，NODE_SIZE是数据最大的大小，需要预先分配，HASH_SIZE是Hash表大小
template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE = DEFAULT_HASH_MAP_NODE_SIZE,
         typename CMP_FUNC = CDefaultHashKeyCmp<KEY_TYPE>, int HASH_SIZE = NODE_SIZE>
class CHashMap
{
    typedef struct tagHashMapNode
    {
        KEY_TYPE m_stKey; //!<节点主键值，根据这个Key计算出Hash值来找到节点
        DATA_TYPE m_stData; //!<存放数据
        char m_cUseFlag; //!<节点是否使用
        int m_iHashNext; //!<当Hash值冲突值，将新加节点放在节点后面，形成冲突链
    } THashMapNode;

public:
    CHashMap();
    ~CHashMap();

public:
    ////!通过数组下标的方式获取值（这里类似STL的Map如果发现该Key值的节点不存在会插入）
    ////TBD 由于不做异常机制，暂时不实现
    //DATA_TYPE& operator[](const KEY_TYPE& rstKeyval);

    //!指定Key值来获取数据指针，如果不存在则返回NULL，否则直接返回数据的指针
    DATA_TYPE* Find(const KEY_TYPE& rstKeyval);

    //!指定索引值来获取数据指针，一般用于遍历中
    DATA_TYPE* GetByIndex(int iDataIdx);

    //!指定索引值来获取关键字指针，一般用于遍历中
    KEY_TYPE* GetKeyByIndex(int iDataIdx);

    //!指定Key值来插入一个数据（如果有相同Key值的节点存在，则失败）
    int Insert(const KEY_TYPE& rstKeyval, const DATA_TYPE& rstData);

    //!指定Key值来更新一个数据（如果未发现该Key值的数据则不做任何事）
    int Update(const KEY_TYPE& rstKeyval, const DATA_TYPE& rstData);

    //!指定Key值来更新一个数据（如果未发现该Key值的数据则插入一个数据）
    int Replace(const KEY_TYPE& rstKeyval, const DATA_TYPE& rstData);

    //!获取已用节点个数
    int GetUsedNum() const
    {
        return m_iUsedNodeNum;
    }

    //!获取可用节点个数
    int GetFreeNum() const
    {
        return NODE_SIZE - m_iUsedNodeNum;
    }

    //!获取总共的节点个数
    int GetSize() const
    {
        return NODE_SIZE;
    }

    //!清除所有节点
    int Erase();

    //!清除对应Key的节点
    int Erase(const KEY_TYPE& rstKey);

    //!在接口返回错误时，调用这个函数获取错误号
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    //!将数据Key值通过Hash转换成索引
    int HashKeyToIndex(const KEY_TYPE& rstKey, int& riIndex) const;

    //!设置错误号
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    int m_iErrorNO; //!错误码
    int m_iUsedNodeNum; //!<已经使用的节点个数
    int m_iFirstFreeIdx; //!<空闲链表头节点
    THashMapNode m_astHashMap[NODE_SIZE]; //!<所有存放的数据节点
    int m_aiHashFirstIdx[HASH_SIZE]; //!<通过Key来Hash计算出的冲突链表的头节点索引
};

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::CHashMap()
{
    m_iErrorNO = 0;
    m_iUsedNodeNum = 0;
    m_iFirstFreeIdx = 0;

    int i;
    for(i = 0; i < NODE_SIZE; ++i)
    {
        m_astHashMap[i].m_iHashNext = i + 1;
        m_astHashMap[i].m_cUseFlag = EHNF_NOT_USED;
    }

    m_astHashMap[NODE_SIZE-1].m_iHashNext = -1;

    for(i = 0; i < HASH_SIZE; ++i)
    {
        m_aiHashFirstIdx[i] = -1;
    }
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::~CHashMap()
{
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
int CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::Erase()
{
    m_iUsedNodeNum = 0;
    m_iFirstFreeIdx = 0;

    int i;
    for(i = 0; i < NODE_SIZE; ++i)
    {
        m_astHashMap[i].m_iHashNext = i + 1;
        m_astHashMap[i].m_cUseFlag = EHNF_NOT_USED;
    }

    m_astHashMap[NODE_SIZE-1].m_iHashNext = -1;

    for(i = 0; i < HASH_SIZE; ++i)
    {
        m_aiHashFirstIdx[i] = -1;
    }

    return 0;
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
DATA_TYPE* CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::GetByIndex(int iDataIdx)
{
    if(iDataIdx < 0 || iDataIdx >= NODE_SIZE)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return NULL;
    }

    if(m_astHashMap[iDataIdx].m_cUseFlag == EHNF_USED)
    {
        return &m_astHashMap[iDataIdx].m_stData;
    }
    else
    {
        return NULL;
    }
}


template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
KEY_TYPE* CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::GetKeyByIndex(int iDataIdx)
{
    if(iDataIdx < 0 || iDataIdx >= NODE_SIZE)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return NULL;
    }

    if(m_astHashMap[iDataIdx].m_cUseFlag == EHNF_USED)
    {
        return &m_astHashMap[iDataIdx].m_stKey;
    }
    else
    {
        return NULL;
    }
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
int CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::Erase(const KEY_TYPE& rstKeyval)
{
    int iHashIdx = 0;
    HashKeyToIndex(rstKeyval, iHashIdx);

    if(iHashIdx < 0 || iHashIdx >= HASH_SIZE)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    int iPreIndex = -1;
    int iCurIndex = m_aiHashFirstIdx[iHashIdx];
    CMP_FUNC stCmpFunc;

    while(iCurIndex != -1)
    {
        //已经存在该Key则返回失败
        if(stCmpFunc(rstKeyval, m_astHashMap[iCurIndex].m_stKey))
        {
            break;
        }

        iPreIndex = iCurIndex;
        iCurIndex = m_astHashMap[iCurIndex].m_iHashNext;
    }

    //找到需要删除的节点
    if(iCurIndex != -1)
    {
        //是冲突链表头节点
        if(m_aiHashFirstIdx[iHashIdx] == iCurIndex)
        {
            m_aiHashFirstIdx[iHashIdx] = m_astHashMap[iCurIndex].m_iHashNext;
        }
        //将上一个节点的Next索引等于当前要删除节点的Next索引
        else
        {
            m_astHashMap[iPreIndex].m_iHashNext = m_astHashMap[iCurIndex].m_iHashNext;
        }

        m_astHashMap[iCurIndex].m_cUseFlag = EHNF_NOT_USED;
        m_astHashMap[iCurIndex].m_iHashNext = m_iFirstFreeIdx;
        m_iFirstFreeIdx = iCurIndex;
        --m_iUsedNodeNum;
    }

    return 0;
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
int CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::Insert(const KEY_TYPE& rstKeyval, const DATA_TYPE& rstData)
{
    int iHashIdx = 0;
    HashKeyToIndex(rstKeyval, iHashIdx);

    if(iHashIdx < 0 || iHashIdx >= HASH_SIZE)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    //已经没有可用的节点了
    if(m_iFirstFreeIdx < 0)
    {
        SetErrorNO(EEN_HASH_MAP__NODE_IS_FULL);

        return -2;
    }

    int iPreIndex = -1;
    int iCurIndex = m_aiHashFirstIdx[iHashIdx];
    CMP_FUNC stCmpFunc;

    while(iCurIndex != -1)
    {
        //已经存在该Key则返回失败
        if(stCmpFunc(rstKeyval, m_astHashMap[iCurIndex].m_stKey))
        {
            SetErrorNO(EEN_HASH_MAP__INSERT_FAILED_FOR_KEY_DUPLICATE);

            return -3;
        }

        iPreIndex = iCurIndex;
        iCurIndex = m_astHashMap[iCurIndex].m_iHashNext;
    }

    int iNowAssignIdx = m_iFirstFreeIdx;
    m_iFirstFreeIdx = m_astHashMap[m_iFirstFreeIdx].m_iHashNext;
    ++m_iUsedNodeNum;
    memcpy(&m_astHashMap[iNowAssignIdx].m_stKey, &rstKeyval, sizeof(rstKeyval));
    memcpy(&m_astHashMap[iNowAssignIdx].m_stData, &rstData, sizeof(rstData));
    m_astHashMap[iNowAssignIdx].m_cUseFlag = EHNF_USED;
    m_astHashMap[iNowAssignIdx].m_iHashNext = -1;

    //是冲突链表的第一个节点
    if(m_aiHashFirstIdx[iHashIdx] == -1)
    {
        m_aiHashFirstIdx[iHashIdx] = iNowAssignIdx;
    }
    else
    {
        m_astHashMap[iPreIndex].m_iHashNext = iNowAssignIdx;
    }

    return 0;
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
int CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::Update(const KEY_TYPE& rstKeyval, const DATA_TYPE& rstData)
{
    DATA_TYPE* pstData = Find(rstKeyval);

    if(pstData == NULL)
    {
        SetErrorNO(EEN_HASH_MAP__NODE_NOT_EXISTED);

        return -1;
    }

    memcpy(pstData, &rstData, sizeof(rstData));

    return 0;
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
int CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::Replace(const KEY_TYPE& rstKeyval, const DATA_TYPE& rstData)
{
    int iHashIdx = 0;
    HashKeyToIndex(rstKeyval, iHashIdx);

    if(iHashIdx < 0 || iHashIdx >= HASH_SIZE)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    int iPreIndex = -1;
    int iCurIndex = m_aiHashFirstIdx[iHashIdx];
    CMP_FUNC stCmpFunc;

    while(iCurIndex != -1)
    {
        //已经存在该Key则直接更新
        if(stCmpFunc(rstKeyval, m_astHashMap[iCurIndex].m_stKey))
        {
            memcpy(&m_astHashMap[iCurIndex].m_stKey, &rstKeyval, sizeof(rstKeyval));
            memcpy(&m_astHashMap[iCurIndex].m_stData, &rstData, sizeof(rstData));

            return 0;
        }

        iPreIndex = iCurIndex;
        iCurIndex = m_astHashMap[iCurIndex].m_iHashNext;
    }

    //已经没有可用的节点了
    if(m_iFirstFreeIdx < 0)
    {
        SetErrorNO(EEN_HASH_MAP__NODE_IS_FULL);

        return -2;
    }

    int iNowAssignIdx = m_iFirstFreeIdx;
    m_iFirstFreeIdx = m_astHashMap[m_iFirstFreeIdx].m_iHashNext;
    ++m_iUsedNodeNum;
    memcpy(&m_astHashMap[iNowAssignIdx].m_stKey, &rstKeyval, sizeof(rstKeyval));
    memcpy(&m_astHashMap[iNowAssignIdx].m_stData, &rstData, sizeof(rstData));
    m_astHashMap[iNowAssignIdx].m_cUseFlag = EHNF_USED;
    m_astHashMap[iNowAssignIdx].m_iHashNext = -1;

    //是冲突链表的第一个节点
    if(m_aiHashFirstIdx[iHashIdx] == -1)
    {
        m_aiHashFirstIdx[iHashIdx] = iNowAssignIdx;
    }
    else
    {
        m_astHashMap[iPreIndex].m_iHashNext = iNowAssignIdx;
    }

    return 0;
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
DATA_TYPE* CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::Find(const KEY_TYPE& rstKeyval)
{
    int iHashIdx = 0;
    HashKeyToIndex(rstKeyval, iHashIdx);

    if(iHashIdx < 0 || iHashIdx >= HASH_SIZE)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return NULL;
    }

    int iCurIndex = m_aiHashFirstIdx[iHashIdx];
    CMP_FUNC stCmpFunc;

    while(iCurIndex != -1)
    {
        if(stCmpFunc(rstKeyval, m_astHashMap[iCurIndex].m_stKey))
        {
            return &m_astHashMap[iCurIndex].m_stData;
        }

        iCurIndex = m_astHashMap[iCurIndex].m_iHashNext;
    }

    return NULL;
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
int CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::HashKeyToIndex(const KEY_TYPE& rstKey, int& riIndex) const
{
    size_t uiKeyLength = sizeof(rstKey);
    unsigned int uiHashSum = 0;
    unsigned int *piTemp = (unsigned int*)&rstKey;

    //目前Hash算法实现比较简单只是将Key值的每个字节的值加起来并对SIZE取模
    unsigned int i;
    for( i = 0; i < uiKeyLength / sizeof(unsigned int); ++i)
    {
        uiHashSum += piTemp[i];
    }

    if(uiKeyLength % sizeof(unsigned int) > 0)
    {
        unsigned char* pByte = (unsigned char*)&rstKey;
        pByte += (uiKeyLength - (uiKeyLength % sizeof(unsigned int)));
        unsigned int uiTemp;
        memcpy((void *)&uiTemp, (const void *)pByte, uiKeyLength%sizeof(unsigned int));
        uiHashSum += uiTemp;
    }

    uiHashSum = (uiHashSum & ((unsigned int)0x7fffffff));

    riIndex = (int)(uiHashSum % HASH_SIZE);

    return 0;
}

}

#endif //__HASH_MAP_HPP__
///:~
