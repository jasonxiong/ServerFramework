
#ifndef __BILL_LOG_HPP__
#define __BILL_LOG_HPP__

///////////////////////////////////////////////////////////////////////////////////// 
#include "LogAdapter.hpp"
#include "GameConfigDefine.hpp"

using namespace ServerLib;

const int MAX_BILL_LOG_LENGTH = 1024;

class CWorldRoleStatusWObj;

class CWorldBillLog
{
public:
    // 初始化
    static int Initialize();

public:
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // 日志接口

    // 接口必须要传入TEventId参数
    // 当rstEventId.m_iEventTime非0时, 表示后续的日志跟上一条日志是同一个事件, 复用相同的EventId		
    static void TraceAddMoney(const unsigned uin, int iLoginType, int64_t iAmount);
    	
    ///////////////////////////////////////////////////////////////////////////////////////////////////

private:
    static void GenerateLogHead(int iMsgID, TEventId& rstEventId, const unsigned char ucZoneID);
    static int WriteBillLog();

private:
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
