#ifndef __LRU_HASH_CACHE_HPP__
#define __LRU_HASH_CACHE_HPP__

#include "SharedMemory.hpp"
#include "ObjAllocator.hpp"
#include "HashMap_K32.hpp"
#include "BiDirQueue.hpp"
//#include "LogAdapter.hpp"

#define HASH_NODE_MULTIPLE    2 // hash结点个数为对象结点个数的2倍

using namespace ServerLib;

// 需要自动回收的cache对象管理器，以hash方式和双向静态链表方式管理。
// 这种cache由三部分组成：对象段（包括idx数组和obj数组），辅助hash数组段，辅助queue段。
// 最近使用的对象结点挂在queue的尾部，这样在创建和访问某个对象结点时均需要将其移动
// 到queue的尾部，使得queue的头部是最近最久未使用的结点。
// hash结点数一般为对象结点数的若干倍，queue结点数等于对象结点数
template <typename TConcreteObj>
class CLRUHashCache
{
private:
    static CObjAllocator* m_pAllocator; // 对象段，包括idx和obj，idx用于obj结点的分配、回收管理
    static CHashMap_K32* m_pHashMap;    // 管理对象段的hash表，用于快速访问
    static CBiDirQueue* m_pBiDirQueue;  // 管理对象段的双向队列，用于回收位于队列头部的最近最久未使用的对象结点

private:
    static int m_iObjNodeNumber;   // 对象结点总个数，分配空间时使用
    static int m_iHashNodeNumber;  // hash结点总个数，分配空间时使用
    static int m_iQueueNodeNumber; // queue结点总个数，分配空间时使用
    static int m_iSingleObjSize;   // 单个对象的大小

    // 私有的辅助函数
private:
    static void SetSingleObjSize();
    static int GetSingleObjSize();

    static void SetObjNodeNumber(const int iNodeNumber);
    static int GetObjNodeNumber();

    // 为减少冲突，hash结点空间一般为对象结点空间的若干倍，iNodeNumber是
    // 对象结点个数，iMultiple是倍数，取值为1,2,3...
    static void SetHashNodeNumber(const int iNodeNumber, const int iMultiple);
    static int GetHashNodeNumber();

    static void SetQueueNodeNumber(const int iNodeNumber);
    static int GetQueueNodeNumber();

    // 设置结点个数
    static void SetNodeNumber(const int iNodeNumber);

    static int CaculateObjSegSize();
    static int CaculateHashSegSize();
    static int CaculateQueueSegSize();

    // 分配缓存时先调用CaculateSize计算需要分配的大小，然后调用AllocateFromShm实施分配
public:
    static int CaculateSize(const int iNodeNumber);
    static void AllocateFromShm(CSharedMemory& shm, bool bResume);

private:
    // 释放双向队列头部的iNodeNumber个结点
    static void SwapOut(const int iNodeNumber);

    // 创建、删除、访问单个对象结点的接口
public:
    static TConcreteObj* CreateByUin(const unsigned int uiUin);
    static int DeleteByUin(const unsigned int uiUin);
    static TConcreteObj* GetByUin(const unsigned int uiUin);
    static int GetUsedNodeNumber();
    static int GetFreeNodeNumber();
};

// 静态数据成员的初始化
template <typename TConcreteObj>
CObjAllocator* CLRUHashCache<TConcreteObj>::m_pAllocator = NULL;

template <typename TConcreteObj>
CHashMap_K32* CLRUHashCache<TConcreteObj>::m_pHashMap = NULL;

template <typename TConcreteObj>
CBiDirQueue* CLRUHashCache<TConcreteObj>::m_pBiDirQueue = NULL;

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::m_iObjNodeNumber = 0;

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::m_iHashNodeNumber = 0;

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::m_iQueueNodeNumber = 0;

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::m_iSingleObjSize = 0;

