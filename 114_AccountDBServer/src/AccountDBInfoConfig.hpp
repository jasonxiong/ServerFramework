#ifndef __ACCOUNTDB_INFO_CONFIG_HPP__
#define __ACCOUNTDB_INFO_CONFIG_HPP__

//读取ACCOUNTDB的信息文件，采用XML的文件格式

#include "GameConfigDefine.hpp"

struct ONEACCOUNTDBINFO
{
    int iDBMajorVersion;    //使用的数据库的主版本号
    int iDBMinVersion;      //使用的数据库的子版本号
    char szDBEngine[64];    //使用的DB引擎的名字
    char szDBHost[64];      //连接数据库的HOST
    char szUserName[64];    //连接数据库的用户名
    char szUserPasswd[64];  //连接数据库的密码
    char szDBName[64];      //连接的数据库的DB表名

    ONEACCOUNTDBINFO()
    {
        memset(this, 0, sizeof(*this));
    };
};

class CAccountDBInfoConfig
{
public:
    CAccountDBInfoConfig();
    ~CAccountDBInfoConfig();

    int LoadAccountDBInfoConfig(const char* pszFilePath);

    const ONEACCOUNTDBINFO* GetOneAccountDBInfoByIndex(int iDBIndex);

public:
    static const int TOTAL_ACCOUNTDB_NUMBER = GameConfig::ACCOUNT_TABLE_SPLIT_FACTOR;

private:
    
    int m_iAccountDBNumber;

    ONEACCOUNTDBINFO m_astAccountDBInfo[TOTAL_ACCOUNTDB_NUMBER];
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
