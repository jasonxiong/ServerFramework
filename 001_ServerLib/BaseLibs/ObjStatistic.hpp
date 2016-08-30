
#ifndef __OBJ_STATISTIC_HPP__
#define __OBJ_STATISTIC_HPP__


#include "Statistic.hpp"
#include "SingletonTemplate.hpp"

namespace ServerLib
{
typedef struct tagObjectStatInfo
{
    int m_iObjectType;
    int m_iObjectIndex;
} TObjectStatInfo;

enum enmStatObjectTypeIndex
{
    ESOTI_OBJECT_ITEM_RESERVER1,
    ESOTI_OBJECT_ITEM_RESERVER2,
    ESOTI_OBJECT_ITEM_RESERVER3,
    ESOTI_OBJECT_ITEM_RESERVER4,
    ESOTI_OBJECT_ITEM_RESERVER5,
    ESOTI_OBJECT_ITEM_RESERVER6,
    ESOTI_OBJECT_ITEM_RESERVER7,
    ESOTI_OBJECT_ITEM_RESERVER8,
    ESOTI_OBJECT_ITEM_RESERVER9,
    ESOTI_OBJECT_ITEM_RESERVER10,
    ESOTI_OBJECT_ITEM_RESERVER11,
    ESOTI_OBJECT_ITEM_RESERVER12,
    ESOTI_OBJECT_ITEM_RESERVER13,
    ESOTI_OBJECT_ITEM_RESERVER14,
    ESOTI_OBJECT_ITEM_RESERVER15,
    ESOTI_OBJECT_ITEM_RESERVER16,
    ESOTI_OBJECT_ITEM_RESERVER17,
    ESOTI_OBJECT_ITEM_RESERVER18,
    ESOTI_OBJECT_ITEM_RESERVER19,
    ESOTI_OBJECT_ITEM_RESERVER20,
    ESOTI_OBJECT_ITEM_RESERVER21,
    ESOTI_OBJECT_ITEM_RESERVER22,
    ESOTI_OBJECT_ITEM_RESERVER23,
    ESOTI_OBJECT_ITEM_RESERVER24,
    ESOTI_OBJECT_ITEM_RESERVER25,
    ESOTI_OBJECT_ITEM_RESERVER26,
    ESOTI_OBJECT_ITEM_RESERVER27,
    ESOTI_OBJECT_ITEM_RESERVER28,
    ESOTI_OBJECT_ITEM_RESERVER_MAX,
};

enum enmStatObjectItemIndex
{
    ESOII_OBJECT_ITEM_NUMBER = 0,
    ESOII_OBJECT_ITEM_RESERVER_MAX
};

extern const char* DEFAULT_OBJECT_STAT_DIRECTORY_PATH; //!<对象统计文件所存放文件夹
extern const char* DEFAULT_OBJECT_STAT_FILE_NAME; //!<对象统计文件名

const int MAX_STAT_OBJECT_NAME_LENGTH = 64;
extern char g_aszObjectName[ESOTI_OBJECT_ITEM_RESERVER_MAX][MAX_STAT_OBJECT_NAME_LENGTH];

extern const char* g_apszObjectItemName[ESOII_OBJECT_ITEM_RESERVER_MAX];

class CObjStatistic
{
public:
    CObjStatistic(void);
public:
    ~CObjStatistic(void);

public:
    /**
    *初始化，在初始化时会分配内存给CStatistic类中的Section
    *@param[in] pszStatPath 统计文件路径，默认是../stat/
    *@param[in] pszStatFileName 统计文件名，默认是s
    *@return 0 success
    */
    int Initialize(const char* pszStatPath = NULL, const char* pszStatFileName = NULL);

    // 设置统计名称
    void SetObjectStatName(const char** apszObjectName, const int iObjectNumber);

    // 添加某对象统计
    int AddObjectStat(int iObjectType, int iObjectNumber);

    //!打印统计信息
    void Print();

    //!清空统计信息
    void Reset();

    //!在接口返回错误时，调用这个函数获取错误号
    int GetErrorNO() const
    {
        return m_iObjectErrorNO;
    }

    // 设置配置信息
    int ReloadLogConfig(TLogConfig& rstLogConfig);

private:

    int AddObjectStatInfo(int iObjectType);

    TObjectStatInfo* GetObjectStatInfo(int iObjectType);

    //!设置错误号
    void SetErrorNO(int iErrorNO)
    {
        m_iObjectErrorNO = iErrorNO;
    }

private:

    CStatistic m_stObjectStatistic;
    int m_iObjectErrorNO; //!错误码

    short m_shObjectTypeNum;
    TObjectStatInfo m_astObjectTypeInfo[ESOTI_OBJECT_ITEM_RESERVER_MAX];


};


//!一般来说只会用到一个CObjStatistic类，所以实现一个单件方便使用
typedef CSingleton<CObjStatistic> ObjectStatisticSingleton;

}

#endif


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
