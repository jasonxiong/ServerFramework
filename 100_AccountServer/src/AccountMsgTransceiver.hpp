#ifndef __KYLIN_MSG_TRANSCEIVER_HPP__
#define __KYLIN_MSG_TRANSCEIVER_HPP__

#include "ZmqBus.hpp"

#include "SingletonTemplate.hpp"
#include "CodeQueueManager.hpp"
#include "ConfigHelper.hpp"

using namespace ServerLib;

const int MAX_FD_NUMBER = 1000000;
class CAccountMsgTransceiver
{
private:
    friend class CSingleton<CAccountMsgTransceiver>;
    CAccountMsgTransceiver();

public:
    virtual ~CAccountMsgTransceiver();

   // 初始化CodeQueue和ZMQ通信通道
    int Initialize(bool bResumeMode, int iWorldID);

public:
    // 发送和接收消息
    int SendOneMsg(const char* pszMsg, int iMsgLength, EGameServerID enMsgPeer, int iInstance = 0);
    int RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, EGameServerID enMsgPeer, int iInstance = 0);

    int GetCodeQueueNum() const
    {
        return m_stCodeQueueManager.GetCodeQueueNum();
    }

private:

    //内网通信部分，使用ZMQ进行通信，这个是Account到World的通道
    ZmqBus m_oZmqAccount2WorldClient;

    // 与Lotus之间还用CodeQueue通信
    CCodeQueueManager m_stCodeQueueManager;
};

#endif

