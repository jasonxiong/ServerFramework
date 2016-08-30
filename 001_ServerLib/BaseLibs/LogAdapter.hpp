/**
*@file LogAdapter.hpp
*@author jasonxiong
*@date 2009-11-04
*@version 1.0
*@brief 为方便CLogFile类使用，写几个偏业务的打印日志类
*
*   使用方法：
*   （1）使用TRACESVR打印重要主控日志，必打，想改变日志文件名调用ServerLogSingleton::Instance()->SetServerLogName
*   （2）使用TRACESVR_EX打印有等级的主控日志
*   （3）TRACE_FUNC_BEG、TRACE_FUNC_END(RET)用于打印函数开始和结束
*   （4）使用ERRORLOG打印错误日志，必打
*   （5）使用TRACEPLAYER来根据Uin打印日志
*   （6）使用PlayerLogSingleton::Instance()->ClearTraceUinSet清空Uin白名单
*   （7）使用PlayerLogSingleton::Instance()->AddTraceUin添加Uin到白名单中
*/

#ifndef __LOG_ADAPTER_HPP__
#define __LOG_ADAPTER_HPP__

#include "LogFile.hpp"
#include "CommonDef.hpp"
#include "SingletonTemplate.hpp"

#ifndef _GAME_NO_BACKTRACE_
#include "BacktraceInfo.hpp"
#endif //_GAME_NO_BACKTRACE_

typedef struct tagEventId
{
    int m_iEventTime;                       // 事件发生的时间
    unsigned short m_ucWorldID : 12;        // 事件发生的World
    unsigned short m_ucZoneID  : 4;         // 事件发生的Zone
    unsigned short m_usEventSeq;            // 事件发生的序号

    // 默认构造函数
    tagEventId()
    {
        m_iEventTime = 0;
    }

} TEventId;

namespace ServerLib
{

class CServerLogAdapter
{
public:
    CServerLogAdapter();
    ~CServerLogAdapter() {}

    int SetServerLogName(const char* pszLogName);
    int SetServerLogSuffix(ENMADDDATESUFFIXTYPE eType);
    int SetLogLevel(int iLogLevel);
    int ReloadLogConfig(TLogConfig& rstLogConfig);

    TLogConfig GetLogConfig()
    {
        return m_stLogFile.GetLogConfig();
    }
    void IncTraceDepth()
    {
        ++m_iTraceDepth;
    }
    void DecTraceDepth()
    {
        --m_iTraceDepth;
    }

public:
    CLogFile m_stLogFile;
    int m_iTraceDepth;
};

//!一般来说只会用到一个CServerLogAdapter类，所以实现成单件
typedef CSingleton<CServerLogAdapter> ServerLogSingleton;

class CAutoCtrlTraceDepth
{
public:
    CAutoCtrlTraceDepth()
    {
        ServerLogSingleton::Instance()->IncTraceDepth();
    }
    ~CAutoCtrlTraceDepth()
    {
        ServerLogSingleton::Instance()->DecTraceDepth();
    }
};

#ifdef _GAME_NO_LOG_
    #define TRACESVR(format, ...)
#else
    //!主控日志，无日志等级，强制打
    #define TRACESVR(format, ...) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_ANY, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

#define SVRLOGFD ServerLogSingleton::Instance()->m_stLogFile.GetFD
#define SVRLOGFP ServerLogSingleton::Instance()->m_stLogFile.GetFilePointer

#ifdef _GAME_NO_LOG_
    #define TRACESVR_EX(iLogLevel, format, ...)
#else
    //!主控日志，有日志等级
    #define TRACESVR_EX(iLogLevel, format, ...) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(iLogLevel, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

//!定义方便日志打印的宏
#ifdef _GAME_NO_LOG_
    #define LOGDEBUG(format, ...)
    #define LOGINFO(format, ...)
    #define LOGDETAIL(format, ...)
    #define LOGWARNING(format, ...)
    #define LOGERROR(format, ...)
#else
    #define LOGDEBUG(format, ...) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_DEBUG, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
    #define LOGINFO(format, ...) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_INFO, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
    #define LOGDETAIL(format, ...) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_DETAIL, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
    #define LOGWARNING(format, ...) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_WARNING, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
    #define LOGERROR(format, ...) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_ERROR, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

#ifdef _GAME_NO_LOG_
    #define DUMP_HEX(iLogLevel, szDesc, acBuf, iBufLen)
#else
//!将Buffer用16进制的格式写到主控日志文件中
#define DUMP_HEX(iLogLevel, szDesc, acBuf, iBufLen) \
        ServerLogSingleton::Instance()->m_stLogFile.WriteLog(iLogLevel, "%s\n", szDesc); \
        ServerLogSingleton::Instance()->m_stLogFile.DumpHex(acBuf, iBufLen);
#endif

//!设置主控日志的日志等级
#define SETTRACELEVEL ServerLogSingleton::Instance()->SetLogLevel

//!设置主控日志的文件名
#define SETLOGTAG ServerLogSingleton::Instance()->SetServerLogName

//!设置主控日志的文件名格式
#define SETLOGSUFFIX ServerLogSingleton::Instance()->SetServerLogSuffix

#ifdef _GAME_NO_LOG_
#define TRACE_DEPTH_AUTO_INC
#else
#define TRACE_DEPTH_AUTO_INC CAutoCtrlTraceDepth stTempTraceDepth
#endif //_GAME_NO_LOG_

#ifdef _GAME_NO_LOG_
#define TRACE_FUNC_BEG
#else
//!主控日志，打印函数开始，日志等级为LOG_LEVEL_CALL
#define TRACE_FUNC_BEG ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_CALL, \
            ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, "*******beg*******\n")
