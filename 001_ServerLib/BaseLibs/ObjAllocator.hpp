/**
*@file ObjAllocator
*@author jasonxiong
*@date 2009-12-09
*@version 1.0
*@brief CObj对象分配类，即新版本的idxobj
*
*	（1）一般用于大型对象的内存分配
*/

#ifndef __OBJ_ALLOCATOR_HPP__
#define __OBJ_ALLOCATOR_HPP__

#include <stdio.h>

namespace ServerLib
{

typedef enum enmObjAllocType
{
    EOAT_ALLOC_BY_SELF = 0, //!<对象内存由ObjAllocator自已动态分配
    EOAT_ALLOC_BY_SHARED_MEMORY = 1, //!<对象内存由共享内存分配
} ENMOBJALLOCTYPE;

class CObj;

typedef enum enmIdxUseFlag
{
    EIUF_FREE = 0, //!<该对象未被使用
    EIUF_USED = 1, //!<该对象已被使用
} ENMIDXUSEFLAG;

//!索引类，仅在CObjAllocator中使用，外层一般不用
class CIdx
{
public:
    CIdx();
    ~CIdx();

public:
    //!初始化函数
    int Initialize();

    //!将对象设置为未使用
    inline void SetFree()
    {
        m_iUseFlag = EIUF_FREE;
    }

    //!将对象设置为已使用
    inline void SetUsed()
    {
        m_iUseFlag = EIUF_USED;
    }

    //!判断对象是否已被使用
    inline int	IsUsed() const
    {
        return m_iUseFlag == EIUF_USED;
    }

    //!获取所在链表下一个索引
    inline int	GetNextIdx() const
    {
        return m_iNextIdx;
    }

    //!设置所在链表下一个索引
    inline void SetNextIdx(int iIdx)
    {
        m_iNextIdx = iIdx;
    }

    //!获取所在链表上一个索引
    inline int	GetPrevIdx() const
    {
        return m_iPrevIdx;
    }

    //!设置所在链表上一个索引
    inline void SetPrevIdx(int iIdx)
    {
        m_iPrevIdx = iIdx;
    }

    //!获取指向的对象
    inline CObj* GetAttachedObj() const
    {
        return m_pAttachedObj;
    }

    //!设置指向的对象
    inline void SetAttachedObj(CObj *pObj)
    {
        m_pAttachedObj = pObj;
    }

private:
    int	m_iNextIdx;	//!<对象索引块链表指针，指向后一个闲/忙索引
    int	m_iPrevIdx;	//!<对象索引块链表指针，指向前一个闲/忙索引
    int	m_iUseFlag; //!<该对象是否已经被使用标志
    CObj *m_pAttachedObj; //!<所指向的对象指针
};

typedef CObj* (*Function_CreateObj)(void *);

class CObjAllocator
{
private:
    //默认构造函数，不允许上层自行调用
    CObjAllocator();

public:
    CObjAllocator(size_t uiObjSize, int iObjCount, Function_CreateObj pfCreateObj);
    ~CObjAllocator();

    /**
    *使用共享内存创建ObjAllocator
    *@param[in] pszKeyFileName 共享内存Attach的文件名
    *@param[in] ucKeyPrjID 共享内存的工程ID
    *@param[in] uiObjSize 对象大小
    *@param[in] iObjCount 对象个数
    *@param[in]
    *@return 0 success
    */
    static CObjAllocator* CreateBySharedMemory(const char* pszKeyFileName,
            const unsigned char ucKeyPrjID,
            size_t uiObjSize, int iObjCount, Function_CreateObj pfCreateObj);

    /**
    *指定内存指针来创建CObjAllocator
    *@param[in] pszMemoryAddress 指定的内存
    *@param[in] uiMemorySize 内存大小
    *@param[in] uiObjSize 对象大小
    *@param[in] iObjCount 对象个数
    *@param[in] pfCreateObj 创建CObj对象的函数，默认可以用DECLARE_DYN中的CreateObject
    *@return 0 success
    */
    static CObjAllocator* CreateByGivenMemory(char* pszMemoryAddress, size_t uiMemorySize,
            size_t uiObjSize, int iObjCount, Function_CreateObj pfCreateObj);

    static size_t CountSize(size_t uiObjSize, int iObjCount);

