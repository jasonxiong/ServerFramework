#ifndef __APP_DEF_HPP__
#define __APP_DEF_HPP__


//在这里添加一些宏定义
#ifdef _DEBUG_
const int MAX_SERVICEAPP_NUMBER = 2;                //最多启动的服务进程的数目
const int APP_NAMEDB_MAX_SLEEP_USEC = 10 * 1000;    //线程sleep时间
#else
const int MAX_SERVICEAPP_NUMBER = 8;                //最多启动的服务进程的数目
const int APP_NAMEDB_MAX_SLEEP_USEC = 10;           //线程sleep时间
#endif

const int CUBE_DB_UIN_MODE = 256; //在选择表的时候Uin需要按256来模

//统计Cache定义
const int STAT_CACHE_ID_TIMER      = 0;        //定时器
const int STAT_CACHE_ID_TIMERINDEX = 1;        //定时器索引
const int STAT_CHCHE_ID_TRANSFER   = 2;        //客户端消息头

#define STAT_CACHE_NAME_TIMER       "TimerCache"
#define STAT_CACHE_NAME_TIMERINDEX  "TimerIndexCache"
#define STAT_CACHE_NAME_TRANSFER    "TransferCache"

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

//连接的MYSQL数据库相关的配置文件
#define NAMEDBINFO_CONFIG_FILE "../conf/DBMSConf.xml"

//定义MYSQL的表名

//名字数据库的表名
#define MYSQL_NAMEINFO_TABLE "t_wulin_nameinfo"

//名字数据库的列数定义: nickname, nametype, nameid
#define MYSQL_NAMEINFO_FIELDS   3

#endif

