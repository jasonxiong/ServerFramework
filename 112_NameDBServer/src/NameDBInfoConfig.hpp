#ifndef __NAMEDB_INFO_CONFIG_HPP__
#define __NAMEDB_INFO_CONFIG_HPP__

//读取NAMEDB的信息文件，采用XML的文件格式

#include "GameConfigDefine.hpp"

struct ONENAMEDBINFO
{
    int iDBMajorVersion;    //使用的数据库的主版本号
    int iDBMinVersion;      //使用的数据库的子版本号
    char szDBEngine[64];    //使用的DB引擎的名字
    char szDBHost[64];      //连接数据库的HOST
    char szUserName[64];    //连接数据库的用户名
    char szUserPasswd[64];  //连接数据库的密码
    char szDBName[64];      //连接的数据库的DB表名

    ONENAMEDBINFO()
    {
        memset(this, 0, sizeof(*this));
    };
};

class CNameDBInfoConfig
{
public:
    CNameDBInfoConfig();
    ~CNameDBInfoConfig();

    int LoadNameDBInfoConfig(const char* pszFilePath);

    const ONENAMEDBINFO* GetOneNameDBInfoByIndex(int iDBIndex);

public:
    static const int TOTAL_NAMEDB_NUMBER = GameConfig::NAME_TABLE_SPLIT_FACTOR;

private:
    
    int m_iNameDBNumber;

    ONENAMEDBINFO m_astNameDBInfo[TOTAL_NAMEDB_NUMBER];
};

#endif
