
#include "Int64Utility.hpp"
#include "LogAdapter.hpp"
#include "ConfigHelper.hpp"

#include "RoleDBMsgTransceiver.hpp"

using namespace ServerLib;

int CRoleDBMsgTransceiver::Initialize(bool bResumeMode, int iWorldID)
{
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // 初始化ZMQ通信通道
    uint64_t ullWorldSvrID = GetServerBusID(iWorldID, GAME_SERVER_WORLD);
    uint64_t ullRoleDBSvrID = GetServerBusID(iWorldID, GAME_SERVER_ROLEDB);

    const char* pszWorld2RoleDBAddr = GetZmqBusAddress(ullWorldSvrID, ullRoleDBSvrID);
    if(!pszWorld2RoleDBAddr)
    {
        LOGERROR("Fail to get zmq bus addr, client id %"PRIu64", server id %"PRIu64"\n", ullWorldSvrID, ullRoleDBSvrID);
        return -1;
    }

    TRACESVR("RoleDB ZMQ Addr: %s\n", pszWorld2RoleDBAddr);

    int iRet = m_oWorld2RoleDBSvr.ZmqInit(pszWorld2RoleDBAddr, EN_ZMQ_SOCKET_PAIR, EN_ZMQ_PROC_TCP, EN_ZMQ_SERVICE_SERVER);
    if(iRet)
    {
        LOGERROR("Init World to RoleDB ZMQ Bus error, ret %d\n", iRet);
        return iRet;
    }

    return 0;
}

// 发送和接收消息
int CRoleDBMsgTransceiver::SendOneMsg(const char* pszMsg, int iMsgLength, EGameServerID enMsgPeer)
{
    int iRet = 0;

    switch (enMsgPeer)
    {
    case GAME_SERVER_WORLD:
        {
            iRet = m_oWorld2RoleDBSvr.ZmqSend(pszMsg, iMsgLength, 0);

            LOGERROR("Send Msg: len = %d, ret = 0x%0x\n", iMsgLength, iRet);

            break;
        }

    default:
        {
            break;
        }
    }

    return iRet;
}

int CRoleDBMsgTransceiver::RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, EGameServerID enMsgPeer)
{
    int iRet = 0;

    switch (enMsgPeer)
    {
    case GAME_SERVER_WORLD:
        {
            iRet = m_oWorld2RoleDBSvr.ZmqRecv(pszMsg, iMaxOutMsgLen, riMsgLength, 0);

            break;
        }

    default:
        {
            break;
        }
    }

    return iRet;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
