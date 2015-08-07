#ifndef __SESSION_KEY_UTILITY_UNITTEST_HPP__
#define __SESSION_KEY_UTILITY_UNITTEST_HPP__

#include "gtest/gtest.h"
#include "SessionKeyUtility.hpp"

class SessionKeyUtilityUnitTest : public testing::Test
{
public:
    //Sets up the test fixture
    virtual void SetUp();

    //Tears down the test fixture
    virtual void TearDown();

protected:
    unsigned int m_uin;
    
    int m_iSessionKeyLen;
    
    char m_szSessionKey[1024];
};

#endif
