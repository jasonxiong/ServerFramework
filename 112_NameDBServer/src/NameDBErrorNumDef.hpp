#ifndef __NAMEDB_ERROR_NUM_DEF_HPP__
#define __NAMEDB_ERROR_NUM_DEF_HPP__

#include "ErrorNumDef.hpp"

enum EN_NAMEDB_MODULE_DEF
{
    EN_NAMEDB_MODULE_COMMON = 0x001,               //通用模块错误码
};

#define TNAMEDB_COMMON_BASE MAKE_ERRNO(EN_SERVER_NAMEDB, EN_NAMEDB_MODULE_COMMON, 0)
enum NAMEDB_SYSTEM_ERR_DEF
{
    T_NAMEDB_SYSTEM_PARA_ERR         = TNAMEDB_COMMON_BASE + 0x01,        //参数错误
    T_NAMEDB_NAME_EXISTS             = TNAMEDB_COMMON_BASE + 0x02,        //要创建的帐号已经存在
    T_NAMEDB_SQL_EXECUTE_FAILED      = TNAMEDB_COMMON_BASE + 0x03,        //SQL语句执行失败
    T_NAMEDB_PROTOBUFF_ERR           = TNAMEDB_COMMON_BASE + 0x04,        //protobuf 相关错误
    T_NAMEDB_INVALID_CONFIG          = TNAMEDB_COMMON_BASE + 0x05,        //非法的配置
    T_NAMEDB_INVALID_RECORD          = TNAMEDB_COMMON_BASE + 0x06,        //非法的数据库记录   
    T_NAMEDB_PASSWD_NOT_MATCH        = TNAMEDB_COMMON_BASE + 0x08,        //帐号密码错误
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
