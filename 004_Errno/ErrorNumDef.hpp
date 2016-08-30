#ifndef __ERROR_NUM_DEF_HPP__
#define __ERROR_NUM_DEF_HPP__

#define T_SERVER_SUCESS   0x0

//说明： 打印错误码请使用 "0x%"
//错误码的组成为 app(服务器类型, 8位) + module(模块类型，12位) + error(错误码，12位)

#define MAKE_ERRNO(app, module, error) (((app&0xFF)<<24) | ((module&0xFFF)<<12)| (error&0xFFF))

enum EN_SERVER_DEF
{
    EN_SERVER_COMMON    = 0x01,                //通用类型的错误码

    EN_SERVER_ACCOUNT   = 0x02,                //登录服务器
    EN_SERVER_GATEWAY   = 0x03,                //接入服务器
    EN_SERVER_DATABASE  = 0x04,                //数据库服务器
    EN_SERVER_ZONE      = 0x05,                 //区服务器
    EN_SERVER_WORLD     = 0x06,                 //世界服务器
    EN_SERVER_ROLEDB    = 0x07,                 //角色数据库更新服务器
    EN_SERVER_REGAUTH   = 0x08,                 //注册和认证服务器
    EN_SERVER_ACCOUNTDB = 0x09,                 //帐号数据库服务器
    EN_SERVER_NAMEDB    = 0x0a,                 //名字数据库服务器
};

enum EN_COMMON_MODULE_DEF
{
    EN_COMMON_MODULE_SYS = 0x001,               //系统错误码
};

#define TSVR_SYSTEM_BASE MAKE_ERRNO(EN_SERVER_COMMON, EN_COMMON_MODULE_SYS, 0)
enum COMMON_SYSTEM_ERR_DEF
{
    T_COMMON_SYSTEM_PARA_ERR        = TSVR_SYSTEM_BASE + 0x01,      //参数错误
    T_COMMON_SYSTEM_FUNC_ERR        = TSVR_SYSTEM_BASE + 0x02,      //系统执行错误

    T_COMMON_LOG_CHKLEVEL_ERR       = TSVR_SYSTEM_BASE + 0x03,      //打印日志等级错误
    T_COMMON_LOG_OPENFILE_ERR       = TSVR_SYSTEM_BASE + 0x04,      //打开日志文件错误

    T_COMMON_FILE_GETSTAT_ERR       = TSVR_SYSTEM_BASE + 0x05,      //获取文件的状态错误
    T_COMMON_FILE_REMOVE_ERR        = TSVR_SYSTEM_BASE + 0x06,      //删除文件错误
    T_COMMON_FILE_RENAME_ERR        = TSVR_SYSTEM_BASE + 0x07,      //重命名文件错误

    T_COMMON_ZMQBUS_CONNECT_ERR     = TSVR_SYSTEM_BASE + 0x08,      //Zmq bug连接错误
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
