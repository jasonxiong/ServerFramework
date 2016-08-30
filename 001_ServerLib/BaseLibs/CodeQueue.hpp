#ifndef __CODEQUEUE_HPP__
#define __CODEQUEUE_HPP__

#include <stdio.h>

namespace ServerLib
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const int QUEUE_RESERVE_LENGTH = 8; //消息队列预留部分的长度，防止首尾相接

typedef enum enmStatusFlag
{
    ESF_NORMAL_STATUS = 0, //!<正常状态
    ESF_DISORDER_STATUS = 1, //!<混乱状态，在这个状态的时候队列可能已经乱序，上层建议不应该再Pop或Push，Pop方设置这个状态，Push清除这个状态
} ENMRESETFLAG;

typedef enum enmQueueAllocType
{
    EQT_NEW_BY_SELF = 0, //!<通过New动态创建的
    EQT_ALLOC_BY_SHARED_MEMEORY = 1, //!<通过共享内存创建的
} ENMQUEUEALLOCTYPE;

class CCodeQueue
{
private:
    CCodeQueue();

public:
    /**
    *用动态分配来创建代码队列，需要指定队列长度
    *@param[in] uiMaxQueueLength 创建的队列长度
    *@return 0 success
    */
    CCodeQueue(unsigned int uiMaxQueueLength);
    ~CCodeQueue();

    /**
    *通过共享内存来创建CodeQueue（注意这样创建的CodeQueue不会初始化，不会改变所在共享内存的值）
    *@param[in] pszKeyFileName 共享内存Attach的文件名
    *@param[in] ucKeyPrjID 共享内存的ProjectID
    *@param[in] uiMaxQueueLength 最大的队列长度
    *@return 0 success
    */
    static CCodeQueue* CreateBySharedMemory(const char* pszKeyFileName,
                                            const unsigned char ucKeyPrjID,
                                            unsigned int uiMaxQueueLength);

public:
    /**
    *初始化编码队列
    *@return 0 success
    */
    int Initialize(bool bResume = false);

    /**
    *插入一个编码
    *@param[in] pucInCode 插入的编码指针
    *@param[in] iCodeLength 插入的编码长度
    *@return 0 success
    */
    int PushOneCode(const unsigned char* pucInCode, int iCodeLength);

    /**
    *取出一个编码拷贝到pucOutCode，向后移动编码队列头指针
    *@param[in] pucOutCode 将取出的编码拷贝到这个缓冲区
    *@param[in] iMaxOutCodeLen 存放编码的缓冲区最大长度
    *#param[out] riCodeLength 取出的编码实际长度
    *@return 0 success
    */
    int PopOneCode(unsigned char* pucOutCode, int iMaxOutCodeLen, int& riCodeLength);

    /**
    *取出一个编码，向后移动编码队列头指针
    *@return 0 success
    */
    int PopOneCode();

    /**
    *获取一个编码的头指针
    *@param[out] rpucOutCode 获取的编码头指针
    *@param[out] riCodeLength 取出的编码实际长度
    *@return 0 success
    */
    int GetOneCode(unsigned char*& rpucOutCode, int& riCodeLength);

    //!获取整个编码队列头指针
    unsigned char* GetHeadCodeQueue() const;

    //!获取整个编码队列尾指针
    unsigned char* GetTailCodeQueue() const;

    /**
    *判断编码队列是否已经满
    *@return true 已满
    */
    bool IsQueueFull();

    // 判断队列是否为空
    bool IsQueueEmpty();

    //!重置队列：清空当前队列的头指针和尾指针，设置为初始状态
    void Reset();

    //!设置状态
    void SetStatus(int iStatus);

    //!获取队列状态
    int GetStatus() const
    {
        return m_iCodeStatus;
    }

    //!在接口返回错误时，调用这个函数获取错误号
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

    /**
    *获取编码队列当前空余长度
    *@return 空余长度
    */
    unsigned int GetFreeLength();

    /**
    *获取编码队列当前已用长度
    *@return 已用长度
    */
    unsigned int GetUsedLength();

private:
    //!设置错误号
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

    //!判断Offset是否合法
    bool IsDataOffsetValid(unsigned int uiDataOffset);

    //!获取当前的数据偏移
    int GetDataOffset(unsigned int& ruiBegOffset, unsigned int& ruiEndOffset);

    //!获取CodeQueue的Buffer指针
    int GetQueueBuf(unsigned char*& rucQueue) const;

    //!设置编码队列开始偏移量
    int SetBeginOffset(unsigned int uiBegOffset);

    //!设置编码队列结束偏移量
    int SetEndOffset(unsigned int uiEndOffset);

    //!在Pop和Push的时候都调用这个函数计算长度，这样做主是因为该类没有锁，在多进程调用时，m_uiBegOffset和m_uiEndOffset随时可能变化
    unsigned int GetUsedLength(unsigned int uiBegOffset, unsigned int uiEndOffset,
                               bool bMidOffsetValid, unsigned int uiMidOffset);

    //!在Pop和Push的时候都调用这个函数计算长度，这样做主是因为该类没有锁，在多进程调用时，m_uiBegOffset和m_uiEndOffset随时可能变化
    unsigned int GetFreeLength(unsigned int uiBegOffset, unsigned int uiEndOffset);

    //!使用MidOffset
    int EnableMidOffset(unsigned int uiMidOffset);

    //!不使用MidOffset
    int DisableMidOffset();

    //!获取MidOffset
    int GetMidOffset(bool& rbMidOffsetValid, unsigned int& ruiMidOffset);

private:
    int m_iErrorNO; //!<错误码
    int m_iCodeStatus; //!<编码队列状态
    short m_shQueueAllocType; //!<编码区分配类型
    unsigned char* m_aucQueue; //!<编码队列缓冲区，仅在使用动态分配时使用
    size_t m_uiQueueOffset; //!<编码队列缓冲区偏移（相对于this指针），在使用共享内存时使用
    unsigned int m_uiMaxQueueLength; //!<编码队列最大长度
    unsigned int m_uiBegOffset; //!<当前编码队列首指针偏移，Pop一个编码的时候，增加这个值
    unsigned int m_uiEndOffset; //!<当前编码队列尾指针偏移，Push一个编码的时候，增加这个值
    bool m_bMidOffsetValid; //!<中段偏移是否有效
    unsigned int m_uiMidOffset; //!<当m_uiEndOffset接近缓冲区尾部时，如果不够存放一个编码，则用m_uiMidOffset记下这时的m_uiEndOffset，m_uiEndOffset从0开始
};
}

#endif



----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
