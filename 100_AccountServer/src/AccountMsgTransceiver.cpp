
#include "Int64Utility.hpp"
#include "LogAdapter.hpp"
#include "AppDef.hpp"
#include "AccountMsgTransceiver.hpp"

using namespace ServerLib;

CAccountMsgTransceiver::CAccountMsgTransceiver()
{

}

CAccountMsgTransceiver::~CAccountMsgTransceiver()
{

}

int CAccountMsgTransceiver::Initialize(bool bResumeMode, int iWorldID)
{
    // 初始化 Lotus 的CodeQueue
    int iRet;
    iRet = m_stCodeQueueManager.LoadCodeQueueConfig(APP_CONFIG_FILE, "Account");
    if (iRet < 0)
    {
        LOGERROR("LoadCodeQueueConfig failed, iRet:%d\n", iRet);
        return -1;
    }

    m_stCodeQueueManager.Initialize(bResumeMode);
    if (iRet < 0)
    {
        LOGERROR("Code Queue Manager Initialize failed, iRet:%d\n", iRet);
        return -2;
    }

    //初始化Account到World的ZMQ通信通道
    uint64_t ullAccountServerID = GetServerBusID(iWorldID, GAME_SERVER_ACCOUNT);
    uint64_t ullWorldServerID = GetServerBusID(iWorldID, GAME_SERVER_WORLD);

    const char* pszZmqBusAddr = GetZmqBusAddress(ullAccountServerID, ullWorldServerID);
    if(!pszZmqBusAddr)
    {
        LOGERROR("Fail to get zmq bus addr, client id %"PRIu64", server id %"PRIu64"\n", ullAccountServerID, ullWorldServerID);
        return -3;
    }

    LOGERROR("Account2World ZMQ Server Address: %s\n", pszZmqBusAddr);

    iRet = m_oZmqAccount2WorldClient.ZmqInit(pszZmqBusAddr, EN_ZMQ_SOCKET_PAIR, EN_ZMQ_PROC_TCP, EN_ZMQ_SERVICE_CLIENT);
    if(iRet)
    {
        LOGERROR("Fail to init Account to World ZMQ Bus Server, ret 0x%0x!\n", iRet);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

// 发送和接收消息
int CAccountMsgTransceiver::SendOneMsg(const char* pszMsg, int iMsgLength, EGameServerID enMsgPeer, int iInstance)
{
    ASSERT_AND_LOG_RTN_INT(pszMsg);

    int iRet = -1;

    switch (enMsgPeer)
    {
    case GAME_SERVER_WORLD:
        {
            iRet = m_oZmqAccount2WorldClient.ZmqSend(pszMsg, iMsgLength, 0);
        }
        break;

    case GAME_SERVER_LOTUSACCOUNT:
        {
            iRet = m_stCodeQueueManager.SendOneMsg(pszMsg, iMsgLength, iInstance);
        }
        break;

    default:
        {
            LOGERROR("Bug..................................\n");
            break;
        }
    }

    return iRet;
}

int CAccountMsgTransceiver::RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, EGameServerID enMsgPeer, int iInstance)
{
    ASSERT_AND_LOG_RTN_INT(pszMsg);

    int iRet = -1;

    switch (enMsgPeer)
    {
    case GAME_SERVER_WORLD:
        {
            iRet = m_oZmqAccount2WorldClient.ZmqRecv(pszMsg, iMaxOutMsgLen, riMsgLength, 0);
        }
        break;

    case GAME_SERVER_LOTUSACCOUNT:
        {
            iRet = m_stCodeQueueManager.RecvOneMsg(pszMsg, iMaxOutMsgLen, riMsgLength, iInstance);
        }
        break;

    default:
        {
            LOGERROR("Bug...............................\n");
            break;
        }
    }

    return iRet;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
