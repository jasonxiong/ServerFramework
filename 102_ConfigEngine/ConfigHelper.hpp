#ifndef __CONFIG_HELPER_HPP__
#define __CONFIG_HELPER_HPP__

#include <typeinfo>
#include <stdint.h>

#include "ErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "StringUtility.hpp"
#include "SortArray.hpp"

using namespace ServerLib;

extern int GAME_GET_WORLD_ID(uint64_t busid);
extern int GAME_GET_ZONE_ID(uint64_t busid);
extern int GAME_GET_FUNC_ID(uint64_t busid);
extern int GAME_GET_INST_ID(uint64_t busid);

// Server服务器ID, 等同于TCM的FuncitionID
typedef enum enGameServerID
{
    GAME_SERVER_UNDEFINE        = -1,
    GAME_SERVER_WORLD           = 0,
    GAME_SERVER_ZONE            = 1,
    GAME_SERVER_ROLEDB          = 2,
    GAME_SERVER_ACCOUNT         = 3,
    GAME_SERVER_MAIL            = 4,
    GAME_SERVER_CHARGE          = 5,
    GAME_SERVER_LOGDB           = 6,
    GAME_SERVER_MAILDB          = 7,
    GAME_SERVER_FRIENDDB        = 8,
    GAME_SERVER_GUILDDB         = 9,
    GAME_SERVER_CLUSTER         = 10,

    GAME_SERVER_LOTUSACCOUNT    = 11,
    GAME_SERVER_LOTUSZONE       = 12,
    GAME_SERVER_LOTUSREGAUTH    = 13,

    GAME_SERVER_REGAUTH         = 14,

    GAME_SERVER_ACCOUNTDB       = 15,

    GAME_SERVER_NAMEDB          = 16,

    GAME_SERVER_MAX             = 17,
} EGameServerID;

typedef enum enServerStatus
{
    // 正常启动中
    // 1) Zone向World发送启动消息
    // 2) World向Cluster发送启动消息
    GAME_SERVER_STATUS_INIT    = 1,

    // 恢复启动中
    GAME_SERVER_STATUS_RESUME  = 2,

    // 空闲运行中
    GAME_SERVER_STATUS_IDLE    = 3,

    // 忙运行中
    GAME_SERVER_STATUS_BUSY    = 4,

    // 准备停止
    // 1) 通知客户端准备停服下线
    // 2) 开始进入STOP状态倒计时
    GAME_SERVER_STATUS_PRESTOP = 5,

    // 停止中
    // 1) 通知Lotus停止所有的输入接收
    // 2) 处理所有的Lotus数据
    // 3) 处理所有的TBus数据
    // 4) 将所有在线玩家踢下线, 并通知Lotus断开连接
    // 5) 停止Lotus和Zone服务器
    GAME_SERVER_STATUS_STOP    = 6,

} EGameServerStatus;

key_t GenerateServerShmKey(EGameServerID enServerID, int iKeyIndex);

extern EGameServerStatus g_enServerStatus;
inline void SetServerStatus(EGameServerStatus enStatus)
{
    g_enServerStatus = enStatus;
}

inline EGameServerStatus GetServerStatus()
{
    return g_enServerStatus;
}

// 生成的ServerID: world:16.zone:16.function:16.instance:16
// 按照TCM规范, 默认instance从1开始
// ZoneID为0, 表示属于整个world的服务器
inline uint64_t GetServerBusID(short iWorldID, EGameServerID enServerID, short iInstance = 1, short iZoneID = 0)
{
    uint64_t ullBusID = ((uint64_t)iWorldID) << 48;

    ullBusID += ((uint64_t)iZoneID)<<32;
    ullBusID += ((uint64_t)enServerID)<<16;
    ullBusID += iInstance;

    return ullBusID;
};

//todo jasonxiong 这个函数在每个服务器里面都要重新定义的,需要使用配置
const char* GetZmqBusAddress(uint64_t ullClientBusID, uint64_t ullServerBusID);

// 获取资源路径
extern int GetCommonResourcePath(char* pszPathBuffer, const int iPathLen, const char* pszResName);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 静态内存对象使用配置

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 动态内存对象使用配置

#ifdef _DEBUG_

