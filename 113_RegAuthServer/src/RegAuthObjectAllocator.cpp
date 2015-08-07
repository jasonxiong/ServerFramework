#include "SessionObj.hpp"
#include "RegAuthObjectAllocator.hpp"
#include "ConfigHelper.hpp"
#include "LogAdapter.hpp"

CRegAuthObjectAllocator::CRegAuthObjectAllocator()
{
    m_iTotalSize = 0;
}

int CRegAuthObjectAllocator::Initialize(bool bResume)
{
    int iShmSize = CaculateTotalSize();
    if(iShmSize <= 0)
    {
        return 0;
    }


    // 所有需要缓存的对象共用一块共享内存区
    int iRet = m_stShm.CreateShmSegmentByKey(GenerateServerShmKey(GAME_SERVER_REGAUTH,1), iShmSize);
    if (iRet < 0)
    {
        return -1;
    }

    return 0;
}

size_t CRegAuthObjectAllocator::CaculateTotalSize()
{
    if (m_iTotalSize > 0)
    {
        return m_iTotalSize;
    }

    //todo jasonxiong 后面做优化时再看是否需要下面的对象
    /*
    // 创建角色消息缓存区，以uin作为hash key
    size_t iRegisterAccountRequestCacheSize =
        CFixedHashCache<CRegisterAccountRequestObj>::CaculateSize(
            MAX_REGISTER_ACCOUNT_REQUEST_NUMBER);
    m_iTotalSize += iRegisterAccountRequestCacheSize;

    // 玩家帐号->uin,passwd的映射的缓存，不需要每次认证都向数据库查询
    size_t iAccountCacheSize =
        CLRUHashCache<CAccountUinObj>::CaculateSize(MAX_ACCOUNT_OBJ_CACHE_NUMBER);
    m_iTotalSize += iAccountCacheSize;
    */

    //LOGDEBUG("RegisterAccountReuqest cache size %zu, Account cache size %zu!\n", iRegisterAccountRequestCacheSize, iAccountCacheSize);

    return m_iTotalSize;
}
