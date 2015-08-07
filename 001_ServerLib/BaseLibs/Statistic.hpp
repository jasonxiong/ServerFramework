/**
*@file Statistic
*@author jasonxiong
*@date 2009-11-20
*@version 1.0
*@brief 基础统计类
*
*
*/

#ifndef __STATISTIC_HPP__
#define __STATISTIC_HPP__

#include <time.h>

#include "ErrorDef.hpp"
#include "LogFile.hpp"

namespace ServerLib
{
const int MAX_STAT_ITEM_NAME_LENGTH = 32; //!<统计项的名字最大长度
const int MAX_STAT_ITEM_NUMBER = 16; //!<每个统计段最多有多少个统计项
const int MAX_STAT_SECTION_NAME_LENGTH = 64; //!<统计段的名字最大长度
const int MAX_STAT_SECTION_STRING_LENGTH = 1024; //!<每一个统计段打印到文件中每一行的最长长度

//!统计项,一个数据表示一个统计项
typedef struct tagStatItem
{
    char m_szName[MAX_STAT_ITEM_NAME_LENGTH]; //!<统计项名
    double m_dValue; //!<统计值
} TStatItem;

typedef enum enmStatSectionFlag
{
    ESSF_NOT_PRINT = 0x0001, //!<不打印统计
} ENMSTATSECTIONFLAG;

//!统计段，一个统计段有多个统计项，同一个统计段的数据打印在同一行上
typedef struct tagStatSection
{
    char m_szName[MAX_STAT_SECTION_NAME_LENGTH]; //!<统计段名
    int m_iStatFlag; //!<统计段标志，目前用做是否打印该统计段
    short m_shStatItemNum; //!<统计项个数
    TStatItem m_astStatItem[MAX_STAT_ITEM_NUMBER];
} TStatSection;

class CStatistic
{
public:
    CStatistic();
    ~CStatistic();

public:
    /**
    *初始化统计段
    *@param[in] pszStatName 统计项名
    *@param[in] iMaxSectionNum 需要支付的统计段个数
    *@return 0 success
    */
    int Initialize(const char* pszStatName, int iMaxSectionNum);

    /**
    *初始化统计段（详细配置）
    *@param[in] pszStatName 统计项名
    *@param[in] iMaxSectionNum 需要支付的统计段个数
    *@param[in] rstLogConfig 统计打印配置
    *@return 0 success
    */
    int Initialize(const char* pszStatName, int iMaxSectionNum,
                   const TLogConfig& rstLogConfig);

    /**
    *增加统计段
    *@param[in] pszSectionName 统计段名
    *@param[out] riSectionIdx 获得的统计段索引
    *@return 0 success
    */
    int AddSection(const char* pszSectionName, int& riSectionIdx);

    /**
    *增加统计项
    *@param[in] iSectionIdx 统计段索引
    *@param[in] pszItemName 统计项名
    *@param[out] riItemIdx 统计项索引
    *@return 0 success
    */
    int AddItem(int iSectionIdx, const char* pszItemName, int& riItemIdx);

    /**
    *更新某个统计值
    *@param[in] iSectionIdx 统计段索引
    *@param[in] iItemIdx 统计项索引
    *@param[in] dValue 统计值
    *@return 0 success
    */
    int SetValue(int iSectionIdx, int iItemIdx, double dValue);

    /**
    *增加某个统计值
    *@param[in] iSectionIdx 统计段索引
    *@param[in] iItemIdx 统计项索引
    *@param[in] dPlusValue 要增加的统计值
    *@return 0 success
    */
    int AddValue(int iSectionIdx, int iItemIdx, double dPlusValue);

    /**
    *获取统计段，方便上层可以自由操作
    *@param[in] iSectionIdx 段索引
    *@return 0 success
    */
    TStatSection* GetSection(int iSectionIdx);

    int GetSectionNum() const
    {
        return m_iCurSectionNum;
    }

    //!统计复位
    void Reset();

    //!打印统计
    void Print();

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

    //!粗略判断双浮点数是否是整数，测试过小数小于0.0000001时无法精确
    bool IsDoubleInt(double dValue);

private:
    char m_szStatName[MAX_STAT_SECTION_NAME_LENGTH];
    int m_iMaxSectionNum; //!<支持的统计段个数
    int m_iCurSectionNum; //!<当前统计段个数
    TStatSection* m_astStatSection;
    int m_iErrorNO; //!错误码

    CLogFile m_stLogFile;
    time_t m_tLastReset; //!<最后一次复位统计的时间


};

}

#endif //__STATISTIC_HPP__
///:~
