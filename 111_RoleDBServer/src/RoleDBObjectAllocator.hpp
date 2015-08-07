#ifndef __GAME_OBJECT_ALLOCATOR_HPP__
#define __GAME_OBJECT_ALLOCATOR_HPP__

#include "SharedMemory.hpp"
#include "SingletonTemplate.hpp"

using namespace ServerLib;

class CServerObjectAllocator
{
public:
    CServerObjectAllocator();
    int Initialize(bool bResumeMode);

private:
    // 新增cache时需要在下面两个函数中做响应修改
    size_t CaculateTotalSize();

private:
    CSharedMemory m_stShm;
    size_t m_iTotalSize; // 本application中各种类型的对象缓存区的大小之和

};

#endif // __GAME_OBJECT_ALLOCATOR_HPP__
