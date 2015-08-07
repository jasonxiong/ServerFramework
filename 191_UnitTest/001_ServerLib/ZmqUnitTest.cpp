#include <stdlib.h>

#include "ErrorNumDef.hpp"
#include "StringUtility.hpp"

#include "ZmqUnitTest.hpp"

using namespace ServerLib;

void ZmqBusUnitTest::SetUp()
{
    memset(m_szSendBuff, 0, sizeof(m_szSendBuff));
    memset(m_szRecvBuff, 0, sizeof(m_szRecvBuff));

    SAFE_STRCPY(m_szSendBuff, "Jason ZmqBus Testing!", sizeof(m_szSendBuff)-1);

    return;
}

void ZmqBusUnitTest::TearDown()
{
    return;
}

TEST_F(ZmqBusUnitTest, PairInProc)
{
    const char* pInProcAddr = "Bus1";

    int iRet = m_oZmqBusServer.ZmqInit(pInProcAddr, EN_ZMQ_SOCKET_PAIR, EN_ZMQ_PROC_INPROC, EN_ZMQ_SERVICE_SERVER);
    ASSERT_EQ(T_SERVER_SUCESS, iRet);

    iRet = m_oZmqBusClient.ZmqInit(pInProcAddr, EN_ZMQ_SOCKET_PAIR, EN_ZMQ_PROC_INPROC, EN_ZMQ_SERVICE_CLIENT);
    ASSERT_EQ(T_SERVER_SUCESS, iRet);

    iRet = m_oZmqBusServer.ZmqSend(m_szSendBuff, strlen(m_szSendBuff)+1, 0);
    ASSERT_EQ(T_SERVER_SUCESS, iRet);

    iRet = m_oZmqBusClient.ZmqSend(m_szSendBuff, strlen(m_szSendBuff)+1, 0);
    ASSERT_EQ(T_SERVER_SUCESS, iRet);

    iRet = m_oZmqBusServer.ZmqSend(m_szSendBuff, strlen(m_szSendBuff)+1, 0);
    ASSERT_EQ(T_SERVER_SUCESS, iRet);

    int iMsgLen = 0;
    iRet = m_oZmqBusServer.ZmqRecv(m_szRecvBuff, sizeof(m_szRecvBuff)-1, iMsgLen, 0);
    ASSERT_EQ(T_SERVER_SUCESS, iRet);

    printf("server recv from client, len %d, data %s\n", iMsgLen, m_szRecvBuff);
}

TEST_F(ZmqBusUnitTest, PairTcp)
{
    const char* pInProcAddr = "10.1.164.90:5569";

    int iRet = m_oZmqBusServer.ZmqInit(pInProcAddr, EN_ZMQ_SOCKET_PAIR, EN_ZMQ_PROC_INPROC, EN_ZMQ_SERVICE_SERVER);
    ASSERT_EQ(T_SERVER_SUCESS, iRet);

    iRet = m_oZmqBusClient.ZmqInit(pInProcAddr, EN_ZMQ_SOCKET_PAIR, EN_ZMQ_PROC_INPROC, EN_ZMQ_SERVICE_CLIENT);
    ASSERT_EQ(T_SERVER_SUCESS, iRet);

    iRet = m_oZmqBusServer.ZmqSend(m_szSendBuff, strlen(m_szSendBuff)+1, 0);
    ASSERT_EQ(T_SERVER_SUCESS, iRet);

    iRet = m_oZmqBusClient.ZmqSend(m_szSendBuff, strlen(m_szSendBuff)+1, 0);
    ASSERT_EQ(T_SERVER_SUCESS, iRet);

    iRet = m_oZmqBusServer.ZmqSend(m_szSendBuff, strlen(m_szSendBuff)+1, 0);
    ASSERT_EQ(T_SERVER_SUCESS, iRet);

    int iMsgLen = 0;
    iRet = m_oZmqBusServer.ZmqRecv(m_szRecvBuff, sizeof(m_szRecvBuff)-1, iMsgLen, ZMQ_DONTWAIT);
    ASSERT_EQ(T_SERVER_SUCESS, iRet);

    printf("server recv from client, len %d, data %s\n", iMsgLen, m_szRecvBuff);

    iRet = m_oZmqBusClient.ZmqRecv(m_szRecvBuff, sizeof(m_szRecvBuff)-1, iMsgLen, ZMQ_DONTWAIT);
    ASSERT_EQ(T_SERVER_SUCESS, iRet);

    printf("client recv from server, len %d, data %s\n", iMsgLen, m_szRecvBuff);
}
