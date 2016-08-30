#ifndef __GAME_OBJ_COMM_DEF_HPP__
#define __GAME_OBJ_COMM_DEF_HPP__

//本文件中主要定义游戏中使用的单位和角色的基本数据结构

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"

using namespace GameConfig;

//场景上单位的定义信息
struct TUNITINFO
{
    int iUnitID;                    //场景单位的ID
    unsigned char ucUnitType;       //场景单位的类型
    unsigned int uiUnitStatus;      //场景单位当前的状态

    TUNITINFO()
    {
        memset(this, 0, sizeof(*this));
    };
};

//玩家角色基本信息结构定义
struct TROLEBASEINFO
{
    char szNickName[MAX_NICK_NAME_LENGTH];  //玩家的名字
    int iLastLogin;             //玩家上次登录的时间
    int iLastLogout;            //玩家上次登出游戏的时间
    int iCreateTime;            //玩家帐号创建的时间
    int iOnlineTime;            //玩家的总在线时长
    int iLoginCount;            //玩家总的登录次数
    int iForbidTalkingTime;     //玩家聊天被禁止发言的时间
    int iLoginTime;             //玩家本次登录的时间
    int iLogoutTime;            //玩家本次登出游戏的时间
    int iBattlefieldObjIndex;   //玩家的战斗战场信息
    bool bIsSelfWin;            //玩家进行战斗胜负结果

    TROLEBASEINFO()
    {
        memset(this, 0, sizeof(*this));
    };
};

//角色的数据信息
struct TROLEINFO
{
    RoleID stRoleID;            //角色ID
    TROLEBASEINFO stBaseInfo;   //角色的基础信息
    TUNITINFO stUnitInfo;       //角色的Unit单位信息
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
