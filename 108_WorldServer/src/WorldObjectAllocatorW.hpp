#ifndef __WORLD_OBJECT_ALLOCATOR_W_HPP__
#define __WORLD_OBJECT_ALLOCATOR_W_HPP__

#include "ObjAllocator.hpp"
#include "SharedMemory.hpp"
#include "WorldRoleStatus.hpp"
#include "WorldObjectHelperW_K32.hpp"
#include "WorldObjectHelperW_K64.hpp"

using namespace ServerLib;

class CWorldObjectAllocatorW
{
public:
	CWorldObjectAllocatorW();

	int Initialize(bool bResume);

	size_t GetObjTotalSize();

private:
    size_t m_iObjTotalSize;

	CSharedMemory m_stSharedMemory;

};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