//////////////////////////////////////////
// Zone
/////////////////////////////////////////

// 单位ID
const int MAX_UNIT_NUMBER_IN_ZONE = 10000;

// 角色对象
const int MAX_ROLE_OBJECT_NUMBER_IN_ZONE = 100;

//游戏内战斗单位对象
const int MAX_FIGHT_UNIT_NUMBER_IN_ZONE = 100 * MAX_UNIT_NUMBER_IN_ZONE;

//游戏战斗战场对象
const int MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE = MAX_ROLE_OBJECT_NUMBER_IN_ZONE;

//游戏战斗时动态战斗单位对象
const int MAX_COMBAT_UNIT_OBJECT_NUMBER_IN_ZONE = MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE * 10;

//游戏战斗时Buff对象
const int MAX_COMBAT_BUFF_OBJECT_NUMBER_IN_ZONE = MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE * 15;

const int MAX_COMBAT_TRAP_OBJECT_NUMBER_IN_ZONE = MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE * 8;

//////////////////////////////////////////
// World
/////////////////////////////////////////
const int MAX_REGISTER_ROLE_NUMBER_PER_WORLD = 10000;

/////////////////////////////////////////
//主HashMap节点大小
const int MAX_ROLE_NAME_NUMBER = 10000;
const int MAX_GUILD_NAME_NUMBER = 10000;
//预缓冲节点大小
const int MAX_PRE_CACHE_SIZE = 10000;
//备份缓冲节点大小
const int MAX_ROLE_BACKUP_CACHE_SIZE = 10000;
const int MAX_GUILD_BACKUP_CACHE_SIZE = 100;
//删除缓冲节点大小
const int MAX_ROLE_DELETE_CACHE_SIZE = 10000;
const int MAX_GUILD_DELETE_CACHE_SIZE = 100;

const int MAX_ROLE_OBJECT_NUMBER_IN_WORLD = 6000;

//////////////////////////////////////////
// Cluster
/////////////////////////////////////////
const int MAX_ROLE_OBJECT_NUMBER_IN_CLUSTER = 1000;

//////////////////////////////////////////
// Register & Auth
/////////////////////////////////////////
const int MAX_REGISTER_ACCOUNT_REQUEST_NUMBER = 100000;
const int MAX_ACCOUNT_OBJ_CACHE_NUMBER = 1000;

#else
//////////////////////////////////////////
// Zone
/////////////////////////////////////////

// 角色对象
const int MAX_ROLE_OBJECT_NUMBER_IN_ZONE = 4000;

// 单位ID
const int MAX_UNIT_NUMBER_IN_ZONE = (MAX_ROLE_OBJECT_NUMBER_IN_ZONE) * 2;

//游戏内战斗单位对象
const int MAX_FIGHT_UNIT_NUMBER_IN_ZONE = 100 * MAX_UNIT_NUMBER_IN_ZONE;

//游戏战斗战场对象
const int MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE = MAX_ROLE_OBJECT_NUMBER_IN_ZONE;

//游戏战斗时动态战斗单位对象
const int MAX_COMBAT_UNIT_OBJECT_NUMBER_IN_ZONE = MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE * 10;

//游戏战斗时Buff对象
const int MAX_COMBAT_BUFF_OBJECT_NUMBER_IN_ZONE = MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE * 15;

const int MAX_COMBAT_TRAP_OBJECT_NUMBER_IN_ZONE = MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE * 8;

//////////////////////////////////////////
// World
/////////////////////////////////////////
const int MAX_ZONE_NUMBER = 3;
const int MAX_REGISTER_ROLE_NUMBER_PER_WORLD = 1000000;
const int MAX_TEAM_OBJECT_NUMBER = 4000;
const int MAX_TEAM_TRANSACTION_OBJECT_NUMBER = 4000;
const int MAX_ROLE_OBJECT_NUMBER_IN_WORLD = 6000;
const int MAX_GUILD_NUMBER_PER_WORLD = 3000;
const int MAX_GUILD_INVITE_REQUEST_NUMBER = 1000;
const int MAX_GUILD_APPLY_OBJECT_NUMBER = 6000;
const int MAX_ROLE_GUILD_MAPPING_NUMBER = 250000;

