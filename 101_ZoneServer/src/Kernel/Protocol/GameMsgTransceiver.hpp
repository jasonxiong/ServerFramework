
#ifndef __GAME_MSG_TRANSCEIVER_HPP__
#define __GAME_MSG_TRANSCEIVER_HPP__

// 封装CodeQueue/zmq等通道的操作, 统一化消息的收取和发送界面
#include "ConfigHelper.hpp"
#include "CodeQueueManager.hpp"
#include "ZmqBus.hpp"

using namespace ServerLib;

const int MAX_FD_NUMBER = 1000000;

class CGameMsgTransceiver
{
public:
    // 初始化所有的资源, 包括CodeQueue, zmq等
    int Initialize(bool bResumeMode);

public:
    // 发送和接收消息
    int SendOneMsg(const char* pszMsg, int iMsgLength, EGameServerID enMsgPeer, int iInstanceID = 0);
    int RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, EGameServerID enMsgPeer, int iInstanceID = 0);

	int GetCodeQueueNum() const
	{
		return m_stCodeQueueManager.GetCodeQueueNum();
	}

private:

    //ZoneServer 和 LotusServer之间通过CodeQueue通信
	CCodeQueueManager m_stCodeQueueManager;
	
private:

    //ZoneServer 和 WorldServer之间通过ZMQ进行通信
    ZmqBus m_oZone2WorldClient;
};

#endif

