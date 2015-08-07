/**
*@file LogFile.hpp
*@author jasonxiong
*@date 2009.08.20
*@version 1.0
*@brief 简单日志类
*
*   1. 简单日志记录文件。
*   2. 日志文件名中插入当前日期。
*   3. 支持日志按文件序号备份，保留最后n份日志文件。例如：run.log.07, run.log.08, run.log.09。
*   4. 当前最新运行日志在原run.log当中 (文件名可以指定）。
*   5. 多进程最好按pid写入不同日志，例如：run.5432.log。
*/

#ifndef  __LOG_FILE_HPP__
#define  __LOG_FILE_HPP__

//在这添加标准库头文件
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

//在这添加ServerLib头文件
#include "CommonDef.hpp"
#include "ErrorDef.hpp"

namespace ServerLib
{
const int DEFAULT_CHECK_SIZE_STEPS = 2000; //!<默认多少次打印日志后检查文件大小
const int DEFAULT_MAX_FILE_SIZE = 0x10000000; //!<默认时，单个日志文件的最大大小
const int DEFAULT_MAX_FILE_COUNT = 5; //!<默认时，日志文件最多可以滚动个数
const int MAX_DUMP_HEX_BUF_LENGTH = 32 * 1024; //!<打印十六进制时传入的Buf最大长度
const int MAX_INDENT_DISTANCE = 5; //!<最大缩进的距离（Tab的个数）
const int DEFAULT_CHECK_FILE_INTERVAL = 60; //!<默认隔多少秒检查文件是否存在
const int DEFAULT_CHECK_CLEANUP_INTERVAL = 24 * 3600;//!<默认隔多少秒检查日志文件清理

/**
*@brief 在日志文件名增加日期后缀的类型
*
*   取值必须为以下枚举中的一个值
*/
typedef enum enmAddDateSuffixType
{
    EADST_DATE_NONE = 0, //!<不增加日期后缀
    EADST_DATE_YMD = 1, //!<增加YYYYmmdd格式的日期名
    EADST_DATE_YMDH = 2, //!<增加YYYYmmddHH格式的日期名
} ENMADDDATESUFFIXTYPE;

/**
*@brief TLogConfig结构中m_shLogType的取值
*
*   取值必须为以下枚举中的其中一个值
*/
typedef enum enmLogType
{
    ELT_LOG_USE_FPRINTF = 0, //!<每次WriteLog时都直接使用fprintf打印
    ELT_LOG_AND_FFLUSH = 1, //!<每次WriteLog时都直接使用fprintf打印，并调用fflush函数立即写到文件中，一般用于写账单这些关键数据
    ELT_LOG_AND_FCLOSE = 2, //!<每次WriteLog时都直接使用fprintf打印，并调用fclose函数关闭文件，为防止文件被删除
} ENMLOGTYPE;

/**
*@brief TLogConfig结构中m_iLogLevel的取值
*
*   取值必须为以下枚举中的其中一个值
*/
typedef enum enmLogLevel
{
    LOG_LEVEL_NONE      = 0x00000000,   //不打印日志
    LOG_LEVEL_DEBUG     = 0x00000001,   //DEBUG日志
    LOG_LEVEL_INFO      = 0x00000002,   //INFO日志
    LOG_LEVEL_DETAIL    = 0x00000004,   //DETAIL日志
    LOG_LEVEL_WARNING   = 0x00000008,   //WARNING日志
    LOG_LEVEL_CALL      = 0x00000010,   //函数调用日志
    LOG_LEVEL_ERROR     = 0x00000020,   //错误日志
    LOG_LEVEL_ANY       = 0xFFFFFFFF,   //强制打印
} ENMLOGLEVEL;

/**
*@brief 日志相关配置
*
*   日志相关配置，在CLogFile中使用，调用WriteLog函数根据配置来打印日志
*/
typedef struct tagLogConfig
{
    char m_szPath[MAX_FILENAME_LENGTH]; //!<日志文件所在路径
    char m_szBaseName[MAX_FILENAME_LENGTH]; //!<日志文件基名
    char m_szExtName[MAX_FILENAME_LENGTH]; //!<日志文件扩展名
    int m_iAddDateSuffixType; //!<在文件名后增加日期后缀的类型
    short m_shLogType; //!<打印日志类型
    int m_iLogLevel; //!<打印日志等级
    int m_iChkSizeSteps; //!<在调用一定打印次数后检查日志文件大小
    int m_iChkFileInterval; //!<检查文件是否存在的时间间隔，主要为了防止文件被删除
    int m_iMaxFileSize; //!<每个日志文件最大的大小
    int m_iMaxFileCount; //!<最多滚动的日志文件个数
    int m_iMaxFileExistDays;//!<日志保留天数
} TLogConfig;

class CLogFile
{

public:
    CLogFile();
    virtual ~CLogFile();

