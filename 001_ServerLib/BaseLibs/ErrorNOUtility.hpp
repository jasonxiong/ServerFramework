/**
*@file ErrorNOUtility.hpp
*@author jasonxiong
*@date 2009-11-11
*@version 1.0
*@brief 错误码工具类，将错误码分模块管理起来，并可以输出对应错误信息
*
*
*/

#ifndef __ERROR_NO_UTIL_HPP__
#define __ERROR_NO_UTIL_HPP__

namespace ServerLib
{
//!每一个模块最多允许定义的错误号个数
const int MAX_ERROR_NO_INFO_PER_MODULE = 64;

const int MAX_ERROR_MODULE_NUMBER = 128;

class CErrorNOUtility
{
    //!错误号与错误描述对应消息
    typedef struct tagErrorNOInfo
    {
        int m_iErrorNO;
        char* m_pszErrorMsg;
    } TErrorNOInfo;

    //!管理一个模块的所有错误信息的结构
    typedef struct tagErrorModuleInfo
    {
        int m_iModuleID; //!<模块ID
        int m_iErrorNONum; //!<该模块当前定义的错误号个数
        TErrorNOInfo m_aszErrorNOInfo[MAX_ERROR_NO_INFO_PER_MODULE]; //!<错误信息
    } TErrorModuleInfo;

public:
    CErrorNOUtility();
    ~CErrorNOUtility();

public:


};

}

#endif //__ERROR_NO_UTIL_HPP__
///:~