//////////////////////////////////////////
// Name
///////////////////////////////////////
//主HashMap节点大小
const int MAX_ROLE_NAME_NUMBER = 100000;
const int MAX_GUILD_NAME_NUMBER = 5000;

//预缓冲节点大小
const int MAX_PRE_CACHE_SIZE = 10000;

//备份缓冲节点大小
const int MAX_ROLE_BACKUP_CACHE_SIZE = 10000;
const int MAX_GUILD_BACKUP_CACHE_SIZE = 5000;

//删除缓冲节点大小
const int MAX_ROLE_DELETE_CACHE_SIZE = 10000;
const int MAX_GUILD_DELETE_CACHE_SIZE = 5000;

//////////////////////////////////////////
// Cluster
/////////////////////////////////////////
const int MAX_ROLE_OBJECT_NUMBER_IN_CLUSTER = 1000000;

//////////////////////////////////////////
// Register & Auth
/////////////////////////////////////////
const int MAX_REGISTER_ACCOUNT_REQUEST_NUMBER = 10000;

const int MAX_ACCOUNT_OBJ_CACHE_NUMBER = 1000000;

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 内部帐号
const unsigned int MAX_TEST_UIN = 10000;

// Zone维护 ：角色数据重试写数据库时间间隔（秒）
const int ROLEDATA_REWRITE_SECONDS = 10;

#ifdef _DEBUG_
const int SERVER_PRESTOP_TIME = 0;
#else
const int SERVER_PRESTOP_TIME = 20;
#endif

typedef struct MiniTdrResHead
{
    int32_t iMagic;             //Magic Number of Resouce File
    int32_t iVersion;           //Version of Resouce File
    int32_t iUnit;              //size of each resouce calced by byte
    int32_t iCount;             //total count number of resouces
} TRESHEAD;

template <typename T, int N>
int LoadTemplateCfg(const char * pCfgFilePath, T (&astTArrar)[N], int *piNum, bool (*IfPred)(const void *))
{
    ASSERT_AND_LOG_RTN_INT(pCfgFilePath);
    ASSERT_AND_LOG_RTN_INT(piNum);

    char szResPath[256] = {0};

    *piNum = 0;
    memset(astTArrar, 0, sizeof(astTArrar));

    // 加载配置信息
    GetCommonResourcePath(szResPath, sizeof(szResPath), pCfgFilePath);

    //打开配置文件
    FILE* pFile = fopen(szResPath, "rb");
    if(!pFile)
    {
        TRACESVR("Fail to open config file %s\n", szResPath);
        return -1;
    }

    fseek(pFile, 0L, SEEK_END);     //定位到文件末尾
    int iFileLen = ftell(pFile);    //获取文件大小
    fseek(pFile, 0L, SEEK_SET);     //定位到文件头

    char* pBuffer = (char*)malloc(iFileLen+1);
    fread(pBuffer, iFileLen, 1, pFile);
    pBuffer[iFileLen] = '\0';

    //先解析头
    TRESHEAD* pstHead = (TRESHEAD*)pBuffer;
    if(pstHead->iUnit > (int)sizeof(T))
    {
        free(pBuffer);
        fclose(pFile);
        TRACESVR("Invalid Unit Size: version not matched, %d %ld\n", pstHead->iUnit, sizeof(T));
        return -2;
    }

    //判断配置单元的个数是否已经超出程序设置的个数
    if (pstHead->iCount > N)
    {
        TRACESVR("Too many Configs: %d:%d\n", pstHead->iCount, N);

        free(pBuffer);
        fclose(pFile);
        return -5;
    }
    
    char* pszResUnit = pBuffer + 136;   //头的长度固定为136个字节

    memcpy((void*)astTArrar, (void*)pszResUnit, pstHead->iCount*sizeof(T));

    *piNum = pstHead->iCount;

    free(pBuffer);

    if (IfPred)
    {
        MyDeleteArray_If((const void *)astTArrar, &pstHead->iCount, sizeof(T), IfPred);
    }

    fclose(pFile);

    TRACESVR("Load %s array: num = %d\n", typeid(T).name(), *piNum);

    return 0;
}

#endif


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
