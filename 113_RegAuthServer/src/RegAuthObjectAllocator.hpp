#ifndef __KYLIN_OBJECT_ALLOCATOR_HPP__
#define __KYLIN_OBJECT_ALLOCATOR_HPP__

#include "SharedMemory.hpp"
#include "SingletonTemplate.hpp"

using namespace ServerLib;

class CRegAuthObjectAllocator
{
private:
    CSharedMemory m_stShm;
    size_t m_iTotalSize; // 本application中各种类型的对象缓存区的大小之和

private:
    // 本类的对象只能在CSingleton<CRegAuthObjectAllocator>类中创建
    friend class CSingleton<CRegAuthObjectAllocator>;
    CRegAuthObjectAllocator();

public:
    int Initialize(bool bResume);

private:

    // 新增cache时需要在下面两个函数中做响应修改
    size_t CaculateTotalSize();
};

#endif // __KYLIN_OBJECT_ALLOCATOR_HPP__
