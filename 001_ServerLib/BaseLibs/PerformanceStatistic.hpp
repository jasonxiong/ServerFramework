/**
*@file PerformanceStatistic.hpp
*@author jasonxiong
*@date 2009-12-21
*@version 1.0
*@brief 性能统计类
*
*
*/

#ifndef __PERFORMANCE_STATISTIC_HPP__
#define __PERFORMANCE_STATISTIC_HPP__

#include "Statistic.hpp"
#include "SingletonTemplate.hpp"

namespace ServerLib
{

const int MAX_PERFORMANCE_STAT_OP_NUMBER = 128; //!<最多统计的OP操作个数

/**
*@brief OP操作需要统计的统计项索引
*
*
*/
typedef enum enmStatOPItemIndex
{
    ESOI_OP_CALL_TIMES = 0, //!<OP调用次数
    ESOI_SUM_PROCESS_TIME = 1, //!<OP总处理时长
    ESOI_AVA_PROCESS_TIME = 2, //!<OP平均处理时长
    ESOI_MAX_PROCESS_TIME = 3, //!<OP最长处理时长
    ESOI_MAX_ITEM_NUMBER //!<总共需要统计的统计项个数，保证这个值不会超过Section能容纳的Item最大值
} ENMSTATOPITEMINDEX;

extern const char* g_apszOPItemName[ESOI_MAX_ITEM_NUMBER];

class CPerformanceStatistic
{
public:
    CPerformanceStatistic();
    ~CPerformanceStatistic();

public:
    /**
    *初始化，在初始化时会分配内存给CStatistic类中的Section
    *@param[in] pszStatPath 统计文件路径，默认是../stat/
    *@param[in] pszStatFileName 统计文件名，默认是s
    *@return 0 success
    */
    int Initialize(const char* pszStatPath = NULL, const char* pszStatFileName = NULL);

    /**
    *添加操作统计信息
    *@param[in] pszOPName 操作名字（一般是函数名）
    *@param[in] tvProcessTime 调用耗时
    *@return 0 success
    */
    int AddOPStat(const char* pszOPName, timeval tvProcessTime);

    //!在接口返回错误时，调用这个函数获取错误号
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

    void Print();
    void Reset();

private:
    //!设置错误号
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    CStatistic m_stStatistic;
    int m_iErrorNO; //!错误码
};

//!一般来说只会用到一个CPerformanceStatistic类，所以实现一个单件方便使用
typedef CSingleton<CPerformanceStatistic> PerformanceStatisticSingleton;

}

#endif //__PERFORMANCE_STATISTIC_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
