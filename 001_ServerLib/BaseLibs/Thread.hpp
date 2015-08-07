#ifndef _THREAD_HPP_
#define _THREAD_HPP_

#include <pthread.h>


enum eRunStatus
{
    rt_init = 0,
    rt_blocked = 1,
    rt_running = 2,
    rt_stopped = 3
};

void* ThreadProc( void *pvArgs );

class CThread
{
public:
    CThread();
    virtual ~CThread();

    virtual int PrepareToRun() = 0;
    virtual int Run() = 0;
    virtual int IsToBeBlocked() = 0;

    int CreateThread();
    int WakeUp();
    int StopThread();

protected:
    int CondBlock();

    pthread_t m_hTrd;
    pthread_attr_t m_stAttr;
    pthread_mutex_t m_stMutex;
    pthread_cond_t m_stCond;
    int m_iRunStatus;
    unsigned char m_abyRetVal[64];

private:
};

#endif
