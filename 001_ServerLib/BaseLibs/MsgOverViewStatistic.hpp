#ifndef __MSG_OVERVIEW_STATISTIC_HPP__
#define __MSG_OVERVIEW_STATISTIC_HPP__


#include "SingletonTemplate.hpp"

namespace ServerLib
{
class CMsgOverViewStatistic
{

public:

    CMsgOverViewStatistic(void);

public:

    ~CMsgOverViewStatistic(void);

public:

    void ClearAllStat();

    int RecordAllStat(int iIntervalTime);

    inline void AddMsgNumberFromClient(int iMsgNumber, int iPacketSize)
    {
        m_iMsgNumberFromClient += iMsgNumber;
        m_iMsgSizeFromClient += iPacketSize;
    };

    inline void AddMsgNumberFromServer(int iMsgNumber, int iPacketSize)
    {
        m_iMsgNumberFromServer += iMsgNumber;
        m_iMsgSizeFromServer += iPacketSize;
    };


    inline void AddMsgNumberToClient(int iMsgNumber, int iPacketSize)
    {
        m_iMsgNumberToClient += iMsgNumber;
        m_iMsgSizeToClient += iPacketSize;
    };

    inline void AddMsgNumberToServer(int iMsgNumber, int iPacketSize)
    {
        m_iMsgNumberToServer += iMsgNumber;
        m_iMsgSizeToServer += iPacketSize;
    };

    inline void AddLoopNumber(int iLoopNumber)
    {
        m_iLoopNumber += iLoopNumber;
    }

public:

    // 来自客户端的消息数和包大小
    int m_iMsgNumberFromClient;
    int m_iMsgSizeFromClient;

    // 来自服务器的消息数和包大小
    int m_iMsgNumberFromServer;
    int m_iMsgSizeFromServer;

    // 发送给客户端的消息数和包大小
    int m_iMsgNumberToClient;
    int m_iMsgSizeToClient;

    // 发送给服务器的消息数和包大小
    int m_iMsgNumberToServer;
    int m_iMsgSizeToServer;

    // 程序Loop循环
    int m_iLoopNumber;
};

typedef CSingleton<CMsgOverViewStatistic> MsgOverViewStatisticSingleton;

}


#endif



