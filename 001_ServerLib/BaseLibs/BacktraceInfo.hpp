/**
*@file BacktraceInfo.hpp
*@author jasonxiong
*@date 2009-11-30
*@version 1.0
*@brief 堆栈模块
*
*	通过该文件定义的一些宏获取堆栈信息
*/

#ifndef __BACK_TRACE_INFO_HPP__
#define __BACK_TRACE_INFO_HPP__

#include "SingletonTemplate.hpp"

namespace ServerLib
{

const int MAX_BACKTRACE_SYMBOLS_NUMBER = 20; //!<最多打印的堆栈层次数
const int MAX_BACKTRACE_STRING_LENGTH = 10240; //!<堆栈打印信息的最大长度

class CBacktraceInfo
{
public:
    CBacktraceInfo();
    ~CBacktraceInfo();

public:
    /**
    *获取所有堆栈信息
    *@return 用字符串描述的堆栈信息
    */
    const char* GetAllBackTraceInfo();

    /**
    *获取指定的堆栈信息
    *@param[in]
    *@param[in]
    *@return 0 success
    */
    const char* GetBackTraceInfo(int iTraceDepth);

    /**
    *生成堆栈信息，在获取前需要生成
    *@param[in]
    *@param[in]
    *@return 0 success
    */
    int GenBacktraceInfo();

private:
    void* m_apBacktraceSymbols[MAX_BACKTRACE_SYMBOLS_NUMBER]; //!<从backtrace返回的堆栈符号地址存放在这
    char** m_apszBacktraceInfo; //!<从backtrace_symbols返回的堆栈信息
    int m_iCurTraceSymbolsNum; //!<当前堆栈符号个数
    char m_szAllBackTraceStr[MAX_BACKTRACE_STRING_LENGTH]; //!<所有堆栈的打印信息
};

//!一般来说只会用到一个CBacktraceInfo类，所以实现成单件
typedef CSingleton<CBacktraceInfo> BacktraceSingleton;

}


#endif //__BACK_TRACE_INFO_HPP__
///:~
