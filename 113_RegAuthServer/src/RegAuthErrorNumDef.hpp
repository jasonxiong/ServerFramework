#ifndef __REGAUTH_ERROR_NUM_DEF_HPP__
#define __REGAUTH_ERROR_NUM_DEF_HPP__

#include "ErrorNumDef.hpp"

enum EN_REGAUTH_MODULE_DEF
{
    EN_REGAUTH_MODULE_COMMON = 0x001,               //通用模块错误码
};

#define TREGAUTH_COMMON_BASE MAKE_ERRNO(EN_SERVER_REGAUTH, EN_REGAUTH_MODULE_COMMON, 0)
enum REGAUTH_SYSTEM_ERR_DEF
{
    T_REGAUTH_SYSTEM_PARA_ERR        = TREGAUTH_COMMON_BASE + 0x01,        //参数错误
    T_REGAUTH_SERVER_BUSY            = TREGAUTH_COMMON_BASE + 0x02,      //RegAuth服务器忙

    T_REGAUTH_ACCOUNT_EXISTS         = TREGAUTH_COMMON_BASE + 0x03,      //帐号已经存在
    T_REGAUTH_PASSWD_ERROR           = TREGAUTH_COMMON_BASE + 0x04,      //帐号密码错误

    T_REGAUTH_INVALID_OPTYPE         = TREGAUTH_COMMON_BASE + 0x05,      //非法的操作类型
};

#endif
