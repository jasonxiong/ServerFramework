#ifndef __FIXED_HASH_CACHE_HPP__
#define __FIXED_HASH_CACHE_HPP__

#include "SharedMemory.hpp"
#include "ObjAllocator.hpp"
#include "HashMap_KString.hpp"
//#include "LogAdapter.hpp"

#define HASH_NODE_MULTIPLE    2 // hash结点个数为对象结点个数的2倍

using namespace ServerLib;

// 结点个数固定的cache对象管理器，以hash方式管理。
// 这种cache由两部分组成：对象段（包括idx数组和obj数组），辅助hash数组段。
// hash结点数一般为对象结点数的若干倍
template <typename TConcreteObj>
class CFixedHashCache
{
private:
    static CObjAllocator* m_pAllocator; // 对象段，包括idx和obj，idx用于obj结点的分配、回收管理
    static CHashMap_KString<unsigned>* m_pHashMap;    // 管理对象段的hash表，用于快速访问

private:
    static int m_iObjNodeNumber;   // 对象结点总个数，分配空间时使用
    static int m_iHashNodeNumber;  // hash结点总个数，分配空间时使用
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

    // 设置结点个数
    static void SetNodeNumber(const int iNodeNumber);

    static int CaculateObjSegSize();
    static int CaculateHashSegSize();

    // 分配缓存时先调用CaculateSize计算需要分配的大小，然后调用AllocateFromShm实施分配
public:
    static int CaculateSize(const int iNodeNumber);
    static void AllocateFromShm(CSharedMemory& shm, bool bResume);

    // 创建、删除、访问单个对象结点的接口
public:
    static TConcreteObj* CreateByKey(const TDataString& stKey);
    static TConcreteObj* CreateByKey(const char* pszKey);
    static int DeleteByKey(const TDataString& stKey);
    static TConcreteObj* GetByKey(const TDataString& stKey);
    static TConcreteObj* GetByKey(const char* pszKey);
    static int GetUsedNodeNumber();
    static int GetFreeNodeNumber();

public:
    static void ClearCache(time_t& stTime); // 清理在参数stTime之前（<=stTime）创建的cache结点
};

// 静态数据成员的初始化
template <typename TConcreteObj>
CObjAllocator* CFixedHashCache<TConcreteObj>::m_pAllocator = NULL;

template <typename TConcreteObj>
CHashMap_KString<unsigned>* CFixedHashCache<TConcreteObj>::m_pHashMap = NULL;

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::m_iObjNodeNumber = 0;

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::m_iHashNodeNumber = 0;

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::m_iSingleObjSize = 0;