    static CObjAllocator* ResumeByGivenMemory(char* pszMemoryAddress,
            size_t uiMemorySize, size_t uiObjSize, int iObjCount, Function_CreateObj pfCreateObj);

public:
    //!初始化函数，将数据清空
    int Initialize();

    //!创建一个对象，成功返回对象ID，失败返回小于0的值
    int	CreateObject();

    //!创建一个对象, 并指定其ID，成功返回对象ID，失败返回小于0的值
    int	CreateObjectByID(int iID);

    //!根据对象ID销毁一个对象，成功返回0
    int	DestroyObject(int iObjectID);

    //!根据ObjectID返回对象，必须保证该对象已被使用
    CObj* GetObj(int iObjectID);

    //!根据ObjectID返回索引
    CIdx* GetIdx(int iObjectID);

    //!获取已用对象链表头索引
    inline int	GetUsedHead() const
    {
        return m_iUsedHeadIdx;
    }

    //!获取空闲对象链表头索引
    inline int	GetFreeHead() const
    {
        return m_iFreeHeadIdx;
    }

    //获取已用对象个数
    inline int	GetUsedCount() const
    {
        return m_iUsedCount;
    }

    //获取空闲对象个数
    inline int	GetFreeCount() const
    {
        return m_iObjCount - m_iUsedCount;
    }

    //!在接口返回错误时，调用这个函数获取错误号
    inline int GetErrorNO() const
    {
        return m_iErrorNO;
    }

    //获取对象分配类型
    inline int GetObjAllocType() const
    {
        return m_shObjAllocType;
    }

    //!获取下一个对象
    CObj* GetNextObj(int iObjectID);

    //!设置对象初始化指针
    inline void SetCreateObjFunc(Function_CreateObj pfCreateObjFunc)
    {
        m_pfCreateObjFunc = pfCreateObjFunc;
    }

private:
    //!设置错误号
    inline void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    //这几个对象只有在构造函数时确定，后面不会更改
    short m_shObjAllocType; //!<对象分配类型
    size_t m_uiObjSize; //!<单个对象
    int m_iObjCount; //!<最多分配的对象个数
    CIdx* m_astIdxs; //!<索引数组，用于管理对象链表
    CObj* m_pstObjBuffer; //!<分配的对象内存
    Function_CreateObj m_pfCreateObjFunc; //!<在内存上创建CObj对象的函数，每个子类需要自己实现

    //以下的对象会更改，调用Initialize初始化
    int m_iErrorNO; //!<错误码
    int	m_iFreeHeadIdx; //!<空闲对象链表头索引
    int m_iFreeTailIdx; //!<空闲对象链表尾索引
    int	m_iUsedHeadIdx; //!<已用对象链表头索引
    int	m_iUsedCount; //!<已用对象个数
};

//!基本对象类
class CObj
{
public:
    CObj() {}
    virtual ~CObj() {}

public:
    //!对象的初始化函数，在CObjAllocator创建对象时会调用，所以子类一定要实现
    virtual int Initialize() = 0;

    //!显示对象函数，可重载
    virtual int Show() const
    {
        return 0;
    }

    //!获取对象ID
    inline int GetObjectID() const
    {
        return m_iObjectID;
    }

    //!设置对象ID
    inline void SetObjectID(int iObjectID)
    {
        m_iObjectID = iObjectID;
    }

    virtual int Resume()
    {
        return 0;
    }

private:
    int m_iObjectID; //!对象ID，即在CObjAllocator中的数组下标

    friend int CObjAllocator::Initialize(); //!<在这个函数中需要直接赋值m_iObjectID，所以设为友元
};

#define DECLARE_DYN \
	public: \
		void* operator new(size_t uiSize, const void* pThis) throw(); \
		static CObj* CreateObject(void* pMem);


#define IMPLEMENT_DYN(class_name) \
	void* class_name::operator new(size_t uiSize, const void* pThis) throw() \
	{ \
		if(!pThis) \
		{ \
			return NULL; \
		} \
		\
		return (void*)pThis; \
	} \
	\
	CObj* class_name::CreateObject( void *pMem ) \
	{ \
		return (CObj*)new(pMem) class_name; \
	}

}

#endif //__OBJ_ALLOCATOR_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
