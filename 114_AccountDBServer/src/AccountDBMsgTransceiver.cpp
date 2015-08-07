
#include "Int64Utility.hpp"
#include "LogAdapter.hpp"
#include "ConfigHelper.hpp"

#include "AccountDBMsgTransceiver.hpp"

using namespace ServerLib;

int CAccountDBMsgTransceiver::Initialize(bool bResumeMode, int iWorldID)
{
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // 初始化ZMQ通信通道
    uint64_t llRegAuthSvrID = GetServerBusID(0, GAME_SERVER_REGAUTH);
    uint64_t ullAccountDBSvrID = GetServerBusID(0, GAME_SERVER_ACCOUNTDB);

    const char* pszRegAuth2AccountDBAddr = GetZmqBusAddress(llRegAuthSvrID, ullAccountDBSvrID);
    if(!pszRegAuth2AccountDBAddr)
    {
        TRACESVR("Fail to get zmq bus addr, client id %"PRIu64", server id %"PRIu64"\n", llRegAuthSvrID, ullAccountDBSvrID);
        return -1;
    }

    TRACESVR("AccountDB ZMQ Addr: %s\n", pszRegAuth2AccountDBAddr);

    int iRet = m_oRegauth2AccountDBSvr.ZmqInit(pszRegAuth2AccountDBAddr, EN_ZMQ_SOCKET_PAIR, EN_ZMQ_PROC_TCP, EN_ZMQ_SERVICE_SERVER);
    if(iRet)
    {
        TRACESVR("Init regauth to AccountDB ZMQ Bus error, ret %d\n", iRet);
        return iRet;
    }

    return 0;
}

// 发送和接收消息
int CAccountDBMsgTransceiver::SendOneMsg(const char* pszMsg, int iMsgLength, EGameServerID enMsgPeer)
{
    int iRet = 0;

    switch (enMsgPeer)
    {
    case GAME_SERVER_REGAUTH:
        {
            iRet = m_oRegauth2AccountDBSvr.ZmqSend(pszMsg, iMsgLength, 0);

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

int CAccountDBMsgTransceiver::RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, EGameServerID enMsgPeer)
{
    int iRet = 0;

    switch (enMsgPeer)
    {
    case GAME_SERVER_REGAUTH:
        {
            iRet = m_oRegauth2AccountDBSvr.ZmqRecv(pszMsg, iMaxOutMsgLen, riMsgLength, 0);

            break;
        }

    default:
        {
            break;
        }
    }

    return iRet;
}
