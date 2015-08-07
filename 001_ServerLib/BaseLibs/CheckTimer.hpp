// ********************************************************************
// 作    者： amacroli(idehong@gmail.com)
// 创建日期： 2011年7月8日
// 描    述： 定时器
// 应    用：
// ********************************************************************

#ifndef __CHECKTIMER_HPP__
#define __CHECKTIMER_HPP__


#include <sys/time.h>
#include <unistd.h>

#include <list>
#include <map>

using namespace std;

class CBaseTimer
{
public:
    CBaseTimer()
    {

    }
    virtual ~CBaseTimer()
    {

    }

public:
    virtual void OnTimer(unsigned int uiUin, unsigned int uiEventID) = 0;
};


class CCheckTimer
{
public:
    CCheckTimer(void):m_uiTurnCount(5000)
    {
        m_uiRunTimerCount = 0;
    }
    ~CCheckTimer(void)
    {

    }
public:
    struct TTimerInfo
    {
        bool bRemove;
        unsigned int m_uiEventID;
        timeval m_tvTime;
        unsigned int m_uiSpaceTime;
        CBaseTimer* pFunCallBack;
    };

public:
    int AddTimer(unsigned int uiUin, CBaseTimer* pFunCallBack, unsigned int uiEventID, unsigned int uiSpaceTime)
    {
        if (NULL == pFunCallBack || uiEventID <= 0)
        {
            return -1;
        }

        TTimerInfo stTimerInfo = {0x00};
        stTimerInfo.bRemove = false;
        stTimerInfo.m_uiEventID = uiEventID;
        gettimeofday(&stTimerInfo.m_tvTime, NULL);
        stTimerInfo.m_uiSpaceTime = uiSpaceTime;
        stTimerInfo.pFunCallBack = pFunCallBack;

        TIMER_INFO_CONTAINER::iterator it = m_mapTimerInfo.find(uiUin);
        if (it != m_mapTimerInfo.end())
        {
            it->second.push_back(stTimerInfo);
        }
        else
        {
            LIST_EVENT& rListEvent = m_mapTimerInfo[uiUin];
            rListEvent.push_back(stTimerInfo);
        }
        return 0;
    }

    void RemoveTimer(unsigned int uiUin, CBaseTimer* pFunCallBack, unsigned int uiEventID)
    {
        if (NULL == pFunCallBack)
        {
            return;
        }

        TIMER_INFO_CONTAINER::iterator it = m_mapTimerInfo.find(uiUin);
        if (it == m_mapTimerInfo.end())
        {
            return;
        }

        LIST_EVENT::iterator it_event = it->second.begin();
        for (; it_event != it->second.end(); ++it_event)
        {
            if (pFunCallBack == it_event->pFunCallBack && uiEventID == it_event->m_uiEventID)
            {
                it_event->bRemove = true;
                //it_event = it->second.erase(it_event);
            }
//			else
//			{
//				++it_event;
//			}

        }
    }

    void RemoveTimer(unsigned int uiUin)
    {
        TIMER_INFO_CONTAINER::iterator it = m_mapTimerInfo.find(uiUin);
        if (it == m_mapTimerInfo.end())
        {
            return;
        }

        LIST_EVENT::iterator it_event = it->second.begin();
        for (; it_event != it->second.end(); ++it_event)
        {
            it_event->bRemove = true;
        }
    }

    void Run()
    {
        //每次轮询 uiTurnCount 个定时器
        size_t iTimerCount = m_mapTimerInfo.size();
        if (iTimerCount > 0)
        {
            m_uiRunTimerCount %= iTimerCount;
        }
        else
        {
            m_uiRunTimerCount = 0;
        }

        timeval tvNow = {0x00};
        gettimeofday(&tvNow, NULL);
        TIMER_INFO_CONTAINER::iterator it = m_mapTimerInfo.begin();
        LIST_EVENT::iterator it_event;
        TTimerInfo* pstTimerInfo = NULL;
        for (; it != m_mapTimerInfo.end(); ++it)
        {
            m_uiRunTimerCount++;
            it_event = it->second.begin();
            for (; it_event != it->second.end(); ++it_event)
            {
                pstTimerInfo = &(*it_event);
                if (NULL == pstTimerInfo || pstTimerInfo->bRemove)
                {
                    continue;
                }

                if (IsTimeOut(pstTimerInfo, &tvNow))
                {
                    pstTimerInfo->pFunCallBack->OnTimer(it->first, pstTimerInfo->m_uiEventID);
                }
            }

            if (m_uiRunTimerCount > m_uiTurnCount)
            {
                break;
            }
        }

        DelTimeObj();
    }

    void DelTimeObj()
    {
        TIMER_INFO_CONTAINER::iterator it = m_mapTimerInfo.begin();
        LIST_EVENT::iterator it_event;
        TTimerInfo* pstTimerInfo = NULL;
        //真删除
        it = m_mapTimerInfo.begin();
        for (; it != m_mapTimerInfo.end();)
        {
            it_event = it->second.begin();
            for (; it_event != it->second.end();)
            {
                pstTimerInfo = &(*it_event);
                if (NULL != pstTimerInfo && pstTimerInfo->bRemove)
                {
                    it->second.erase(it_event++);
                    //it_event = it->second.erase(it_event);
                }
                else
                {
                    ++it_event;
                }
            }

            if (it->second.empty())
            {
                m_mapTimerInfo.erase(it++);
            }
            else
            {
                ++it;
            }
        }
    }


private:
    bool IsTimeOut(TTimerInfo* pstTimerInfo, timeval* pstTimeNow)
    {
        int iMilliSecondsNow = pstTimeNow->tv_sec * 1000 + pstTimeNow->tv_usec / 1000;
        int iMilliSecondsRecord = pstTimerInfo->m_tvTime.tv_sec * 1000 + pstTimerInfo->m_tvTime.tv_usec / 1000;

        int iDiffTime = iMilliSecondsNow - iMilliSecondsRecord;

        return (iDiffTime > 0 && (unsigned int)iDiffTime > pstTimerInfo->m_uiSpaceTime) ? true : false;
    }

private:
    typedef std::list<TTimerInfo> LIST_EVENT;
    typedef std::map<unsigned int/*uin*/, LIST_EVENT>	TIMER_INFO_CONTAINER;
    TIMER_INFO_CONTAINER m_mapTimerInfo;
    unsigned int m_uiRunTimerCount;
    const unsigned int m_uiTurnCount;
};
#endif // __CHECKTIMER_HPP__
