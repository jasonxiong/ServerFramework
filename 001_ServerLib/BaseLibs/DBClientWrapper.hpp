#ifndef __DB_CLIENT_WRAPPER_HPP__
#define __DB_CLIENT_WRAPPER_HPP__

#include <stdlib.h>
#include <string.h>

#include <mysql/mysql.h>

#if MYSQL_VERSION_ID >= 4000
#define mysql_connect(m,h,u,p) mysql_real_connect((m),(h),(u),(p),NULL,0,NULL,0)
#endif

//MYSQL 数据库的连接
struct MysqlConnection
{
    struct MysqlConnection* pNext;

    char szHostAddress[64];
    char szUserName[64];
    char szUserPasswd[128];

    char szDBName[64];

    MYSQL stMySql;

    bool bDBIsConnect; 

    MysqlConnection()
    {
        memset(this, 0 ,sizeof(*this));
    };
};

struct MysqlDBLinkedList
{
    MysqlConnection stMysqlConnection;
    MysqlConnection* pstCurMysqlConnection;

    MysqlDBLinkedList()
    {
        memset(this, 0, sizeof(*this));
    };
};

class DBClientWrapper
{
public:

    DBClientWrapper();

    ~DBClientWrapper();

    void Init(bool bMultiDBConn);

    int SetMysqlDBInfo(const char* pszHostAddr, const char* pszUserName, const char* pszPasswd, const char* pszDBName);

    int CloseMysqlDB();

    int CloseCurMysqlDB();

    MYSQL& GetCurMysqlConn();

    int ExecuteRealQuery(const char* pszQuery, int iLength, bool bIsSelect);

    int ExecuteRealQuery(const char* pszQuery, int iLength, bool bIsSelect, int& iErrNo);

    int FreeResult();

    //返回select到的单行的结果，返回pstResult结果，pLengths返回数组中每个字段的长度，uFields单行返回字段的数量
    int FetchOneRow(MYSQL_ROW& pstResult, unsigned long*& pLengthes, unsigned int& uFields);

    int GetAffectedRows();
    int GetNumberRows();

    //获取上次插入的AUTO_INCREMENT返回的ID值
    unsigned GetLastInsertID();

    const char* GetDBErrString() { return m_szErrString; };

private:

     MysqlDBLinkedList m_stDBLinkedList;    //操作的MYSQL handler的链表

     MYSQL_RES* m_pstRes;           //当前操作的RecordSet的内容

     int m_iResNum;                 //当前操作的RecordSet的数目

     bool m_bMultiDBConn;           //是否支持多个MYSQL连接

     char m_szErrString[1024];      //执行SQL语句过程中产生的错误
};

#endif