// 成员函数
template <typename TConcreteObj>
void CFixedHashCache<TConcreteObj>::SetSingleObjSize()
{
    m_iSingleObjSize = sizeof(TConcreteObj);
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::GetSingleObjSize()
{
    return m_iSingleObjSize;
}

template <typename TConcreteObj>
void CFixedHashCache<TConcreteObj>::SetObjNodeNumber(const int iNodeNumber)
{
    m_iObjNodeNumber = iNodeNumber;
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::GetObjNodeNumber()
{
    return m_iObjNodeNumber;
}

template <typename TConcreteObj>
void CFixedHashCache<TConcreteObj>::SetHashNodeNumber(const int iNodeNumber,
        const int iMultiple)
{
    m_iHashNodeNumber = iNodeNumber * iMultiple;
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::GetHashNodeNumber()
{
    return m_iHashNodeNumber;
}

template <typename TConcreteObj>
void CFixedHashCache<TConcreteObj>::SetNodeNumber(const int iNodeNumber)
{
    SetObjNodeNumber(iNodeNumber);
    SetHashNodeNumber(iNodeNumber, HASH_NODE_MULTIPLE);
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::CaculateObjSegSize()
{
    return CObjAllocator::CountSize(GetSingleObjSize(), GetObjNodeNumber());
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::CaculateHashSegSize()
{
    return CHashMap_KString<unsigned>::CountSize(GetHashNodeNumber());
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::CaculateSize(const int iNodeNumber)
{
    SetSingleObjSize();
    SetNodeNumber(iNodeNumber);

    int iObjSegSize = CaculateObjSegSize();
    int iHashSegSize = CaculateHashSegSize();

    return (iObjSegSize + iHashSegSize);
}

template <typename TConcreteObj>
void CFixedHashCache<TConcreteObj>::AllocateFromShm(CSharedMemory& shm, bool bResume)
{
    if (!bResume)
    {
        m_pAllocator = CObjAllocator::CreateByGivenMemory(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetSingleObjSize(), GetObjNodeNumber(),
            TConcreteObj::CreateObject);
        ASSERT_AND_LOG_RTN_VOID(m_pAllocator);
        shm.UseShmBlock(CaculateObjSegSize());

        m_pHashMap = CHashMap_KString<unsigned>::CreateHashMap(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetHashNodeNumber());
        ASSERT_AND_LOG_RTN_VOID(m_pHashMap);
        shm.UseShmBlock(CaculateHashSegSize());
    }
    else
    {
        return;

        //todo jasonxiong 目前 CHashMap_KString 还不支持从共享内存中resume
        /*
        m_pAllocator = CObjAllocator::ResumeByGivenMemory(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetSingleObjSize(), GetObjNodeNumber(),
            TConcreteObj::CreateObject);
        ASSERT_AND_LOG_RTN_VOID(m_pAllocator);
        shm.UseShmBlock(CaculateObjSegSize());

        m_pHashMap = CHashMap_KString::ResumeHashMap(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetHashNodeNumber());
        ASSERT_AND_LOG_RTN_VOID(m_pHashMap);
        shm.UseShmBlock(CaculateHashSegSize());
        */
    }

    //todo jasonxiong 暂时不支持从共享内存中resume
    /*
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
    */
}

template <typename TConcreteObj>
TConcreteObj* CFixedHashCache<TConcreteObj>::CreateByKey(const TDataString& stKey)
{
    // 从对象段中征用一个未使用的结点，如果没有可用结点了，则返回NULL
    int iObjID = m_pAllocator->CreateObject();
    if (iObjID < 0)
    {
        TRACESVR("Create object failed\n");
        return NULL;
    }

    // 纳入辅助hash表的管理
    int iRet = m_pHashMap->InsertValueByKey(stKey, iObjID);
    if (iRet < 0)
    {
        TRACESVR("Insert object to hash table failed\n");
        return NULL;
    }

    //TRACESVR("create ok, uin: %u, obj id: %d\n", uiUin, iObjID);

    return (TConcreteObj*)m_pAllocator->GetObj(iObjID);
}

template <typename TConcreteObj>
TConcreteObj* CFixedHashCache<TConcreteObj>::CreateByKey(const char* pszKey)
{
    if(!pszKey)
    {
        TRACESVR("Failed to create by key, invalid param!\n");
        return NULL;
    }

    return CreateByKey(TDataString(pszKey));
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::DeleteByKey(const TDataString& stKey)
{
    unsigned iObjID = 0;

    // 脱离辅助hash表的管理
    int iRet = m_pHashMap->DeleteByKey(stKey, iObjID);
    if (iRet < 0)
    {
        TRACESVR("Delete object from hash table failed\n");
        return -1;
    }

    // 将结点归还到对象段中成为未使用结点
    iRet = m_pAllocator->DestroyObject(iObjID);
    if (iRet < 0)
    {
        TRACESVR("Destroy object failed\n");
        return -3;
    }

    TRACESVR("delete ok, obj id: %d\n", iObjID);

    return 0;
}

template <typename TConcreteObj>
TConcreteObj* CFixedHashCache<TConcreteObj>::GetByKey(const TDataString& stKey)
{
    unsigned iObjID = 0;

    // 通过辅助hash表实现快速访问
    int iRet = m_pHashMap->GetValueByKey(stKey, iObjID);
    if (iRet < 0)
    {
        TRACESVR("Get object from hash table failed\n");
        return NULL;
    }

    //TRACESVR("get ok, uin: %u, obj id: %d\n", uiUin, iObjID);

    return (TConcreteObj*)m_pAllocator->GetObj(iObjID);
}

template <typename TConcreteObj>
TConcreteObj* CFixedHashCache<TConcreteObj>::GetByKey(const char* pszKey)
{
    if(!pszKey)
    {
        TRACESVR("Failed to get by key in fixedhashcache!\n");
        return NULL;
    }

    unsigned iObjID = 0;

    TDataString stKey(pszKey);

    // 通过辅助hash表实现快速访问
    int iRet = m_pHashMap->GetValueByKey(stKey, iObjID);
    if (iRet < 0)
    {
        TRACESVR("Get object from hash table failed\n");
        return NULL;
    }

    //TRACESVR("get ok, uin: %u, obj id: %d\n", uiUin, iObjID);

    return (TConcreteObj*)m_pAllocator->GetObj(iObjID);
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::GetUsedNodeNumber()
{
    return m_pAllocator->GetUsedCount();
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::GetFreeNodeNumber()
{
    return m_pAllocator->GetFreeCount();
}

template <typename TConcreteObj>
void CFixedHashCache<TConcreteObj>::ClearCache(time_t& stTime)
{
    TConcreteObj* pObj = NULL;
    CIdx *pIdx = NULL;
    time_t* pstCreatedTime = NULL;
    int iNextIdx;

    int iObjID = m_pAllocator->GetUsedHead();
    while (iObjID != -1)
    {
        pObj = (TConcreteObj*)m_pAllocator->GetObj(iObjID);
        ASSERT_AND_LOG_RTN_VOID(pObj);
        // TRACESVR("obj id: %d, uin: %u\n", iObjID, pObj->GetUin());

        pIdx = m_pAllocator->GetIdx(iObjID);
        ASSERT_AND_LOG_RTN_VOID(pIdx);
        iNextIdx = pIdx->GetNextIdx();

        pstCreatedTime = pObj->GetCreatedTime();
        if (*pstCreatedTime <= stTime)
        {
            TRACESVR("clear obj cache, id: %d\n", iObjID);
            DeleteByKey(pObj->GetKey());
        }
        
        iObjID = iNextIdx;
    }
}

#endif // __FIXED_HASH_CACHE_HPP__
