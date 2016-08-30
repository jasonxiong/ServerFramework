#include <stdlib.h>

#include "google/protobuf/text_format.h"
#include "ErrorNumDef.hpp"
#include "StringUtility.hpp"
#include "GameProtocol.hpp"

#include "DBClientWrapperUnitTest.hpp"

using namespace ServerLib;

void DBClientWrapperUnitTest::SetUp()
{
    m_oDBClient.Init(true);

    return;
}

void DBClientWrapperUnitTest::TearDown()
{
    return;
}

TEST_F(DBClientWrapperUnitTest, DBClientTest)
{
    //测试连接
    int iRet = m_oDBClient.SetMysqlDBInfo("192.168.78.144", "qmonster", "kingnetdevelop", "1_RoleDB");
    EXPECT_EQ(T_SERVER_SUCESS, iRet);

    iRet = m_oDBClient.SetMysqlDBInfo("192.168.78.144", "qmonster", "kingnetdevelop", "1_RoleDB");
    EXPECT_EQ(T_SERVER_SUCESS, iRet);

    //测试执行SQL语句
    iRet = m_oDBClient.SetMysqlDBInfo("192.168.78.144", "qmonster", "kingnetdevelop", "1_RoleDB");
   
    EXPECT_EQ(T_SERVER_SUCESS, iRet);
}

TEST_F(DBClientWrapperUnitTest, DBClientSelectTest)
{
    //建立到MYSQL数据库的链接
    int iRet = m_oDBClient.SetMysqlDBInfo("192.168.78.144", "qmonster", "kingnetdevelop", "1_RoleDB");
    EXPECT_EQ(T_SERVER_SUCESS, iRet);

    //测试拉取玩家数据
    static char szQueryString[4096] = {0};
    int iLength = SAFE_SPRINTF(szQueryString, sizeof(szQueryString)-1, "select %s from t_qmonster_userdata where uin = %d and worldID=%d", "base_info", 10001, 1);

    //玩家基本信息 base_info
    iRet = m_oDBClient.ExecuteRealQuery(szQueryString, iLength, true);
    EXPECT_EQ(T_SERVER_SUCESS, iRet);

    MYSQL_ROW pstResult = NULL;
    unsigned long* pLengths = NULL;
    unsigned int uFields = 0;

    iRet = m_oDBClient.FetchOneRow(pstResult, pLengths, uFields);
    EXPECT_EQ(T_SERVER_SUCESS, iRet);
    ASSERT_EQ(1u, uFields);

    BASEDBINFO stBaseInfo;
    bool bRet = stBaseInfo.ParseFromArray(pstResult[0], pLengths[0]);
    EXPECT_TRUE(bRet);

    EXPECT_EQ(T_SERVER_SUCESS, iRet);

    //打印protobuf数据，确认数据完整性
    std::string strTmp;
    ::google::protobuf::TextFormat::PrintToString(stBaseInfo, &strTmp);

    printf("\n\n%s\n\n\n", strTmp.c_str());

}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
