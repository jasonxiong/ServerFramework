#ifndef __APP_DEF_HPP__
#define __APP_DEF_HPP__


//在这里添加一些宏定义
#ifdef _DEBUG_
const int MAX_SERVICEAPP_NUMBER = 2; //最多启动的服务进程的数目
#else
const int MAX_SERVICEAPP_NUMBER = 8; //最多启动的服务进程的数目
#endif

//统计Cache定义
const int STAT_CACHE_ID_TIMER      = 0;        //定时器
const int STAT_CACHE_ID_TIMERINDEX = 1;        //定时器索引
const int STAT_CHCHE_ID_TRANSFER   = 2;        //客户端消息头

#define STAT_CACHE_NAME_TIMER       "TimerCache"
#define STAT_CACHE_NAME_TIMERINDEX  "TimerIndexCache"

typedef enum enIOIndex
{
    EII_LoginProxy  = 0,
    EII_FloraProxy  = 1,
    EII_TTCProxy    = 2,
    EII_QueryProxy  = 3,
    EII_AvatarProxy = 4,

    //100 以内为ProxyID
    EII_CodeQueue = 100,

} EIOIndex;

#define APP_CONFIG_FILE     "../conf/GameServer.tcm"

#endif

