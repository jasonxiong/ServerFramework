#ifndef __ACCOUNT_ERROR_NUM_DEF_HPP__
#define __ACCOUNT_ERROR_NUM_DEF_HPP__

#include "ErrorNumDef.hpp"

enum EN_ACCOUNT_MODULE_DEF
{
    EN_ACCOUNT_MODULE_COMMON = 0x001,               //通用模块错误码
};

#define TACCOUNT_COMMON_BASE MAKE_ERRNO(EN_SERVER_ACCOUNT, EN_ACCOUNT_MODULE_COMMON, 0)
enum ACCOUNT_SYSTEM_ERR_DEF
{
    T_ACCOUNT_SYSTEM_PARA_ERR        = TACCOUNT_COMMON_BASE + 0x01,        //参数错误
    T_ACCOUNT_CANNOT_CREATE_ROLE     = TACCOUNT_COMMON_BASE + 0x02,        //Account Server不能创建角色
    T_ACCOUNT_ROLE_NUMLIMIT          = TACCOUNT_COMMON_BASE + 0x03,         //创建的角色数目已经达到系统上限
    T_ACCOUNT_SERVER_BUSY            = TACCOUNT_COMMON_BASE + 0x04,      //Account服务器忙
};

#endif
