#include "RoleSeqRecordObj.hpp"
#include "LRUHashCache.hpp"
#include "RoleDBObjectAllocator.hpp"
#include "StringUtility.hpp"
#include "ConfigHelper.hpp"

CServerObjectAllocator::CServerObjectAllocator()
{
    m_iTotalSize = 0;
}

int CServerObjectAllocator::Initialize(bool bResumeMode)
{
    // 所有需要缓存的对象共用一块共享内存区
    size_t iShmSize = CaculateTotalSize();
    int iRet = m_stShm.CreateShmSegmentByKey(GenerateServerShmKey(GAME_SERVER_ROLEDB,1), iShmSize);
    if (iRet < 0)
    {
        return -1;
    }
    
    // 从共享内存中分配各种对象缓存区
    //todo jasonxiong2 这边不需要分配缓存
    //CLRUHashCache<CRoleSeqRecordObj>::AllocateFromShm(m_stShm);

    return 0;
}

size_t CServerObjectAllocator::CaculateTotalSize()
{
    if (m_iTotalSize > 0)
    {
        return m_iTotalSize;
    }

    // 角色序号记录对象缓存区
    //todo jasonxiong2 这边不需要分配缓存
    //size_t iRoleSeqRecordCacheSize = CLRUHashCache<CRoleSeqRecordObj>::CaculateSize(
    //                                  MAX_ROLE_SEQ_RECORD_OBJ_NUMBER);
    //m_iTotalSize += iRoleSeqRecordCacheSize;

    return m_iTotalSize;
}
