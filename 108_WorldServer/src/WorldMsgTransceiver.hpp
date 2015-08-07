#ifndef __KYLIN_MSG_TRANSCEIVER_HPP__
#define __KYLIN_MSG_TRANSCEIVER_HPP__

#include "ZmqBus.hpp"
#include "ConfigHelper.hpp"
#include "CodeQueue.hpp"

using namespace ServerLib;

class CWorldMsgTransceiver
{
public:
    // 初始化通信的ZMQ通道
    int Initialize(bool bResumeMode);

public:
    // 发送和接收消息
    int SendOneMsg(const char* pszMsg, int iMsgLength, EGameServerID enMsgPeer, int iInstance = 1);

    //可以从连接的所有通道中轮询接收消息
    int RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, EGameServerID& enMsgPeer, int& iInstance);

private:
    ZmqBus* GetZmqBus(EGameServerID enMsgPeer, int iInstance);

private:

    //Account Server 到World之间的通信信道
    ZmqBus m_oAccount2WorldSvr;

    //Zone Server 到World之间的通信信道
    int m_iZone2WorldBusNum;
    //注意：数组下标与ZoneID对应，下表为0的位置为空，不使用
    ZmqBus m_szZone2WorldSvrs[MAX_ZONE_PER_WORLD];

    //World 到 ROLEDB之间的通信信道
    ZmqBus m_oWorld2RoleDBClient;

    //World 到 Cluster之间的通信信道
    //ZmqBus m_oWorld2ClusterClient;

    //World 到 NameDBServer 之间的通信信道
    ZmqBus m_oWorld2NameDBClient;
};

#endif

