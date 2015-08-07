#ifndef __ZMQ_UNIT_TEST_HPP__
#define __ZMQ_UNIT_TEST_HPP__

#include "gtest/gtest.h"
#include "GameProtocol.hpp"

class ProtoUnitTest : public testing::Test
{
public:
    //Sets up the test fixture
    virtual void SetUp();

    //Tears down the test fixture
    virtual void TearDown();
};

#endif