    /**
    *简单配置日志信息
    *@param[in] pszLogPath 日志文件路径
    *@param[in] pszLogName 日志名
    *@param[in] iLogLevel 日志等级
    *@return 0 success
    */
    int Initialize(const char* pszLogPath, const char* pszLogName, int iLogLevel);

    /**
    *初始化日志详细配置
    *@param[in] rstLogConfig 日志配置
    *@return 0 success
    */
    int Initialize(const TLogConfig& rstLogConfig);

    /**
    *获取日志配置
    *@return TLogConfig 返回当前日志配置
    */
    const TLogConfig& GetLogConfig()
    {
        return m_stLogConfig;
    }

    /**
    *关闭日志文件
    *@return 0 success
    */
    int CloseFile();

    /**
    *使用变长参数来打印日志，类似fprintf
    *@param[in] iLoglevel 日志等级
    *@param[in] pcContent 日志格式串
    *@return 0 success
    */
    int WriteLog(int iLoglevel, const char *pcContent, ...);

    /**
    *使用va来打印日志，类似vfprintf
    *@param[in] iLoglevel 日志等级
    *@param[in] pcContent 日志格式串
    *@return 0 success
    */
    int WriteLogVA(int iLogLevel, const char *pcContent, va_list& ap);

    /**
    *变长参数打印日志扩展函数
    *@param[in] iLoglevel 日志等级
    *@param[in] pszFile 调用打印日志函数的文件名
    *@param[in] iLine 调用打印日志函数的行号
    *@param[in] pszFunc 调用打印日志函数的函数名
    *@param[in] pcContent 日志格式串
    *@return 0 success
    */
    int WriteLogEx(int iLoglevel, int iIndentDistance, const char* pszFile, int iLine, const char* pszFunc, const char *pcContent, ...) __attribute__((format(printf, 7, 8)));

    /**
    *va参数打印日志扩展函数
    *@param[in] iLoglevel 日志等级
    *@param[in] pszFile 调用打印日志函数的文件名
    *@param[in] iLine 调用打印日志函数的行号
    *@param[in] pszFunc 调用打印日志函数的函数名
    *@param[in] pcContent 日志格式串
    *@return 0 success
    */
    int WriteLogExVA(int iLoglevel, int iIndentDistance, const char* pszFile, int iLine, const char* pszFunc, const char *pcContent, va_list& ap);

    /**
    *打印十六进制
    *@param[in] pcBuffer 传入的十六进制串
    *@param[in] iLength 字节串个数
    *@return 0 success
    */
    int DumpHex(char *pcBuffer, int iLength);

    //!在接口返回错误时，调用这个函数获取错误号
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

    //!获取文件指针
    FILE* GetFilePointer()
    {
        OpenLogFile();
        return m_pFile;
    }

    //获取文件套接字
    int GetFD() const;

    //!打开日志文件
    int OpenLogFile();

private:
    //!检查写日志等级
    int CheckWriteLogLevel(int iLogLevel);
    //!备份日志
    int BackupLog();
    //!获取真实日志文件名
    int GetRealLogName(int iLogIndex, char* szRealName, int iNameSize);

    //!检查日志路径
    int CheckPath();
    //!设置错误号
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

    //!清理日志文件
    void CleanupLogFile();

    const char* GetLogLevelStr(int iLogLevel);

private:
    int m_iErrorNO; //!错误码
    TLogConfig m_stLogConfig; //!<日志文件配置

    FILE* m_pFile; //!<文件指针
    char m_szCurLogName[MAX_FILENAME_LENGTH]; //!<当前打开的日志文件名

    int m_iLogCount; //!<打印日志的次数
    time_t m_tLastChkFileTime; //!<上一次打印日志的时间
    time_t m_tLastCleanupFileTime;//!<上一次清理日志的时间
    int m_iLastLogHour; //!<上一次打印日志的小时，每小时重新打开一次文件

    //extended function, insert __FILE__, __LINE__, __FUNCTION__ automatically
    int m_iLine; //!<调用打印日志函数的行号
    const char* m_pszFile; //!<调用打印日志函数的文件名
    const char* m_pszFunc; //!<调用打印日志函数的函数名
    int m_iIndentDistance; //!<缩进距离
    bool m_bEnableFLF; //!<是否打印函数、行号、文件等信息
};

}

#endif /* __LOG_FILE_HPP__ */
///:~
