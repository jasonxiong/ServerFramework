/**
*@file BiDirQueue.hpp
*@author jasonxiong
*@date 2009-12-14
*@version 1.0
*@brief 双向队列
*
*
*/

#ifndef __BI_DIR_QUEUE_HPP__
#define __BI_DIR_QUEUE_HPP__

namespace ServerLib
{

typedef enum enmItemUseFlag
{
    EINF_NOT_USED = 0, //!<Item节点未使用
    EINF_USED = 1, //!<Item节点已使用
} ENMITEMUSEFLAG;

typedef struct tagBiDirQueueItem
{
    int m_iNextItem; //!<下一个Item的索引
    int m_iPreItem; //!<上一个Item的索引
    char m_cUseFlag; //!<节点是否使用
} TBiDirQueueItem;

typedef enum enmBiDirQueueAllocType
{
    EBDT_ALLOC_BY_SELF = 0, //!<通过自己动态分配内存
    EBDT_ALLOC_BY_SHARED_MEMORY = 1, //!<通过共享内存分配内存
} ENMBIDIRQUEUEALLOCTYPE;

class CBiDirQueue
{
public:
    static CBiDirQueue* CreateByGivenMemory(char* pszMemoryAddress,
                                            const size_t uiFreeMemorySize,
                                            const int iNodeNumber);

    static size_t CountSize(const int iNodeNumber);

private:
    CBiDirQueue();

public:
    CBiDirQueue(int iMaxItemCount);
    ~CBiDirQueue();

    /**
    *通过共享内存来创建CBiDirQueue（注意这样创建的CBiDirQueue不会初始化以支持恢复模式，不会改变所在共享内存的值）
    *@param[in] pszKeyFileName 共享内存Attach的文件名
    *@param[in] ucKeyPrjID 共享内存的ProjectID
    *@param[in] iMaxItemCount 队列中最大的Item个数
    *@return 0 success
    */
    static CBiDirQueue* CreateBySharedMemory(const char* pszKeyFileName,
            const unsigned char ucKeyPrjID,
            int iMaxItemCount);

    static CBiDirQueue* ResumeByGivenMemory(char* pszMemoryAddress,
                                            const size_t uiFreeMemorySize,
                                            const int iNodeNumber);

public:
    //!初始化，清空队列
    int Initialize();

    //!获取队列长度
    int GetQueueItemCount() const
    {
        return m_iCurItemCount;
    }

    //!获取队列头部索引
    int	GetHeadItem() const
    {
        return m_iHeadIdx;
    }

    //!获取队列尾部索引
    int GetTailItem() const
    {
        return m_iTailIdx;
    }

    //!删除指定Item
    int	DeleteItem(int iItemIdx);

    //!将一个Item添加到末尾
    int	AppendItemToTail(int iItemIdx);

    //!将一个Item添加到队首
    int	InsertItemToHead(int iItemIdx);

    //!获取队列头部索引，并将其推出队列
    int	PopHeadItem();

    //!获取队列尾部索引，并将其推出队列
    int PopTailItem();

    //!获取指定Item的下一个Item索引，成功返回0
    int	GetNextItem(int iItemIdx, int& iNextItemIdx);

    //!获取指定Item的上一个Item索引，成功返回0
    int	GetPrevItem(int iItemIdx, int& iPrevItemIdx);

    //!将一个Item添加另一个Item后面
    int	InsertItemAfter( int iItemIdx, int iPrevItemIdx);

    //!将一个Item添加另一个Item前面
    int	InsertItemBefore(int iItemIdx, int iNextItemIdx);

public:
    //!在接口返回错误时，调用这个函数获取错误号
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    int	SetNextItem(int iItemIdx, int iNextItemIdx);
    int	SetPrevItem(int iItemIdx, int iPrevItemIdx);

    //!设置错误号
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    int m_iErrorNO; //!错误码
    int m_iCurItemCount; //!<队列中当前的Item个数
    int	m_iHeadIdx; //!<队列头部索引
    int	m_iTailIdx; //!<队列尾部索引
    short m_shQueueAllocType; //!<队列分配类型
    int m_iMaxItemCount; //!<队列中最大的Item个数
    TBiDirQueueItem* m_astQueueItems; //!<Item队列
};

}

#endif //__BI_DIR_QUEUE_HPP__
///:~
