/**
*@file TimeUtility.hpp
*@author jasonxiong
*@date 2009-11-23
*@version 1.0
*@brief Time相关函数封装类
*
*
*/

#ifndef __TIME_UTIL_HPP__
#define __TIME_UTIL_HPP__

//在这添加标准库头文件
#include <time.h>
#include <sys/time.h>
#include "TimeValue.hpp"

const int SECOND_IN_ONE_MIN = 60;
const int SECONDS_IN_ONE_HOUR = 60 *60;// 一天的秒数
const int HOURS_IN_ONE_DAY = 24;

const int SECONDS_IN_ONE_DAY = 24 * 60 * 60;


namespace ServerLib
{

typedef struct
{
    char m_cHoursNum;
    char m_acHour[HOURS_IN_ONE_DAY];
} MultiHours;

class CTimeUtility
{
public:
    CTimeUtility() {}
    ~CTimeUtility() {}

public:
    //!获取当前时间
    static time_t GetNowTime()
    {
        return time(NULL);
    }
    //!将用tTime表示的时间转换成用"%Y-%m-%d %H-%M-%S"表示
    static int ConvertUnixTimeToTimeString(time_t tTime, char* szTimeString);
    //!将用tTime表示的时间转换成用"%Y-%m-%d"表示
    static int ConvertUnixTimeToDateString(time_t tTime, char* szDateString);
    //!将用tTime表示的时间转换成用"%Y-%m"表示
    static int ConvertUinxTimeToMonthString(time_t tTime, char* szMonthString);
    //!将用"%Y-%m-%d %H-%M-%S"表示的时间转换成UnixTime
    static int ConvertTimeStringToUnixTime(char* szTimeString, time_t& rtTime);
    //!将用"%Y-%m-%d %H-%M-%S"表示的时间转换成tm表示的结构
    static int ConvertTimeStringToTmStruct(char* szTimeString, struct tm& rstTM);
    //!将用tTime表示的时间转换成某一年之后的某一天
    static int ConvertUnixTimeToDaysAfterYear(time_t tTime, int iYear, unsigned short& rushDays);
    //!tvResult = tvA + tvB
    static int TimeValPlus(const timeval& tvA, const timeval& tvB, timeval& tvResult);
    //!tvResult = tvA - tvB
    static int TimeValMinus(const timeval& tvA, const timeval& tvB, timeval& tvResult);

    // 获取tTime是一周的第几天
    // 返回值: 周一(1) - 周日(7)
    static int GetWeekDay(time_t tTime);

    // 获取当天指定的时间点
    // tTime: 当前时间点
    // iTimeSeconds: 从0点开始的秒数, 默认为0, 即0点
    static time_t GetTodayTime(time_t tTime, int iTimeSeconds = 0);

    // 获取本周指定的时间点
    // tTime: 当前时间点
    // iWeekDay: 周一(1) - 周日(7)
    // iDaySeconds: iWeekDay的从0点开始的秒数
    static time_t GetWeekTime(time_t tTime, int iWeekDay, int iDaySeconds);

    // 获得指定iHour的上一个时间点
    // tTime: 当前时间点
    // 如果当前时间小于iHour 则返回前一天的iHour对应的time
    // 否则返回当天 iHour对应的time
    static int GetLastNearestHourTime(time_t tTime, int iHour);

    // 获得指定iHour的下一个时间点
    // tTime: 当前时间点
    // 如果当前时间小于 iHour 则返回当天的iHour对应的time
    // 否则返回下一天 iHour对应的time
    static int GetNextNearestHourTime(time_t tTime, int iHour);

    //获得指定iHour的下一个时间点
    // tTime: 当前时间点
    // 如果当前时间小于 iSec 则返回当天的iSec对应的time
    // 否则返回下一天 iSec对应的time
    static int GetNextNearestSecTime(time_t tTime, int iSec);

    // 获得指定iHour的下几个时间点
    // tTime: 当前时间点
    // 如果当前时间小于 iHour 则返回当天的iHour对应的time
    // 否则返回下一天 iHour对应的time
    static int GetNextNearestMutiHourTimes(time_t tTime, const MultiHours* pstMultiHours);

    // 获得指定时间的上一周时间点
    // tTime: 当前时间点
    // iWeekDay: 周一(1) - 周日(7)
    // iDaySeconds: iWeekDay的从0点开始的秒数
    static int GetLastNearestWeekTime(time_t tTime, int iWeekDay, int iDaySeconds);

    // 获得指定时间的下一周时间点
    // tTime: 当前时间点
    // iWeekDay: 周一(1) - 周日(7)
    // iDaySeconds: iWeekDay的从0点开始的秒数
    static int GetNextNearestWeekTime(time_t tTime, int iWeekDay, int iDaySeconds);

    // 返回年月日，时分秒，星期
    static int GetTimeOfDay(time_t tTime,
                            int* iYear = NULL,
                            int* iMon = NULL,
                            int* iDay = NULL,
                            int* iHour = NULL,
                            int* iMin = NULL,
                            int* iSec = NULL,
                            int* iWeekDay = NULL);

    /*
     *如果tTime > year..sec return 1
     *tTime < year..sec return -1
     *tTime == year..sec return 0
     *
     *注意iHour[0-23]
     */
    static int CompareDateTime(time_t tTime, int iYear, int iMonth, int iDay, int iHour, int iMin, int iSec);

    /*
    *如果tTime > year..sec return 1
    *tTime < year..sec return -1
    *tTime == year..sec return 0
    *
    *注意iHour[0-23]
    */
    static int CompareTime(time_t tTime, int iHour, int iMin, int iSec);

    /* 是否是同一个月 */
    static bool IsInSameMonth(time_t tTime1, time_t tTime2);

    /* 是否是同一年 */
    static bool IsInSameYear(time_t tTime1, time_t tTime2);

    static time_t MakeTime(int iYear, int iMonth, int iDay, int iHour, int iMin, int iSec);

    // 是否是同一天
    static bool IsInSameDay(time_t tTime1, time_t tTime2);

    //是否在同一周
    static bool IsInSameWeek(time_t tTime1, time_t tTime2);

    //获取下一天0点的时间
    static time_t GetNextDayBeginTime();
public:
    //保存tick开始的timevalue
    static CTimeValue m_stTimeValueTick;

    //保存tick开始的time
    static time_t m_uiTimeTick;

};
}

#endif //__TIME_UTIL_HPP__
///:~
