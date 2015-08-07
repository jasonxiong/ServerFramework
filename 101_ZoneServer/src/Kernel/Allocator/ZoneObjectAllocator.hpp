#ifndef __ZONE_OBJECT_ALLOCATOR_HPP__
#define __ZONE_OBJECT_ALLOCATOR_HPP__

#include "ObjAllocator.hpp"
#include "SharedMemory.hpp"

using namespace ServerLib;

class CZoneObjectAllocator
{
public:
	CZoneObjectAllocator();

    int Initialize(bool bResumeMode);

	size_t GetObjTotalSize();

private:
    size_t m_iObjTotalSize;
	CSharedMemory m_stSharedMemory;
};

#endif
