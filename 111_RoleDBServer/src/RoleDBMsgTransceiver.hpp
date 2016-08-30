
#ifndef __ROLEDB_MSG_TRANSCEIVER_HPP__
#define __ROLEDB_MSG_TRANSCEIVER_HPP__

#include "ZmqBus.hpp"
#include "ConfigHelper.hpp"

class CRoleDBMsgTransceiver
{
public:
    // 初始化ZMQ Bus系统
    int Initialize(bool bResumeMode, int iWorldID);

public:
    // 发送和接收消息
    int SendOneMsg(const char* pszMsg, int iMsgLength, EGameServerID enMsgPeer);
    int RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, EGameServerID enMsgPeer);

private:

    //World 到 RoleDB的ZMQ通信通道
    ZmqBus m_oWorld2RoleDBSvr;
};

#endif


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
