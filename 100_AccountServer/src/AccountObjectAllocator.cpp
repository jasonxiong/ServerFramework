#include "CreateRoleRequestObj.hpp"
#include "RoleNumberObj.hpp"
#include "SessionObj.hpp"
#include "FixedHashCache.hpp"
#include "LRUHashCache.hpp"
#include "AccountObjectAllocator.hpp"
#include "ConfigHelper.hpp"

CAccountObjectAllocator::CAccountObjectAllocator()
{
    m_iTotalSize = 0;
}

int CAccountObjectAllocator::Initialize(bool bResume)
{
    int iShmSize = CaculateTotalSize();

    // 所有需要缓存的对象共用一块共享内存区
    int iRet = m_stShm.CreateShmSegmentByKey(GenerateServerShmKey(GAME_SERVER_ACCOUNT,1), iShmSize);
    if (iRet < 0)
    {
        return -1;
    }

    // 创建角色消息缓存区
    CFixedHashCache<CCreateRoleRequestObj>::AllocateFromShm(m_stShm, bResume);

    // 用户在各个world上的角色个数缓存区
    CLRUHashCache<CRoleNumberObj>::AllocateFromShm(m_stShm, bResume);

    return 0;
}

size_t CAccountObjectAllocator::CaculateTotalSize()
{
    if (m_iTotalSize > 0)
    {
        return m_iTotalSize;
    }

    // 创建角色消息缓存区，以uin作为hash key
    size_t iCreateRoleRequestCacheSize =
        CFixedHashCache<CCreateRoleRequestObj>::CaculateSize(
            MAX_ROLE_OBJECT_NUMBER_IN_WORLD);
    m_iTotalSize += iCreateRoleRequestCacheSize;

    // 用户在各个world上的角色个数缓存区，以uin作为hash key
    size_t iRoleNumberCacheSize =
        CLRUHashCache<CRoleNumberObj>::CaculateSize(MAX_ROLE_OBJECT_NUMBER_IN_WORLD);
    m_iTotalSize += iRoleNumberCacheSize;

    return m_iTotalSize;
}
