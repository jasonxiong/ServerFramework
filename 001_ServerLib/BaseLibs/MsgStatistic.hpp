/**
*@file MsgStatistic.hpp
*@author jasonxiong
*@date 2009-11-20
*@version 1.0
*@brief 消息统计类
*
*
*/

#ifndef __MSG_STATISTIC_HPP__
#define __MSG_STATISTIC_HPP__

#include "Statistic.hpp"
#include "SingletonTemplate.hpp"

namespace ServerLib
{
const int MAX_STAT_MSG_NUMBER = 4096; //最多统计的消息种类数
const int MAX_STAT_MSG_TYPE = 4; // 最多统计消息种类
const int MAX_STAT_LOOP_STAT = 1;

//!用于通过MsgID方便定位到在CStatistic中的Section索引
typedef struct tagStatMsgInfo
{
    int m_iMsgID; //!<消息ID
    int m_iMsgIndex; //!<消息在CStaitistic中的Section索引
} TStatMsgInfo;


typedef struct tagStatMsgTypeInfo
{
    int m_iMsgType; //!<消息类型
    int m_iMsgTypeIndex; //!<消息在CStaitistic中的Section索引
} TStatMsgTypeInfo;

typedef enum enmStatMsgType
{
    ESMT_FROM_CLIENT = 0, // 从客户端收到消息
    ESMT_FROM_SERVER = 1, // 从服务器收到消息
    ESMT_TO_CLIENT = 2,   // 发送给客户端
    ESMT_TO_SERVER = 3,   // 发送给服务器
} ENMSTATMSGTYPE;

typedef enum enmStatMsgResult
{
    ESMR_SUCCEED = 0, //!<消息处理成功
    ESMR_FAILED = 1, //!<消息处理失败
    ESMR_TIMEOUT = 2, //!<消息处理超时
} ENMSTATMSGRESULT;

/**
*@brief 消息需要统计的统计项
*
*
*/
typedef enum enmStatMsgItemIndex
{
    ESMI_SUCCESS_MSG_NUMBER_IDX = 0, //!<消息处理成功的个数
    ESMI_FAILED_MSG_NUMBER_IDX, //!<消息处理失败的个数
    ESMI_TIMEOUT_MSG_NUMBER_IDX, //!<消息处理超时的个数
    ESMI_SUM_PROCESSTIME_IDX, //!<总处理耗时
    ESMI_MAX_PROCESSTIME_IDX, //!<最大处理耗时
    ESMI_AVA_PROCESSTIME_IDX, //!<平均处理耗时
    ESMI_MSG_LENGTH,  // 消息长度
    ESMI_MAX_ITEM_NUMBER //!<总共需要统计的统计项个数，保证这个值不会超过Section能容纳的Item最大值
} ENMSTATMSGITEMINDEX;

typedef enum enmStatMsgTypeItemIndex
{
    ESMTI_TOTAL_MSG_NUMBER = 0, // 消息个数
    ESMTI_TOTAL_SUCCESS_MSG_NUMBER, // 成功的消息个数
    ESMTI_TOTAL_FAILED_MSG_NUMBER, // 失败的消息个数
    ESMTI_TOTAL_TIMEOUT_NUMBER, // 超时的消息个数
    ESMTI_MAX_ITEM_NUMBER //!<总共需要统计的统计项个数，保证这个值不会超过Section能容纳的Item最大值
} ENMSTATMSGTYPEITEMINDEX;

typedef enum enmStatLoopItemIndex
{
    ESLI_LOOP_NUMBER = 0,  // Loop个数
    ESLI_MAX_ITEM_NUMBER //!<总共需要统计的统计项个数，保证这个值不会超过Section能容纳的Item最大值
} ENMSTATLOOPITEMINDEX;

extern const char* DEFAULT_MSG_STAT_DIRECTORY_PATH; //!<消息统计文件所存放文件夹
extern const char* DEFAULT_MSG_STAT_FILE_NAME; //!<消息统计文件名
extern const char* g_apszMsgItemName[ESMI_MAX_ITEM_NUMBER];
extern const char* g_apszMsgTypeItemName[ESMTI_MAX_ITEM_NUMBER];
extern const char* g_apszLoopItemName[ESLI_MAX_ITEM_NUMBER];

class CMsgStatistic
{
public:
    CMsgStatistic();
    ~CMsgStatistic();

public:
    /**
    *初始化，在初始化时会分配内存给CStatistic类中的Section
    *@param[in] pszStatPath 统计文件路径，默认是../stat/
    *@param[in] pszStatFileName 统计文件名，默认是s
    *@return 0 success
    */
    int Initialize(const char* pszStatPath = NULL, const char* pszStatFileName = NULL);

    //!添加统计信息
    int AddMsgStat(int iMsgID, short shResult, int iMsgLength, timeval tvProcessTime, int iMsgType, int iMsgTimes = 1);

    // 添加Loop统计信息
    int AddLoopStat(int iLoopNumber);

    //!打印统计信息
    void Print();

    //!清空统计信息
    void Reset();

    //!在接口返回错误时，调用这个函数获取错误号
    int GetErrorNO() const
    {
        return m_iMsgErrorNO;
    }

    // 设置配置信息
    int ReloadLogConfig(TLogConfig& rstLogConfig);

private:
    //!通过MsgID寻找到对应的TStatMsgInfo结构
    TStatMsgInfo* GetStatMsgInfo(int iMsgID);

    TStatMsgTypeInfo* GetStatMsgTypeInfo(int iMsgType);

    //!增加MsgID到统计
    int AddMsgInfo(int iMsgID);

    // 添加MsgType统计
    int AddMsgTypeInfoStat(int iMsgType, short shResult, int iMsgTimes = 1);

    // 增加MsgType到统计
    int AddMsgTypeInfo(int iMsgID);

    //!设置错误号
    void SetErrorNO(int iErrorNO)
    {
        m_iMsgErrorNO = iErrorNO;
    }

private:

    CStatistic m_stMsgStatistic;
    int m_iMsgErrorNO; //!错误码

    short m_shMsgNum;
    TStatMsgInfo m_astMsgInfo[MAX_STAT_MSG_NUMBER];

    CStatistic m_stMsgTypeStatistic;

    short m_shMsgTypeNum;
    TStatMsgTypeInfo m_astMsgTypeInfo[MAX_STAT_MSG_TYPE];

    CStatistic m_stLoopStatistic;
    bool m_bAddedLoopFlag;
};

//!一般来说只会用到一个CMsgStatistic类，所以实现一个单件方便使用
typedef CSingleton<CMsgStatistic> MsgStatisticSingleton;

}

#endif //__MSG_STATISTIC_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
