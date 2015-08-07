#ifndef __FIXED_CACHE_HPP__
#define __FIXED_CACHE_HPP__

#include "SharedMemory.hpp"
#include "ObjAllocator.hpp"
//#include "LogAdapter.hpp"

using namespace ServerLib;

// 结点个数固定的cache对象管理器。
// 这种cache仅由一个对象段（包括idx数组和obj数组）组成。
template <typename TConcreteObj>
class CFixedCache
{
private:
    static CObjAllocator* m_pAllocator; // 对象段，包括idx和obj，idx用于obj结点的分配、回收管理

private:
    static int m_iObjNodeNumber;   // 对象结点总个数，分配空间时使用
    static int m_iSingleObjSize;   // 单个对象的大小

    // 私有的辅助函数
private:
    static void SetSingleObjSize();
    static int GetSingleObjSize();

    static void SetObjNodeNumber(const int iNodeNumber);
    static int GetObjNodeNumber();

    // 设置结点个数
    static void SetNodeNumber(const int iNodeNumber);

    static int CaculateObjSegSize();

    // 分配缓存时先调用CaculateSize计算需要分配的大小，然后调用AllocateFromShm实施分配
public:
    static int CaculateSize(const int iNodeNumber);
    static void AllocateFromShm(CSharedMemory& shm, bool bResume);

    // 创建、删除、访问单个对象结点的接口
public:
    // 从对象段中征用一个未使用的结点，如果没有可用结点了，则返回一个负值
    // 成功时返回新建的对象结点的ID
    static int Create();

    static int Delete(const int iObjID);

    // 根据对象结点的ID获取该对象结点，返回值为NULL表示失败
    static TConcreteObj* GetByID(const int iObjID);

    static int GetUsedNodeNumber();
    static int GetFreeNodeNumber();
};

// 静态数据成员的初始化
template <typename TConcreteObj>
CObjAllocator* CFixedCache<TConcreteObj>::m_pAllocator = NULL;

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::m_iObjNodeNumber = 0;

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::m_iSingleObjSize = 0;

// 成员函数
template <typename TConcreteObj>
void CFixedCache<TConcreteObj>::SetSingleObjSize()
{
    m_iSingleObjSize = sizeof(TConcreteObj);
}

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::GetSingleObjSize()
{
    return m_iSingleObjSize;
}

template <typename TConcreteObj>
void CFixedCache<TConcreteObj>::SetObjNodeNumber(const int iNodeNumber)
{
    m_iObjNodeNumber = iNodeNumber;
}

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::GetObjNodeNumber()
{
    return m_iObjNodeNumber;
}

template <typename TConcreteObj>
void CFixedCache<TConcreteObj>::SetNodeNumber(const int iNodeNumber)
{
    SetObjNodeNumber(iNodeNumber);
}

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::CaculateObjSegSize()
{
    return CObjAllocator::CountSize(GetSingleObjSize(), GetObjNodeNumber());
}

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::CaculateSize(const int iNodeNumber)
{
    SetSingleObjSize();
    SetNodeNumber(iNodeNumber);

    int iObjSegSize = CaculateObjSegSize();

    return (iObjSegSize);
}

template <typename TConcreteObj>
void CFixedCache<TConcreteObj>::AllocateFromShm(CSharedMemory& shm, bool bResume)
{
    if (!bResume)
    {
        m_pAllocator = CObjAllocator::CreateByGivenMemory(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetSingleObjSize(), GetObjNodeNumber(),
            TConcreteObj::CreateObject);
    }
    else
    {
        m_pAllocator = CObjAllocator::ResumeByGivenMemory(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetSingleObjSize(), GetObjNodeNumber(),
            TConcreteObj::CreateObject);
    }

    ASSERT_AND_LOG_RTN_VOID(m_pAllocator);

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

    shm.UseShmBlock(CaculateObjSegSize());
}

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::Create()
{
    int iObjID = m_pAllocator->CreateObject();
    if (iObjID < 0)
    {
        TRACESVR("Create object failed\n");
        return -1;
    }

    //TRACESVR("create ok, obj id: %d\n", iObjID);
    return iObjID;
}

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::Delete(const int iObjID)
{
    // 将结点归还到对象段中成为未使用结点
    int iRes = m_pAllocator->DestroyObject(iObjID);
    if (iRes < 0)
    {
        TRACESVR("Destroy object failed\n");
        return -1;
    }

    //TRACESVR("delete ok, obj id: %d\n", iObjID);
    return 0;
}

template <typename TConcreteObj>
TConcreteObj* CFixedCache<TConcreteObj>::GetByID(const int iObjID)
{
    TConcreteObj* pObj = (TConcreteObj*)m_pAllocator->GetObj(iObjID);

    //TRACESVR("get ok,obj id: %d\n", iObjID);
    return pObj;
}

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::GetUsedNodeNumber()
{
    return m_pAllocator->GetUsedCount();
}

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::GetFreeNodeNumber()
{
    return m_pAllocator->GetFreeCount();
}

#endif // __FIXED_CACHE_HPP__
