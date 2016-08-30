/**
*@file NowTIme.hpp
*@author jasonxiong
*@date 2009-11-23
*@version 1.0
*@brief 当前时间的定义
*
*	默认情况下不使用“假时间”，调用GetNowTime即调用了time(NULL)获取时间
*	在使用“假时间”后，需要应用自行调用RefreshNowTime刷新时间，GetNowTime获取的是上一次刷新时的时间
*/

#ifndef __NOW_TIME_HPP__
#define __NOW_TIME_HPP__

#include "UnixTime.hpp"
#include "TimeValue.hpp"
#include "SingletonTemplate.hpp"

namespace ServerLib
{

class CNowTime
{
public:
    CNowTime()
    {
        m_bUsePseudoTime = false;
    }
    ~CNowTime() {}

public:
    //!获取当前UnixTime时间（秒级）
    time_t GetNowTime() const;
    //!使用假UnixTime时间，当前时间刷新由应用调用RefreshNowTime控制
    void EnablePseudoTime()
    {
        m_bUsePseudoTime = true;
    }
    //!不使用假UnixTime时间，当前时间由系统调用生成
    void DisablePseudoTime()
    {
        m_bUsePseudoTime = false;
    }
    //!刷新当前UnixTime时间
    void RefreshNowTime()
    {
        m_stPseudoTime.RefreshTime();
    }


    //!获取当前timeval时间（微秒级）
    timeval GetNowTimeVal() const;
    //!使用假timeval时间，当前时间刷新由应用调用RefreshNowTimeVal控制
    void EnablePseudoTimeVal()
    {
        m_bUsePseudoTime = true;
    }
    //!不使用假timeval时间，当前时间由系统调用生成
    void DisablePseudoTimeVal()
    {
        m_bUsePseudoTime = false;
    }
    //!刷新当前的TimeVal
    void RefreshNowTimeVal()
    {
        m_stPseudoTimeValue.RefreshTime();
    }

private:
    bool m_bUsePseudoTime; //!<是否使用假时间，由应用自行调用RefreshNowTime来刷新当前时间
    CUnixTime m_stPseudoTime; //!<管理time_t的类
    CTimeValue m_stPseudoTimeValue; //!<管理timeval的类
};

typedef CSingleton<CNowTime> NowTimeSingleton;

}

#endif //__NOW_TIME_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
