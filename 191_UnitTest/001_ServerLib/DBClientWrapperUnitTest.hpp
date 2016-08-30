#ifndef __DBCLIENT_WRAPPER_UNITTEST_HPP__
#define __DBCLIENT_WRAPPER_UNITTEST_HPP__

#include "gtest/gtest.h"
#include "DBClientWrapper.hpp"

class DBClientWrapperUnitTest : public testing::Test
{
public:
    //Sets up the test fixture
    virtual void SetUp();

    //Tears down the test fixture
    virtual void TearDown();

protected:
    DBClientWrapper m_oDBClient;
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