// 成员函数
template <typename TConcreteObj>
void CLRUHashCache<TConcreteObj>::SetSingleObjSize()
{
    m_iSingleObjSize = sizeof(TConcreteObj);
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::GetSingleObjSize()
{
    return m_iSingleObjSize;
}

template <typename TConcreteObj>
void CLRUHashCache<TConcreteObj>::SetObjNodeNumber(const int iNodeNumber)
{
    m_iObjNodeNumber = iNodeNumber;
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::GetObjNodeNumber()
{
    return m_iObjNodeNumber;
}

template <typename TConcreteObj>
void CLRUHashCache<TConcreteObj>::SetHashNodeNumber(const int iNodeNumber,
        const int iMultiple)
{
    m_iHashNodeNumber = iNodeNumber * iMultiple;
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::GetHashNodeNumber()
{
    return m_iHashNodeNumber;
}

template <typename TConcreteObj>
void CLRUHashCache<TConcreteObj>::SetQueueNodeNumber(const int iNodeNumber)
{
    m_iQueueNodeNumber = iNodeNumber;
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::GetQueueNodeNumber()
{
    return m_iQueueNodeNumber;
}

template <typename TConcreteObj>
void CLRUHashCache<TConcreteObj>::SetNodeNumber(const int iNodeNumber)
{
    SetObjNodeNumber(iNodeNumber);
    SetHashNodeNumber(iNodeNumber, HASH_NODE_MULTIPLE);
    SetQueueNodeNumber(iNodeNumber);
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::CaculateObjSegSize()
{
    return CObjAllocator::CountSize(GetSingleObjSize(), GetObjNodeNumber());
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::CaculateHashSegSize()
{
    return CHashMap_K32::CountSize(GetHashNodeNumber());
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::CaculateQueueSegSize()
{
    return CBiDirQueue::CountSize(GetQueueNodeNumber());
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::CaculateSize(const int iNodeNumber)
{
    SetSingleObjSize();
    SetNodeNumber(iNodeNumber);

    int iObjSegSize = CaculateObjSegSize();
    int iHashSegSize = CaculateHashSegSize();
    int iQueueSegSize = CaculateQueueSegSize();

    return (iObjSegSize + iHashSegSize + iQueueSegSize);
}

template <typename TConcreteObj>
void CLRUHashCache<TConcreteObj>::AllocateFromShm(CSharedMemory& shm, bool bResume)
{
    if (!bResume)
    {
        m_pAllocator = CObjAllocator::CreateByGivenMemory(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetSingleObjSize(), GetObjNodeNumber(),
            TConcreteObj::CreateObject);
        ASSERT_AND_LOG_RTN_VOID(m_pAllocator);
        shm.UseShmBlock(CaculateObjSegSize());

        m_pHashMap = CHashMap_K32::CreateHashMap(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetHashNodeNumber());
        ASSERT_AND_LOG_RTN_VOID(m_pHashMap);
        shm.UseShmBlock(CaculateHashSegSize());

        // 管理cache访问时间的双向队列
        m_pBiDirQueue = CBiDirQueue::CreateByGivenMemory(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetQueueNodeNumber());
        ASSERT_AND_LOG_RTN_VOID(m_pBiDirQueue);
        shm.UseShmBlock(CaculateQueueSegSize());
    }
    else
    {
        m_pAllocator = CObjAllocator::ResumeByGivenMemory(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetSingleObjSize(), GetObjNodeNumber(),
            TConcreteObj::CreateObject);
        ASSERT_AND_LOG_RTN_VOID(m_pAllocator);
        shm.UseShmBlock(CaculateObjSegSize());

        m_pHashMap = CHashMap_K32::ResumeHashMap(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetHashNodeNumber());
        ASSERT_AND_LOG_RTN_VOID(m_pHashMap);
        shm.UseShmBlock(CaculateHashSegSize());

        // 管理cache访问时间的双向队列
        m_pBiDirQueue = CBiDirQueue::ResumeByGivenMemory(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetQueueNodeNumber());
        ASSERT_AND_LOG_RTN_VOID(m_pBiDirQueue);
        shm.UseShmBlock(CaculateQueueSegSize());
    }

    if (bResume)
    {
        // 恢复使用中的obj
        int iUsedIdx = m_pAllocator->GetUsedHead();
        while (iUsedIdx != -1)
        {
            CObj* pObj = m_pAllocator->GetObj(iUsedIdx);
            pObj->Resume();
            iUsedIdx = m_pAllocator->GetIdx(iUsedIdx)->GetNextIdx();
        }
    }
}

template <typename TConcreteObj>
void CLRUHashCache<TConcreteObj>::SwapOut(const int iNodeNumber)
{
    int iRemoveObjID = -1;
    int i = 0;

    while (i < iNodeNumber)
    {
        iRemoveObjID = m_pBiDirQueue->GetHeadItem();
        TConcreteObj* pObj = (TConcreteObj*)m_pAllocator->GetObj(iRemoveObjID);

        ASSERT_AND_LOG_RTN_VOID(pObj);
        unsigned int uiUin = pObj->GetUin();
        //TRACESVR("swap out, uin: %u, obj id: %d\n", uiUin, iRemoveObjID);

        DeleteByUin(uiUin);
        i++;
    }
}

template <typename TConcreteObj>
TConcreteObj* CLRUHashCache<TConcreteObj>::CreateByUin(const unsigned int uiUin)
{
    // 如果没有可用结点了，则释放最近最久未使用的10个结点
    if (GetFreeNodeNumber() < 1)
    {
        SwapOut(10);
    }

    // 从对象段中征用一个未使用的结点
    int iObjID = m_pAllocator->CreateObject();
    if (iObjID < 0)
    {
        TRACESVR("Create object failed\n");
        return NULL;
    }

    // 纳入辅助hash表的管理
    int iRet = m_pHashMap->InsertValueByKey(uiUin, iObjID);
    if (iRet < 0)
    {
        TRACESVR("Insert object to hash table failed\n");
        return NULL;
    }

    // 将最新的结点挂到queue的尾部，纳入辅助queue的管理
    iRet = m_pBiDirQueue->AppendItemToTail(iObjID);
    if (iRet != 0)
    {
        TRACESVR("Append object to tail of LRU queue failed\n");
        return NULL;
    }

    //TRACESVR("create ok, uin: %u, obj id: %d\n", uiUin, iObjID);

    return (TConcreteObj*)m_pAllocator->GetObj(iObjID);
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::DeleteByUin(const unsigned int uiUin)
{
    int iObjID = 0;

    // 脱离辅助hash表的管理
    int iRet = m_pHashMap->DeleteByKey(uiUin, iObjID);
    if (iRet < 0)
    {
        TRACESVR("Delete object from hash table failed\n");
        return -1;
    }

    // 脱离辅助queue的管理
    iRet = m_pBiDirQueue->DeleteItem(iObjID);
    if (iRet < 0)
    {
        TRACESVR("Delete object from LRU queue failed\n");
        return -2;
    }

    // 将结点归还到对象段中成为未使用结点
    iRet = m_pAllocator->DestroyObject(iObjID);
    if (iRet < 0)
    {
        TRACESVR("Destroy object failed\n");
        return -3;
    }

    //TRACESVR("delete ok, obj id: %d\n", iObjID);

    return 0;
}

template <typename TConcreteObj>
TConcreteObj* CLRUHashCache<TConcreteObj>::GetByUin(const unsigned int uiUin)
{
    int iObjID = 0;

    // 通过辅助hash表实现快速访问
    int iRet = m_pHashMap->GetValueByKey(uiUin, iObjID);
    if (iRet < 0)
    {
        TRACESVR("Get object from hash table failed\n");
        return NULL;
    }

    // 将该结点移动到queue的尾部
    iRet = m_pBiDirQueue->AppendItemToTail(iObjID);
    if (iRet != 0)
    {
        TRACESVR("Appent object to tail of LRU queue failed\n");
        return NULL;
    }

    //TRACESVR("get ok, uin: %u, obj id: %d\n", uiUin, iObjID);

    return (TConcreteObj*)m_pAllocator->GetObj(iObjID);
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::GetUsedNodeNumber()
{
    return m_pAllocator->GetUsedCount();
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::GetFreeNodeNumber()
{
    return m_pAllocator->GetFreeCount();
}

#endif // __LRU_HASH_CACHE_HPP__
