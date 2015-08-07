#ifndef __CACHE_TIME_HPP__
#define __CACHE_TIME_HPP__

#include <time.h>

class CCacheTime
{
private:
    time_t m_stCreatedTime; // 缓存结点创建的时间

protected:
    CCacheTime();

public:
    void SetCreatedTime(const time_t* pstTime);
    time_t* GetCreatedTime();
    void GetCreatedTime(char* pszTime, unsigned int uiBufLen);
};

#endif // __CACHE_TIME_HPP__
