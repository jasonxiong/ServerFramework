/**
*@file UnixTime.hpp
*@author jasonxiong
*@date 2009-11-23
*@version 1.0
*@brief 对time_t进行封装管理的类
*
*	使用RefreshTime来刷新时间
*	调用各个GetXXX函数来获取需要的时间格式
*/

#ifndef __UNIX_TIME_HPP__
#define __UNIX_TIME_HPP__

#include <time.h>

#include "SingletonTemplate.hpp"

namespace ServerLib
{

class CUnixTime
{
public:
    CUnixTime();
    CUnixTime(time_t tTime);
    ~CUnixTime();

public:
    //!将当前类中的时间用"%Y-%m-%d %H-%M-%S"表示
    int GetTimeString(char* szTimeString);
    //!将当前类中的时间用"%Y-%m-%d"表示
    int GetDateString(char* szDateString);
    //!将当前类中的时间用"%Y-%m"表示
    int GetMonthString(char* szMonthString);
    //!将当前类中的时间用某一年之后的某一天
    int GetDaysAfterYear(int iYear, unsigned short& rushDays);
    //!通过当前类中的时间获取年份
    int GetYear(int& iYear);
    //!直接返回年份，失败返回-1
    int GetYear();
    //!通过当前类中的时间获取月份
    int GetMonth(int& iMonth);
    //!直接返回月份，失败返回-1
    int GetMonth();
    //!通过当前类中的时间获取日期
    int GetDate(int& iDate);
    //!直接返回日期，失败返回-1
    int GetDate();
    //!通过当前类中的时间获取小时
    int GetHour(int& riHour);
    //!直接返回小时，失败返回-1
    int GetHour();
    //!通过当前类中的时间获取分钟
    int GetMinute(int& riMinute);
    //!直接返回分钟，失败返回-1
    int GetMinute();

    void SetTime(time_t tTime)
    {
        m_tTime = tTime;
    }
    time_t GetTime() const
    {
        return m_tTime;
    }

    //!刷新Unix时间为当前时间
    void RefreshTime();

private:
    time_t m_tTime; //!<UnixTime，主要用于时间表示
};

}

#endif //__UNIX_TIME_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
