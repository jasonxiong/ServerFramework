#include "Int64Utility.hpp"
#include "LogAdapter.hpp"
#include "ConfigHelper.hpp"

#include "NameDBMsgTransceiver.hpp"

using namespace ServerLib;

int CNameDBMsgTransceiver::Initialize(bool bResumeMode, int iWorldID)
{
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // 初始化ZMQ通信通道
    uint64_t ullWorldSvrID = GetServerBusID(iWorldID, GAME_SERVER_WORLD);
    uint64_t ullNameDBSvrID = GetServerBusID(iWorldID, GAME_SERVER_NAMEDB);

    const char* pszWorld2NameDBAddr = GetZmqBusAddress(ullWorldSvrID, ullNameDBSvrID);
    if(!pszWorld2NameDBAddr)
    {
        TRACESVR("Fail to get zmq bus addr, client id %"PRIu64", server id %"PRIu64"\n", ullWorldSvrID, ullNameDBSvrID);
        return -1;
    }

    TRACESVR("NameDB ZMQ Addr: %s\n", pszWorld2NameDBAddr);

    int iRet = m_oWorld2NameDBSvr.ZmqInit(pszWorld2NameDBAddr, EN_ZMQ_SOCKET_PAIR, EN_ZMQ_PROC_TCP, EN_ZMQ_SERVICE_SERVER);
    if(iRet)
    {
        TRACESVR("Init world to NameDB ZMQ Bus error, ret %d\n", iRet);
        return iRet;
    }

    return 0;
}

// 发送和接收消息
int CNameDBMsgTransceiver::SendOneMsg(const char* pszMsg, int iMsgLength, EGameServerID enMsgPeer)
{
    int iRet = 0;

    switch (enMsgPeer)
    {
    case GAME_SERVER_WORLD:
        {
            iRet = m_oWorld2NameDBSvr.ZmqSend(pszMsg, iMsgLength, 0);

            TRACESVR("Send Msg: len = %d, ret = 0x%0x\n", iMsgLength, iRet);

            break;
        }

    default:
        {
            break;
        }
    }

    return iRet;
}

int CNameDBMsgTransceiver::RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, EGameServerID enMsgPeer)
{
    int iRet = 0;

    switch (enMsgPeer)
    {
    case GAME_SERVER_WORLD:
        {
            iRet = m_oWorld2NameDBSvr.ZmqRecv(pszMsg, iMaxOutMsgLen, riMsgLength, 0);

            break;
        }

    default:
        {
            break;
        }
    }

    return iRet;
}
