/**
*@file CachStatistic.hpp
*@author jasonxiong
*@date 2009-11-21
*@version 1.0
*@brief 缓冲区状态统计类
*
*	使用这个类来统计缓冲区的状态信息，包括可用缓存、已用缓存，命中率等
*/

#ifndef __CACHE_STATISTIC_HPP__
#define __CACHE_STATISTIC_HPP__

#include "Statistic.hpp"
#include "SingletonTemplate.hpp"

namespace ServerLib
{

const int MAX_CACHE_TYPE_NUMBER = 32; //!<最大支持的CacheID个数，就是能统计的Cache种类个数

extern const char* DEFAULT_CACHE_STAT_DIRECTORY_PATH; //!<Cache统计文件所存放文件夹
extern const char* DEFAULT_CACHE_STAT_FILE_NAME; //!<Cache统计文件名

typedef enum enmCacheStatusItemIndex
{
    ECSI_USED_COUNT = 0, //!<Cache使用数
    ECSI_FREE_COUNT, //!<Cache空闲数
    ECSI_RECYCLE_COUNT, //!<Cache在回收队列中的个数
    ECSI_SWAP_TIMES, //!<Cache被交换的次数
    ECSI_MAX_ITEM_NUMBER //!<总共需要统计的统计项个数，保证这个值不会超过Section能容纳的Item最大值
} ENMCACHESTATUSITEMINDEX;

extern const char* g_apszCacheStatusItemName[ECSI_MAX_ITEM_NUMBER];

typedef enum enmReadCacheItemIndex
{
    ERCI_HIT_CACHE_COUNT = 0, //!<读Cache命中次数
    ERCI_MISS_CACHE_COUNT, //!<读Cache未命中次数
    ERCI_HIT_RATE, //!<读Cache命中率
    ERCI_MAX_ITEM_NUMBER //!<总共需要统计的统计项个数，保证这个值不会超过Section能容纳的Item最大值
} ENMREADCACHEITEMINDEX;

extern const char* g_apszReadCacheItemName[ERCI_MAX_ITEM_NUMBER];

typedef enum enmWriteCacheItemIndex
{
    EWCI_HIT_CACHE_COUNT = 0, //!<写Cache命中次数
    EWCI_MISS_CACHE_COUNT, //!<写Cache未命中次数
    EWCI_HIT_RATE, //!<写Cache命中率
    EWCI_MAX_ITEM_NUMBER //!<总共需要统计的统计项个数，保证这个值不会超过Section能容纳的Item最大值
} ENMWRITECACHEITEMINDEX;

extern const char* g_apszWriteCacheItemName[EWCI_MAX_ITEM_NUMBER];

//!用于通过CacheID方便定位到在CStatistic中的Section索引
typedef struct tagStatCacheInfo
{
    int m_iCacheType; //!<Cache类型，每一种Cache必须保证这个值唯一
    int m_iCacheStatusIndex; //!<CacheStatus在CStaitistic中的Section索引
    int m_iReadCacheIndex; //!<ReadCache在CStatistic中的Section索引
    int m_iWriteCacheIndex; //!<WriteCache在CStatistic中的Section索引
} TStatCacheInfo;

class CCacheStatistic
{
public:
    CCacheStatistic();
    ~CCacheStatistic();

public:
    /**
    *初始化，在初始化时会分配内存给CStatistic类中的Section
    *@param[in] pszStatPath 统计文件路径，默认是../stat/
    *@param[in] pszStatFileName 统计文件名，默认是s
    *@return 0 success
    */
    int Initialize(const char* pszStatPath = NULL, const char* pszStatFileName = NULL);

    /**
    *增加Cache类型做统计，在需要某一个Cache前要调用这个函数将这个Cache的类型添加上去
    *@param[in] iCacheType Cache类型，每一种Cache必须保证这个值唯一，后面所有对Cache的统计需要指定这个ID
    *@param[in] pszCacheName Cache名
    *@return 0 success
    */
    int AddCacheType(int iCacheType, const char* pszCacheName);

    /**
    *增加Cache统计信息
    *@param[in] iCacheType Cache类型，通过这个值定位到Section索引
    *@param[in] iUsedCount Cache已用个数
    *@param[in] iFreeCount Cache空闲个数
    *@param[in] iRecycleCount Cache在回收队列中的个数
    *@param[in] iSwapTimes Cache被交换的次数
    *@return 0 success
    */
    int UpdateCacheStatus(int iCacheType, int iUsedCount, int iFreeCount,
                          int iRecycleCount, int iSwapTimes);

    /**
    *增加读Cache时的统计信息
    *@param[in] iCacheType Cache类型，通过这个值定位到Section索引
    *@param[in] iAddHitCount 需要增加的读命中次数
    *@param[in] iAddMissCount 需要增加的读丢失次数
    *@return 0 success
    */
    int AddReadCacheStat(int iCacheType, int iAddHitCount, int iAddMissCount);

    /**
    *增加写Cache时的统计信息
    *@param[in] iCacheType Cache类型，通过这个值定位到Section索引
    *@param[in] iAddHitCount 需要增加的写命中次数
    *@param[in] iAddMissCount 需要增加的写丢失次数
    *@return 0 success
    */
    int AddWriteCacheStat(int iCacheType, int iAddHitCount, int iAddMissCount);

    //!打印统计信息
    void Print();

    //!清空统计信息
    void Reset();

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

    //!增加CacheStatus统计段
    int AddCacheStatusSection(const char* pszCacheName, int& riSectionIdx);

    //!增加ReadCache统计段
    int AddReachCacheSection(const char* pszCacheName, int& riSectionIdx);

    //!增加WriteCache统计段
    int AddWriteCacheSection(const char* pszCacheName, int& riSectionIdx);

    //!根据Cache类型获取指定的Cache信息结构
    TStatCacheInfo* GetCacheInfo(int iCacheType);

private:
    CStatistic m_stStatistic;
    int m_iErrorNO; //!错误码

    short m_shCacheTypeNum;
    TStatCacheInfo m_astCacheInfo[MAX_CACHE_TYPE_NUMBER];

};

//!一般来说只会用到一个CMsgStatistic类，所以实现一个单件方便使用
typedef CSingleton<CCacheStatistic> CacheStatisticSingleton;

}

#endif //__CACHE_STATISTIC_HPP__
///:~
