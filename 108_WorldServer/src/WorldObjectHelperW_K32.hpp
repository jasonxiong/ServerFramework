#ifndef __APP_ALLOCATOR_K32_HPP__
#define __APP_ALLOCATOR_K32_HPP__

#include "ObjAllocator.hpp"
#include "HashMap_K32.hpp"
#include "WorldRoleStatus.hpp"

using namespace ServerLib;

//对象操作器
template <class TYPE_Object>
class WorldType
{
public:
    static int Create()
    {
        return m_pAllocator->CreateObject();
    }

    static TYPE_Object* Get(const int iID)
    {
        return (TYPE_Object*)m_pAllocator->GetObj(iID);
    }

    static int Del(const int iID)
    {
        return m_pAllocator->DestroyObject(iID);
    }

public:
    static int RegisterAllocator(CObjAllocator* pAllocator)
    {
        if(!pAllocator)
        {
            return -1;
        }
        m_pAllocator = pAllocator;
        return 0;
    }

private:
    static CObjAllocator* m_pAllocator;

};

template <class TYPE_Object>
CObjAllocator* WorldType<TYPE_Object>::m_pAllocator = 0;

//32位Hash对象操作器
template <class TYPE_Object>
class WorldTypeK32
{
public:
	static TYPE_Object* CreateByUin(const unsigned int uiUin)
	{
		int iID = 0;
		iID = m_pAllocator->CreateObject();
		if(iID < 0)
		{
			return NULL;
		}

		int iRet = 0;
		iRet = m_pHashMap->InsertValueByKey(uiUin, iID);
		if(iRet < 0)
		{
			m_pAllocator->DestroyObject(iID);
			return NULL;
		}

		return (TYPE_Object*)m_pAllocator->GetObj(iID);
	}

	static TYPE_Object* GetByUin(const unsigned int uiUin)
	{
		int iID = 0;
		int iRet = 0;
		iRet = m_pHashMap->GetValueByKey(uiUin, iID);
		if(iRet < 0)
		{
			return NULL;
		}

		return (TYPE_Object*)m_pAllocator->GetObj(iID);
	}

	//删除对象还必须删除Hash
	static int DeleteByUin(const unsigned int uiUin)
	{
		int iID = 0;
		int iRet = 0;
		iRet = m_pHashMap->DeleteByKey(uiUin, iID);
		if(iRet < 0)
		{
			return -1;
		}

		iRet = m_pAllocator->DestroyObject(iID);
		if(iRet < 0)
		{
			return -2;
		}

		return 0;
	}

    //根据RoleID获取缓存角色信息
    static TYPE_Object* GetByRoleID(const RoleID& stRoleID)
    {
        CWorldRoleStatusWObj* pRoleObj = (CWorldRoleStatusWObj*)GetByUin(stRoleID.uin());
        if(!pRoleObj || (pRoleObj->GetRoleID().uiseq()!=stRoleID.uiseq()))
        {
            return NULL;
        }

        return (TYPE_Object*)pRoleObj;
    }

    static TYPE_Object* GetFirstUin() 
    {
        return (TYPE_Object*) m_pAllocator->GetObj(m_pAllocator->GetUsedHead());
    }

    static TYPE_Object* GetNextUin(const int iIdx)
    {
        return (TYPE_Object*) m_pAllocator->GetNextObj(iIdx);
    }

    static TYPE_Object* CreateByHashKey(const unsigned int uiHashKey)
    {
        int iID = 0;
        iID = m_pAllocator->CreateObject();
        if(iID < 0)
        {
            return NULL;
        }

        int iRet = 0;
        iRet = m_pHashMap->InsertValueByKey(uiHashKey, iID);
        if(iRet < 0)
        {
			m_pAllocator->DestroyObject(iID);
            return NULL;
        }

        return (TYPE_Object*)m_pAllocator->GetObj(iID);
    }

    static TYPE_Object* GetByHashKey(const unsigned int uiHashKey)
    {
        int iID = 0;
        int iRet = 0;
        iRet = m_pHashMap->GetValueByKey(uiHashKey, iID);
        if(iRet < 0)
        {
            return NULL;
        }

        return (TYPE_Object*)m_pAllocator->GetObj(iID);
    }

    //删除对象还必须删除Hash
    static int DeleteByHashKey(const unsigned int uiHashKey)
    {
        int iID = 0;
        int iRet = 0;
        iRet = m_pHashMap->DeleteByKey(uiHashKey, iID);
        if(iRet < 0)
        {
            return -1;
        }

        iRet = m_pAllocator->DestroyObject(iID);
        if(iRet < 0)
        {
            return -2;
        }

        return 0;
    }

    static int GetUsedHead()
    {
        return m_pAllocator->GetUsedHead();
    }

    static int GetNextIdx(const int iIdx)
    {
        CIdx *pIdx = m_pAllocator->GetIdx(iIdx);
        if (pIdx)
        {
            return pIdx->GetNextIdx();
        }

        return -1;
    }

    static TYPE_Object* GetByIdx(const int iIdx)
    {
        return (TYPE_Object*) m_pAllocator->GetObj(iIdx);
    }

    static int GetUsedObjNumber()
    {
        return m_pAllocator->GetUsedCount();
    }

public:
	static int RegisterHashAllocator(CObjAllocator* pAllocator, CHashMap_K32* pHashMap)
	{
		if(!pAllocator || !pHashMap)
		{
			return -1;
		}
		m_pAllocator = pAllocator;
		m_pHashMap = pHashMap;
		return 0;
	}

private:
	static CObjAllocator* m_pAllocator;

	static CHashMap_K32* m_pHashMap;

};

template <class TYPE_Object>
CObjAllocator* WorldTypeK32<TYPE_Object>::m_pAllocator = 0;

template <class TYPE_Object>
CHashMap_K32* WorldTypeK32<TYPE_Object>::m_pHashMap = 0;


#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
