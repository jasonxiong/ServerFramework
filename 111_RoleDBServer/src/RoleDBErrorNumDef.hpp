#ifndef __ROLEDB_ERROR_NUM_DEF_HPP__
#define __ROLEDB_ERROR_NUM_DEF_HPP__

#include "ErrorNumDef.hpp"

enum EN_ROLEDB_MODULE_DEF
{
    EN_ROLEDB_MODULE_COMMON = 0x001,               //通用模块错误码
};

#define TROLEDB_COMMON_BASE MAKE_ERRNO(EN_SERVER_ROLEDB, EN_ROLEDB_MODULE_COMMON, 0)
enum ROLEDB_SYSTEM_ERR_DEF
{
    T_ROLEDB_SYSTEM_PARA_ERR         = TROLEDB_COMMON_BASE + 0x01,        //参数错误
    T_ROLEDB_CANNOT_CREATE_ROLE      = TROLEDB_COMMON_BASE + 0x02,        //不能创建角色
    T_ROLEDB_SQL_EXECUTE_FAILED      = TROLEDB_COMMON_BASE + 0x03,        //SQL语句执行失败
    T_ROLEDB_PROTOBUFF_ERR           = TROLEDB_COMMON_BASE + 0x04,        //protobuf 相关错误
    T_ROLEDB_INVALID_RECORD           = TROLEDB_COMMON_BASE + 0x04,        //非法的数据库查询记录
};

#endif
