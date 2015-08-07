/**
*@file TimeValue.hpp
*@author jasonxiong
*@date 2009-11-24
*@version 1.0
*@brief 用于方便计算timeval值的类
*
*	直接使用数学运算符来对时间进行计算
*/

#ifndef __TIME_VALUE_HPP__
#define __TIME_VALUE_HPP__

#include <time.h>
#include <sys/types.h>

namespace ServerLib
{

class CTimeValue
{
public:
    CTimeValue();
    CTimeValue(timeval tvTimeVal);
    CTimeValue(int iSec, int iUsec);
    ~CTimeValue();

public:
    //!以下重载的运算符用于timeval的计算
    CTimeValue& operator=(const CTimeValue& rstTime);
    CTimeValue operator+(const CTimeValue& rstTime);
    CTimeValue operator-(const CTimeValue& rstTime);
    int operator>(const CTimeValue& rstTime);
    int operator<(const CTimeValue& rstTime);
    int operator>=(const CTimeValue& rstTime);
    int operator<=(const CTimeValue& rstTime);
    int operator==(const CTimeValue& rstTime);
    int operator!=(const CTimeValue& rstTime);

    void SetTimeValue(timeval stTimeValue)
    {
        m_stTimeval = stTimeValue;
    }
    timeval GetTimeValue() const
    {
        return m_stTimeval;
    }

    int64_t GetMilliSec() const
    {
        return (int64_t)m_stTimeval.tv_sec * 1000 + m_stTimeval.tv_usec / 1000;
    }
    void RefreshTime();


private:
    timeval m_stTimeval; //!<精确时刻，主要用于计算时间差

};

}


#endif //__TIME_VALUE_HPP__
///:~
