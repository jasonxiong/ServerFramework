#ifndef __APP_DEF_HPP__
#define __APP_DEF_HPP__


//在这里添加一些宏定义
#ifdef _DEBUG_
const int MAX_SERVICEAPP_NUMBER = 2;                //最多启动的服务进程的数目
const int APP_ROLEDB_MAX_SLEEP_USEC = 10 * 1000;    //线程sleep时间
#else
const int MAX_SERVICEAPP_NUMBER = 8;                //最多启动的服务进程的数目
const int APP_ROLEDB_MAX_SLEEP_USEC = 10;           //线程sleep时间
#endif

//统计Cache定义
const int STAT_CACHE_ID_TIMER      = 0;        //定时器
const int STAT_CACHE_ID_TIMERINDEX = 1;        //定时器索引
const int STAT_CHCHE_ID_TRANSFER   = 2;        //客户端消息头

#define STAT_CACHE_NAME_TIMER       "TimerCache"
#define STAT_CACHE_NAME_TIMERINDEX  "TimerIndexCache"

#define APP_CONFIG_FILE     "../conf/GameServer.tcm"

//连接的MYSQL数据库相关的配置文件
#define ROLEDBINFO_CONFIG_FILE "../conf/DBMSConf.xml"

//定义MYSQL的表名

//玩家角色数据表的表名
#define MYSQL_USERINFO_TABLE "t_crystal_userdata"

//玩家角色数据表的列数,详细字段参见建表语句
#define MYSQL_USERINFO_FIELDS   9

#endif


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
