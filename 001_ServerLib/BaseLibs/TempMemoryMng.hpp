/**
*@file TempMemoryMng.hpp
*@author jasonxiong
*@date 2009-11-28
*@version 1.0
*@brief 临时内存对象管理类
*
*	注意：
*	（1）不支持多进程或多线程
*	（2）创建内存后，在不用的时候需要调用DestroyTempMem来回收内存
*/

#ifndef __TEMP_MEMORY_MNG_HPP__
#define __TEMP_MEMORY_MNG_HPP__

#include <stdio.h>

namespace ServerLib
{

typedef enum enmTempMemAllocType
{
    ETMT_ALLOC_BY_MEMORY_MNG = 0, //!<由CTempMemoryMng自行分配
    ETMT_ALLOC_BY_SHARED_MEMEORY = 1, //!<由共享内存创建
} ENMTEMPMEMALLOCTYPE;

typedef struct tagTempMemoryObj
{
    int m_iNexIndex; //!<下一块临时内存的数组索引
    unsigned int m_uiMemOffset; //!<在整块临时内存中的偏移量
} TTempMemoryObj;

class CTempMemoryMng
{
private:
    CTempMemoryMng();
    //void* operator new(unsigned int uiSize, const void* pThis) throw();

public:
    /**
    *创建临时内存管理器
    *@param[in] iObjSize 每个临时对象块大小
    *@param[in] iMaxObjCount 最大的临时对象块个数
    *@return 0 success
    */
    CTempMemoryMng(int iObjSize, int iMaxObjCount);
    ~CTempMemoryMng();

    /**
    *通过共享内存来创建CTempMemoryMng（注意这样创建的CTempMemoryMng不会初始化，不会改变所在共享内存的值）
    *@param[in] pszKeyFileName 共享内存Attach的文件名
    *@param[in] ucKeyPrjID 共享内存的ProjectID
    *@param[in] iObjSize 每个临时对象块大小
    *@param[in] iMaxObjCount 最大的临时对象块个数
    *@return 0 success
    */
    static CTempMemoryMng* CreateBySharedMemory(const char* pszKeyFileName,
            const unsigned char ucKeyPrjID,
            int iObjSize, int iMaxObjCount);

public:
    /**
    *初始化临时内存管理器
    *@return 0 success
    */
    int Initialize();

    //!创建临时内存
    void* CreateTempMem();

    //!销毁临时内存
    int DestroyTempMem(void *pTempMem);

    //!获取已用对象个数
    int	GetUsedCount() const;

    //!获取空闲对象个数
    int	GetFreeCount() const;

    //!在接口返回错误时，调用这个函数获取错误号
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    //!设置错误号
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    int m_iErrorNO; //!错误码
    int m_iMemAllocType; //!<内存分配类型，0由CTempMemoryMng分配，1由使用者指定
    unsigned char* m_aucObjMem; //!<分配的对象内存
    int m_iObjSize; //!<每个对象块的内存大小
    int m_iMaxObjCount; //!<分配多少个对象块
    int m_iUsedCount; //!<已经使用的内存块个数
    int m_iFirstFreeObjIdx; //!<空闲临时内存队首对象索引
    TTempMemoryObj* m_astTmpMemObj; //!<临时内存对象数组
};

}

#endif //__TEMP_MEMORY_MNG_HPP__
///:~