#endif

#ifdef _GAME_NO_LOG_
#define TRACE_FUNC_BEG_EX(iLogLevel)
#else
//!主控日志，打印函数开始，日志等级可控
#define TRACE_FUNC_BEG_EX(iLogLevel) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(iLogLevel, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, "*******beg*******\n")
#endif

#ifdef _GAME_NO_LOG_
#define TRACE_FUNC_END
#else
//!主控日志，打印函数结束及返回值，日志等级为LOG_LEVEL_CALL
#define TRACE_FUNC_END ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_CALL, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, "*******end*******\n");
#endif //_GAME_NO_LOG_

#ifdef _GAME_NO_LOG_
#define TRACE_FUNC_END_EX(iLogLvel)
#else
//!主控日志，打印函数结束及返回值，日志等级为LOG_LEVEL_CALL
#define TRACE_FUNC_END_EX(iLogLvel) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(iLogLvel, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, "*******end*******\n");
#endif //_GAME_NO_LOG_

#ifdef _GAME_NO_LOG_
#define TRACE_RETURN(RET) return RET
#else
//!主控日志，打印函数结束及返回值，日志等级为LOG_LEVEL_CALL
#define TRACE_RETURN(RET) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_CALL, \
            ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, "*******end*******\n"); \
        return RET
#endif

#ifdef _GAME_NO_LOG_
#define TRACE_ERROR_RETURN(RET) return RET
#else
//!主控日志，打印函数结束及返回值，日志等级为LOG_LEVEL_CALL
#define TRACE_ERROR_RETURN(RET) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_CALL, \
            ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, "*******end*******, iRet = %d\n", RET); \
        return RET
#endif

#ifdef _GAME_NO_LOG_
#define TRACE_AND_DO(X) X
#else
//!主控日志，打印程序语句并执行，日志等级为LOG_LEVEL_DETAIL
#define TRACE_AND_DO(X) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_DETAIL, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, "%s\n", #X); X
#endif

template <bool>
struct CompileAssert
{
};

#undef COMPILE_ASSERT
#define COMPILE_ASSERT(expr, msg) \
    typedef CompileAssert<(bool(expr))> msg[bool(expr)? 1: -1]

class CErrorLogAdapter
{
public:
    CErrorLogAdapter();
    ~CErrorLogAdapter() {}

    int ReloadLogConfig(TLogConfig& rstLogConfig);

public:
    CLogFile m_stLogFile;
};

//!一般来说只会用到一个CErrorLogAdapter类，所以实现成单件
typedef CSingleton<CErrorLogAdapter> ErrorLogSingleton;

#ifdef _GAME_NO_LOG_
#define ERRORLOG(format, ...)
#else
#ifndef _GAME_NO_BACKTRACE_
//!错误日志，无日志等级，强制打，打堆栈
#define ERRORLOG(format, ...) \
    TRACESVR("[ERRORLOG]"format, ##__VA_ARGS__); \
    ErrorLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_ANY, -1, \
    __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__); \
    BacktraceSingleton::Instance()->GenBacktraceInfo(); \
    ErrorLogSingleton::Instance()->m_stLogFile.WriteLog(LOG_LEVEL_ANY, "%s\n", \
    BacktraceSingleton::Instance()->GetAllBackTraceInfo())
#else
//!错误日志，无日志等级，强制打，不打堆栈
#define ERRORLOG(format, ...) \
    ErrorLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_ANY, -1, \
    __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif //_GAME_NO_BACKTRACE_
#endif

