#ifndef __ACCOUNT_OSS_LOG_HPP__
#define __ACCOUNT_OSS_LOG_HPP__

///////////////////////////////////////////////////////////////////////////////////// 
#include "LogAdapter.hpp"
#include "GameConfigDefine.hpp"

using namespace ServerLib;

class CGameRoleObj;

enum TAccountOssLogType
{
    OSS_LOG_TYPE_CREATE_ACCOUNT = 50001,    //创建帐号的日志
};

//OSS运营系统日志工具类

class CAccountOssLog
{
public:
    // 初始化
    static int Initialize();

public:
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //Account日志打印接口

    //记录创建角色相关日志
    static void TraceCreateAccount(unsigned int uin, unsigned int uiSessionFd);
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
