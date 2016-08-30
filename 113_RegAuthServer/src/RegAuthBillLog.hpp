
#ifndef __BILL_LOG_HPP__
#define __BILL_LOG_HPP__

/////////////////////////////////////////////////////////////////////////////////////
#include "LogAdapter.hpp"

using namespace ServerLib;

#define OSS_LOG_UID "kingnetregauth"

const int MAX_BILL_LOG_LENGTH = 1024;

class CRegAuthBillLog
{
public:
    // 初始化
    static int Initialize();

public:
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // 日志接口

    // 接口必须要传入TEventId参数
    // 当rstEventId.m_iEventTime非0时, 表示后续的日志跟上一条日志是同一个事件, 复用相同的EventId

    //玩家注册帐号
    static void TraceRegAccount(const char* szAccountID, int iAccountType, unsigned uin, int iGender, const char* szPhone, 
                                unsigned uClientIP, TEventId& rstEventID);

    //玩家登录认证帐号
    static void TraceAuthAccount(const char* szAccountID, int iAccountType, unsigned uin, TEventId& rstEventID);

    //玩家修改帐号信息
    static void TraceUpdateAccount(const char* szAccountID, int iAccountType, unsigned int uin, TEventId& rstEventID);

    ///////////////////////////////////////////////////////////////////////////////////////////////////

private:
    static void GenerateLogHead(TEventId& rstEventId);
    static int WriteBillLog();

private:
    static char m_szSendBuff[1024*5];

    // 本地Bill日志
    static CBillLogAdapter m_stBillLog;

private:
    // 一个Zone内事件发生的流水号
    static const int MAX_BILL_EVENT_PER_SECOND;
    static int m_iLastEventTime;
    static unsigned short m_usEventSequence;
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
