#ifndef __APP_ALLOCATOR_HPP__
#define __APP_ALLOCATOR_HPP__

#include "ObjAllocator.hpp"
#include "HashMap_K32.hpp"
#include "HashMap_K64.hpp"
#include "GameConfigDefine.hpp"
#include "LogAdapter.hpp"

using namespace ServerLib;

//////////////////////////////////////////////////////////////////////////

//对象操作器
template <class TYPE_Object>
class GameType
{
public:
    static int GetUsedObjNumber()
    {
        return m_pAllocator->GetUsedCount();
    }

    static int GetFreeObjNumber()
    {
        return m_pAllocator->GetFreeCount();
    }

	static int Create()
	{
		int iIdx = m_pAllocator->CreateObject();

        return iIdx;
	}

    static int CreateByIdx(const int iIdx)
    {
        return m_pAllocator->CreateObjectByID(iIdx);
    }

	static TYPE_Object* Get(const int iIdx)
	{
		return (TYPE_Object*) m_pAllocator->GetObj(iIdx);
	}

    static TYPE_Object* GetFirst() 
    {
        return (TYPE_Object*) m_pAllocator->GetObj(m_pAllocator->GetUsedHead());
    }

    static TYPE_Object* GetNext(const int iIdx)
    {
        return (TYPE_Object*) m_pAllocator->GetNextObj(iIdx);
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

	static int Del(const int iIdx)
	{
        LOGDEBUG("Delete: iIdx = %d\n", iIdx);
		return m_pAllocator->DestroyObject(iIdx);
	}

public:
	static int RegisterAllocator(CObjAllocator* pAllocator)
	{
		if (!pAllocator)
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
CObjAllocator* GameType<TYPE_Object>::m_pAllocator = 0;

//////////////////////////////////////////////////////////////////////////

//对象操作器
template <class TYPE_Object>
class GameTypeK32
{
public:
    static int GetUsedObjNumber()
    {
        return m_pAllocator->GetUsedCount();
    }

    static int GetFreeObjNumber()
    {
        return m_pAllocator->GetFreeCount();
    }

	static TYPE_Object* CreateByKey(const unsigned int uiKey)
	{
		int iIdx = m_pAllocator->CreateObject();
		if (iIdx < 0)
		{
			return NULL;
		}

		int iRet = m_pHashMap->InsertValueByKey(uiKey, iIdx);
		if (iRet < 0)
		{
			m_pAllocator->DestroyObject(iIdx);
			return NULL;
		}

		return (TYPE_Object*) m_pAllocator->GetObj(iIdx);
	}

	static TYPE_Object* GetByKey(const unsigned int uiKey)
	{
		int iIdx = 0;
		int iRet = 0;

		iRet = m_pHashMap->GetValueByKey(uiKey, iIdx);
		if (iRet < 0)
		{
			return NULL;
		}

		return (TYPE_Object*) m_pAllocator->GetObj(iIdx);
	}

    static TYPE_Object* GetByIdx(const int iIdx)
    {
        return (TYPE_Object*) m_pAllocator->GetObj(iIdx);
    }

    static TYPE_Object* GetNextByIdx(const int iIdx)
    {
        return (TYPE_Object*) m_pAllocator->GetNextObj(iIdx);
    }

	//删除对象还必须删除Hash
	static int DeleteByKey(const unsigned int uiKey)
	{
		int iIdx = 0;
		int iRet = 0;

		iRet = m_pHashMap->DeleteByKey(uiKey, iIdx);
		if (iRet < 0)
		{
			return -1;
		}

		iRet = m_pAllocator->DestroyObject(iIdx);
		if (iRet < 0)
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

public:
	static int RegisterHashAllocator(CObjAllocator* pAllocator, CHashMap_K32* pHashMap)
	{
		if (!pAllocator || !pHashMap)
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
CObjAllocator* GameTypeK32<TYPE_Object>::m_pAllocator = 0;

template <class TYPE_Object>
CHashMap_K32* GameTypeK32<TYPE_Object>::m_pHashMap = 0;

//////////////////////////////////////////////////////////////////////////

template <class TYPE_Object>
class GameTypeK64
{
public:
    static int GetUsedObjNumber()
    {
        return m_pAllocator->GetUsedCount();
    }

	static TYPE_Object* CreateByKey(uint64_t ddwKey)
	{
        int iObjID = 0;
        iObjID = m_pAllocator->CreateObject();
        if(iObjID < 0)
        {
            return NULL;
        }

        TData64 key64;
		memcpy(&key64, &ddwKey, sizeof(uint64_t));

        int iRet = m_pHashMap->InsertValueByKey(key64, iObjID);
        if(iRet < 0)
        {
			m_pAllocator->DestroyObject(iObjID);
            return NULL;
        }

        return (TYPE_Object*)m_pAllocator->GetObj(iObjID);
	}

    static TYPE_Object* GetByKey(uint64_t ddwKey)
    {
        int iID = 0;
        TData64 key64;
		memcpy(&key64, &ddwKey, sizeof(uint64_t));
        int iRet = 0;
        iRet = m_pHashMap->GetValueByKey(key64, iID);
        if(iRet < 0)
        {
            return NULL;
        }

        return (TYPE_Object*)m_pAllocator->GetObj(iID);
    }

    static TYPE_Object* GetByID(int iID)
    {
        return (TYPE_Object*)m_pAllocator->GetObj(iID);
    }

    static int DeleteByKey(uint64_t ddwKey)
    {
        TData64 key64;
		memcpy(&key64, &ddwKey, sizeof(uint64_t));
        int iID = 0;
        int iRet = 0;
        iRet = m_pHashMap->DeleteByKey(key64, iID);
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
    static TYPE_Object* GetByIdx(const int iIdx)
    {
        return (TYPE_Object*) m_pAllocator->GetObj(iIdx);
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

public:
    static int RegisterHashAllocator(CObjAllocator* pAllocator, CHashMap_K64* pHashMap)
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

    static CHashMap_K64* m_pHashMap;
};

template <class TYPE_Object>
CObjAllocator* GameTypeK64<TYPE_Object>::m_pAllocator = 0;

template <class TYPE_Object>
CHashMap_K64* GameTypeK64<TYPE_Object>::m_pHashMap = 0;

//////////////////////////////////////////////////////////////////////////

#endif

