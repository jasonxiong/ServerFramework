#ifndef __ZMQ_UNIT_TEST_HPP__
#define __ZMQ_UNIT_TEST_HPP__

#include "gtest/gtest.h"
#include "ZmqBus.hpp"

class ZmqBusUnitTest : public testing::Test
{
public:
    //Sets up the test fixture
    virtual void SetUp();

    //Tears down the test fixture
    virtual void TearDown();

protected:
    ZmqBus m_oZmqBusServer;
    ZmqBus m_oZmqBusClient;

    char m_szSendBuff[512];
    char m_szRecvBuff[512];
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
