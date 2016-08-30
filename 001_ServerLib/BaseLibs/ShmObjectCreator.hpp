/**
*@file ShmObjectCreator.hpp
*@author jasonxiong
*@date 2009-11-03
*@version 1.0
*@brief CShmObjectCreator模板类的定义
*
*	对共享内存CSharedMemory的使用简单封装，对一个大小固定的类用共享内存分配
*/

#ifndef __SHM_OBJECT_CREATOR_HPP__
#define __SHM_OBJECT_CREATOR_HPP__

#include <new>

#include "SharedMemory.hpp"



namespace ServerLib
{

template <class TYPE_ShmObject>
class CShmObjectCreator
{
public:

    /**
    *用共享内存创建对象
    *@param[in] pstSharedMemory 使用的共享内存类，最后一块内存用一个类，防止出现内存泄漏
    *@param[in] pszKeyFileName 共享内存关联的文件名
    *@param[in] ucKeyPrjID 共享内存关联的ID
    *@return 0 success
    */
    static TYPE_ShmObject* CreateObject(
        CSharedMemory* pstSharedMemory,
        const char* pszKeyFileName,
        const unsigned char ucKeyPrjID)
    {
        int iRet = 0;
        iRet = pstSharedMemory->CreateShmSegment(pszKeyFileName, ucKeyPrjID, sizeof(TYPE_ShmObject));
        if (iRet < 0)
        {
            printf("CreateShmSegment failed, iRet=%d\n ", iRet);
            return NULL;
        }
        TYPE_ShmObject* pTYPE_ShmObject = NULL;
        pTYPE_ShmObject = new((void*)pstSharedMemory->GetFreeMemoryAddress()) TYPE_ShmObject();
        if(!pTYPE_ShmObject)
        {
            return NULL;
        }
        iRet = pstSharedMemory->UseShmBlock(sizeof(TYPE_ShmObject));
        if(iRet < 0)
        {
            return NULL;
        }
        return pTYPE_ShmObject;
    }

    static TYPE_ShmObject* CreateObjectByKey(
        CSharedMemory* pstSharedMemory,
        key_t uiShmKey)
    {
        int iRet = 0;
        iRet = pstSharedMemory->CreateShmSegmentByKey(uiShmKey, sizeof(TYPE_ShmObject));
        if (iRet < 0)
        {
            printf("CreateShmSegment failed, iRet=%d\n ", iRet);
            return NULL;
        }
        TYPE_ShmObject* pTYPE_ShmObject = NULL;
        pTYPE_ShmObject = new((void*)pstSharedMemory->GetFreeMemoryAddress()) TYPE_ShmObject();
        if(!pTYPE_ShmObject)
        {
            return NULL;
        }
        iRet = pstSharedMemory->UseShmBlock(sizeof(TYPE_ShmObject));
        if(iRet < 0)
        {
            return NULL;
        }
        return pTYPE_ShmObject;
    }

    /**
    *用共享内存创建对象，并额外分配一定大小
    *@param[in] pstSharedMemory 使用的共享内存类，最后一块内存用一个类，防止出现内存泄漏
    *@param[in] pszKeyFileName 共享内存关联的文件名
    *@param[in] ucKeyPrjID 共享内存关联的ID
    *@param[in] iAddistionalSize 额外分配的内存大小
    *@return 0 success
    */
    static TYPE_ShmObject* CreateObject(
        CSharedMemory* pstSharedMemory,
        const char* pszKeyFileName,
        const unsigned char ucKeyPrjID,
        const int iAddistionalSize)
    {
        int iRet = 0;
        int iSharedMemorySize = sizeof(TYPE_ShmObject) + iAddistionalSize;
        pstSharedMemory->CreateShmSegment(pszKeyFileName, ucKeyPrjID, iSharedMemorySize);
        TYPE_ShmObject* pTYPE_ShmObject = NULL;;
        pTYPE_ShmObject = new((void*)pstSharedMemory->GetFreeMemoryAddress()) TYPE_ShmObject(iAddistionalSize);
        if(!pTYPE_ShmObject)
        {
            return NULL;
        }
        iRet = pstSharedMemory->UseShmBlock(iSharedMemorySize);
        if(iRet < 0)
        {
            return NULL;
        }
        return pTYPE_ShmObject;
    }

    static TYPE_ShmObject* CreateObjectByKey(
        CSharedMemory* pstSharedMemory,
        key_t uiShmKey,
        const int iAddistionalSize)
    {
        int iRet = 0;
        int iSharedMemorySize = sizeof(TYPE_ShmObject) + iAddistionalSize;
        pstSharedMemory->CreateShmSegmentByKey(uiShmKey, iSharedMemorySize);
        TYPE_ShmObject* pTYPE_ShmObject = NULL;;
        pTYPE_ShmObject = new((void*)pstSharedMemory->GetFreeMemoryAddress()) TYPE_ShmObject(iAddistionalSize);
        if(!pTYPE_ShmObject)
        {
            return NULL;
        }
        iRet = pstSharedMemory->UseShmBlock(iSharedMemorySize);
        if(iRet < 0)
        {
            return NULL;
        }
        return pTYPE_ShmObject;
    }

};



//////////////////////////////////////////////////////////////////////////

template <class TYPE>
class CShmSingleton
{
public:
    static TYPE* Instance(void)
    {
        if(m_pstInstance == NULL)
        {
            //TBD
            if(m_pszMemory == NULL)
            {
                m_pstInstance = new TYPE;
            }
            else
            {
                m_pstInstance = new(m_pszMemory) TYPE;
            }
        }
        return m_pstInstance;
    }

    static TYPE* CreateObject(
        CSharedMemory* pstSharedMemory,
        const char* pszKeyFileName,
        const unsigned char ucKeyPrjID)
    {
        int iRet = 0;
        pstSharedMemory->CreateShmSegment(pszKeyFileName, ucKeyPrjID, sizeof(TYPE));
        m_pstInstance = new((void*)pstSharedMemory->GetFreeMemoryAddress()) TYPE;
        if(!m_pstInstance)
        {
            return NULL;
        }
        iRet = pstSharedMemory->UseShmBlock(sizeof(TYPE));
        if(iRet < 0)
        {
            return NULL;
        }
        return m_pstInstance;
    }
protected:
    CShmSingleton()
    {

    }
protected:
    static TYPE* m_pstInstance;
    static char* m_pszMemory;

};

template <class TYPE>
TYPE* CShmSingleton<TYPE>::m_pstInstance = NULL;

template <class TYPE>
char* CShmSingleton<TYPE>::m_pszMemory = NULL;

}

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
