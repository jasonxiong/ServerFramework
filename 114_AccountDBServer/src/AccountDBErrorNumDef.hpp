#ifndef __ACCOUNTDB_ERROR_NUM_DEF_HPP__
#define __ACCOUNTDB_ERROR_NUM_DEF_HPP__

#include "ErrorNumDef.hpp"

enum EN_ACCOUNTDB_MODULE_DEF
{
    EN_ACCOUNTDB_MODULE_COMMON = 0x001,               //通用模块错误码
};

#define TACCOUNTDB_COMMON_BASE MAKE_ERRNO(EN_SERVER_ACCOUNTDB, EN_ACCOUNTDB_MODULE_COMMON, 0)
enum ACCOUNTDB_SYSTEM_ERR_DEF
{
    T_ACCOUNTDB_SYSTEM_PARA_ERR         = TACCOUNTDB_COMMON_BASE + 0x01,        //参数错误
    T_ACCOUNTDB_ACCOUNT_EXISTS          = TACCOUNTDB_COMMON_BASE + 0x02,        //要创建的帐号已经存在
    T_ACCOUNTDB_SQL_EXECUTE_FAILED      = TACCOUNTDB_COMMON_BASE + 0x03,        //SQL语句执行失败
    T_ACCOUNTDB_PROTOBUFF_ERR           = TACCOUNTDB_COMMON_BASE + 0x04,        //protobuf 相关错误
    T_ACCOUNTDB_INVALID_CONFIG          = TACCOUNTDB_COMMON_BASE + 0x05,        //非法的配置
    T_ACCOUNTDB_INVALID_RECORD          = TACCOUNTDB_COMMON_BASE + 0x06,        //非法的数据库记录   
    T_ACCOUNTDB_AUTH_FAILED             = TACCOUNTDB_COMMON_BASE + 0x07,        //帐号认证失败   
    T_ACCOUNTDB_PASSWD_NOT_MATCH        = TACCOUNTDB_COMMON_BASE + 0x08,        //帐号密码错误
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
