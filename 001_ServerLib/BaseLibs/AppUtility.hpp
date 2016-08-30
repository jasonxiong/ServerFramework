#ifndef __APP_UTILITY_HPP__
#define __APP_UTILITY_HPP__

#include "SignalUtility.hpp"

namespace ServerLib
{
const int APPCMD_NOTHING_TODO   = 0;          //啥也不干
const int APPCMD_STOP_SERVICE   = 1;          //停止命令
const int APPCMD_RELOAD_CONFIG  = 2;          //重读配置命令
const int APPCMD_QUIT_SERVICE   = 3;          //立刻跳出循环

class CAppUtility
{
public:
    // 启动服务器进程, 完成的功能有:
    // 1. 注册USR处理函数, 负责停止/刷新服务器
    // 2. 支持原有的参数和TCM参数, 检测是否恢复模式启动, 分析TBUS地址
    // 3. 原有的参数是-r, -w, -ins指定恢复模式和TBUS地址
    // 4. TApp模式的进程, 使用--resume和--id指定恢复模式和TBUS地址
    static void AppLaunch(int argc, char** argv,
                          Function_SignalHandler pSigHandler,
                          bool& rbResume,
                          int& riWorldID,
                          int& riInstanceID,
                          int* piZoneID = NULL,
                          char* pszHostIP = NULL,
                          int* piHostPort = NULL,
                          bool bEnableQuitSig = false);

public:
    // 初始化日志配置
    // pszConfigFile: 配置文件. NULL则使用默认配置
    // pszLogName: 日志文件名
    static void LoadLogConfig(const char* pszConfigFile, const char* pszLogName);

public:
    // 注册停止和重载配置信号处理
    static void RegisterSignalHandler(Function_SignalHandler pSigHandler, bool bEnableQuitSig);

private:
    // 打印版本
    static void ShowVersion();

    // 启动方法
    static void ShowUsage(const char* pszName);

    //检查文件锁，防止重复运行
    static void CheckLock(const char* pszLockFile);

    //初始化为守护进程的函数
    static void DaemonLaunch(void);

    //如进程正常退出，pid文件将被删除；否则为异常终止
    //进程pid可被脚本用来关闭进程、重新读取配置文件
    static void WritePidFile();

    // 清除PID文件
    static void CleanPidFile();

    // 读取PID文件
    static int ReadPidFile();

private:
    // 支持文件参数启动
    static int ReadConfigFile(const char* pszFilename, bool* pbDaemonLaunch, bool* pbResume, int *piWorldID,int *piZoneID, int *piInstance);
};
}

#endif


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