// 默认开启assert
#ifdef _GAME_NO_LOG_
#define __ASSERT_AND_LOG(X) __ASSERT_AND_LOG(X)
#else

#ifdef _ENABLE_ASSERT_
#define __ASSERT_AND_LOG(X) if(!(X)) { ERRORLOG("%s\n", #X); assert(X);}
#else
#define __ASSERT_AND_LOG(X) \
            if(!(X)) { ERRORLOG("%s\n", #X); }
#endif

#endif

// assert失败后, 返回错误值
#define ASSERT_AND_LOG_RTN_INT(X) \
            __ASSERT_AND_LOG(X); if (!(X)) return -1

#define ASSERT_AND_LOG_RTN_BOOL(X) \
            __ASSERT_AND_LOG(X); if (!(X)) return false

#define ASSERT_AND_LOG_RTN_PTR(X) \
            __ASSERT_AND_LOG(X); if (!(X)) return NULL

#define ASSERT_AND_LOG_RTN_VOID(X) \
            __ASSERT_AND_LOG(X); if (!(X)) return

#define ASSERT_AND_LOG_RTN_EX(X, iReturn) \
    __ASSERT_AND_LOG(X); if (!(X)) return (iReturn)


const int MAX_TRACE_UIN_NUMBER = 1024;

class CPlayerLogAdapter
{
public:
    CPlayerLogAdapter();
    ~CPlayerLogAdapter() {}

    int ReloadLogConfig(TLogConfig& rstLogConfig);
    int ClearTraceUinSet();
    int AddTraceUin(unsigned int uiUin);
    int WriteLog(unsigned int uiUin, const char* pszFile, int iLine,
                 const char* pszFunc, const char *pcContent, ...);

    TLogConfig& GetLogConfig();
    FILE* GetPlayerFile(unsigned int uiUin);
    FILE* GetPlayerProtoFile(unsigned int uiUin);

private:
    int CheckUin(unsigned int uiUin);

private:
    short m_shTraceUinNum;
    unsigned int m_auiTraceUin[MAX_TRACE_UIN_NUMBER];
    char m_szBaseName[MAX_FILENAME_LENGTH];

    TLogConfig m_stLogConfig;
    CLogFile m_stLogFile;
    CLogFile m_stProtoLogFile;
};

//!一般来说只会用到一个CPlayerLogAdapter类，所以实现成单件
typedef CSingleton<CPlayerLogAdapter> PlayerLogSingleton;

#ifdef _GAME_NO_LOG_
#define TRACEPLAYER(uiUin, format, ...) PlayerLogSingleton::Instance()->WriteLog(uiUin, \
    __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#else
//!错误日志，只有在TraceUin列表中的Uin才会打
#define TRACEPLAYER(uiUin, format, ...) PlayerLogSingleton::Instance()->WriteLog(uiUin, \
        __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif


#define PLAYERLOGFD PlayerLogSingleton::Instance()->m_stLogFile.GetFD
#define PLAYERLOGFP(uiUin) PlayerLogSingleton::Instance()->GetPlayerFile(uiUin)
#define PLAYERPROTOFILE(uiUin) PlayerLogSingleton::Instance()->GetPlayerProtoFile(uiUin)

#ifdef _GAME_NO_LOG_
#define CLEARTRACEDUIN()
#else
#define CLEARTRACEDUIN() PlayerLogSingleton::Instance()->ClearTraceUinSet()
#endif

#ifdef _GAME_NO_LOG_
#define ADDTRACEDUIN(uiUin)
#else
#define ADDTRACEDUIN(uiUin) PlayerLogSingleton::Instance()->AddTraceUin(uiUin)
#endif
const int MAX_TRACE_MSG_NUMBER = 1024;

class CMsgLogAdapter
{
public:
    CMsgLogAdapter();
    ~CMsgLogAdapter() {}

    int ReloadLogConfig(TLogConfig& rstLogConfig);
    int ClearTraceMsgSet();
    int AddTraceMsg(int iMsgID);
    int WriteLog(int iLogLevel, int iMsgID, const char* pszFile, int iLine,
                 const char* pszFunc, const char *pcContent, ...);
    int WriteLog(int iLogLevel, const char* pszFile, int iLine,
                 const char* pszFunc, const char *pcContent, ...);

private:
    int CheckMsg(int iMsgID);

private:
    short m_shTraceMsgNum;
    int m_aiTraceMsg[MAX_TRACE_MSG_NUMBER];
    CLogFile m_stLogFile;
};

//!一般来说只会用到一个CPlayerLogAdapter类，所以实现成单件
typedef CSingleton<CMsgLogAdapter> MsgLogSingleton;

#ifdef _GAME_NO_LOG_
#define TRACEMSG(iLogLevel, format, ...)
#else
//!错误日志，只有在TraceUin列表中的Uin才会打
#define TRACEMSG(iLogLevel, format, ...) MsgLogSingleton::Instance()->WriteLog(iLogLevel, \
        __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

#ifdef _GAME_NO_LOG_
#define TRACEMSG_EX(iLogLevel, iMsgID, format, ...)
#else
//!错误日志，只有在TraceUin列表中的Uin才会打
#define TRACEMSG_EX(iLogLevel, iMsgID, format, ...) MsgLogSingleton::Instance()->WriteLog(iLogLevel, iMsgID, \
        __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif


class CBillLogAdapter
{
public:
    CBillLogAdapter();
    ~CBillLogAdapter() {}

    int SetBillLogName(const char* pszLogName);
    int ReloadLogConfig(TLogConfig& rstLogConfig);
    int CheckBillPath();

public:
    CLogFile m_stLogFile;
    int m_iCurMonth;
};

//!一般来说只会用到一个CServerLogAdapter类，所以实现成单件
typedef CSingleton<CBillLogAdapter> BillLogSingleton;

#ifdef _GAME_NO_LOG_
#define TRACEBILL(format, ...)
#else
//!主控日志，无日志等级，强制打
#define TRACEBILL(format, ...) BillLogSingleton::Instance()->CheckBillPath(); \
        BillLogSingleton::Instance()->m_stLogFile.WriteLog(LOG_LEVEL_ANY, format, ##__VA_ARGS__)
#endif



//////////////////////////////////////////////////////////////////////////
// Thread Log
class CThreadLogAdapter
{
public:
    CThreadLogAdapter();
    ~CThreadLogAdapter() {}

    int ReloadLogConfig(TLogConfig& rstLogConfig);
    int WriteLog(int iThreadID, const char* pszFile, int iLine,
                 const char* pszFunc, const char *pcContent, ...);


private:
    char m_szBaseName[MAX_FILENAME_LENGTH];

    TLogConfig m_stLogConfig;
    CLogFile m_stLogFile;

};

//!一般来说只会用到一个CPlayerLogAdapter类，所以实现成单件
typedef CSingleton<CThreadLogAdapter> ThreadLogSingleton;

#ifdef _GAME_NO_LOG_
#define TRACETHREAD(uiUin, format, ...)
#else
//!错误日志，只有在TraceUin列表中的Uin才会打
#define TRACETHREAD(iThreadID, format, ...) ThreadLogSingleton::Instance()->WriteLog(iThreadID, \
    __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif


//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Room Log
class CRoomLogAdapter
{
public:
    CRoomLogAdapter();
    ~CRoomLogAdapter() {}

    int ReloadLogConfig(TLogConfig& rstLogConfig);
    int WriteLog(int iRoomID, const char* pszFile, int iLine,
                 const char* pszFunc, const char *pcContent, ...);


private:
    char m_szBaseName[MAX_FILENAME_LENGTH];

    TLogConfig m_stLogConfig;
    CLogFile m_stLogFile;

};

//!一般来说只会用到一个CRoomLogAdapter类，所以实现成单件
typedef CSingleton<CRoomLogAdapter> RoomLogSingleton;

#ifdef _GAME_NO_LOG_
#define TRACEROOM(iRoomID, format, ...)
#else
//!错误日志，只有在TraceUin列表中的Uin才会打
#define TRACEROOM(iRoomID, format, ...) RoomLogSingleton::Instance()->WriteLog(iRoomID, \
    __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif


//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// Client Log
class CClientLogAdapter
{
public:
    CClientLogAdapter();
    ~CClientLogAdapter() {}

    int ReloadLogConfig(TLogConfig& rstLogConfig);
    int WriteLog(int iUin, const char* pszFile, int iLine,
                 const char* pszFunc, const char *pcContent, ...);


private:
    char m_szBaseName[MAX_FILENAME_LENGTH];

    TLogConfig m_stLogConfig;
    CLogFile m_stLogFile;

};

//!一般来说只会用到一个ClientLogSingleton类，所以实现成单件
typedef CSingleton<CClientLogAdapter> ClientLogSingleton;

#ifdef _GAME_NO_LOG_
#define TRACECLIENT(iUin, format, ...)
#else
//!错误日志，只有在TraceUin列表中的Uin才会打
#define TRACECLIENT(iUin, format, ...) ClientLogSingleton::Instance()->WriteLog(iUin, \
    __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

//////////////////////////////////////////////////////////////////////////
}
#endif //__LOG_ADAPTER_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
