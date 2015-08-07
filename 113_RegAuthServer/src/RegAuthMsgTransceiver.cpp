
#include "Int64Utility.hpp"
#include "LogAdapter.hpp"
#include "AppDef.hpp"
#include "RegAuthMsgTransceiver.hpp"

using namespace ServerLib;

CRegAuthMsgTransceiver::CRegAuthMsgTransceiver()
{

}

CRegAuthMsgTransceiver::~CRegAuthMsgTransceiver()
{

}

int CRegAuthMsgTransceiver::Initialize(bool bResumeMode, int iWorldID)
{
    // 初始化 Lotus 的CodeQueue
    int iRet;
    iRet = m_stCodeQueueManager.LoadCodeQueueConfig(APP_CONFIG_FILE, "RegAuth");
    if (iRet < 0)
    {
        TRACESVR("LoadCodeQueueConfig failed, iRet:%d\n", iRet);
        return -1;
    }

    m_stCodeQueueManager.Initialize(bResumeMode);
    if (iRet < 0)
    {
        TRACESVR("Code Queue Manager Initialize failed, iRet:%d\n", iRet);
        return -2;
    }

    //初始化RegAuth到World的ZMQ通信通道
    uint64_t ullRegAuthServerID = GetServerBusID(0, GAME_SERVER_REGAUTH);
    uint64_t ullAccountDBServerID = GetServerBusID(0, GAME_SERVER_ACCOUNTDB);

    const char* pszZmqBusAddr = GetZmqBusAddress(ullRegAuthServerID, ullAccountDBServerID);
    if(!pszZmqBusAddr)
    {
        TRACESVR("Fail to get zmq bus addr, client id %"PRIu64", server id %"PRIu64"\n", ullRegAuthServerID, ullAccountDBServerID);
        return -3;
    }

    TRACESVR("RegAuth2World ZMQ Server Address: %s\n", pszZmqBusAddr);

    iRet = m_oZmqRegAuth2AccountDBClient.ZmqInit(pszZmqBusAddr, EN_ZMQ_SOCKET_PAIR, EN_ZMQ_PROC_TCP, EN_ZMQ_SERVICE_CLIENT);
    if(iRet)
    {
        LOGERROR("Fail to init RegAuth to World ZMQ Bus Server, ret 0x%0x!\n", iRet);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

// 发送和接收消息
int CRegAuthMsgTransceiver::SendOneMsg(const char* pszMsg, int iMsgLength, EGameServerID enMsgPeer, int iInstance)
{
    ASSERT_AND_LOG_RTN_INT(pszMsg);

    int iRet = 0;

    switch (enMsgPeer)
    {
    case GAME_SERVER_ACCOUNTDB:
        {
            iRet = m_oZmqRegAuth2AccountDBClient.ZmqSend(pszMsg, iMsgLength, 0);
        }
        break;

    case GAME_SERVER_LOTUSREGAUTH:
        {
            iRet = m_stCodeQueueManager.SendOneMsg(pszMsg, iMsgLength, iInstance);
        }
        break;

    default:
        {
            TRACESVR("Bug..................................\n");
            break;
        }
    }

    return iRet;
}

int CRegAuthMsgTransceiver::RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, EGameServerID enMsgPeer, int iInstance)
{
    ASSERT_AND_LOG_RTN_INT(pszMsg);

    int iRet = 0;

    switch (enMsgPeer)
    {
    case GAME_SERVER_ACCOUNTDB:
        {
            iRet = m_oZmqRegAuth2AccountDBClient.ZmqRecv(pszMsg, iMaxOutMsgLen, riMsgLength, 0);
        }
        break;

    case GAME_SERVER_LOTUSREGAUTH:
        {
            iRet = m_stCodeQueueManager.RecvOneMsg(pszMsg, iMaxOutMsgLen, riMsgLength, iInstance);
        }
        break;

    default:
        {
            TRACESVR("Bug...............................\n");
        }
        break;
    }

    return iRet;
}
