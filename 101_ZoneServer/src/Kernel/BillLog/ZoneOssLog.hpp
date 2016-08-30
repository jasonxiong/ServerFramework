#ifndef __ZONE_OSS_LOG_HPP__
#define __ZONE_OSS_LOG_HPP__

///////////////////////////////////////////////////////////////////////////////////// 
#include "LogAdapter.hpp"
#include "GameConfigDefine.hpp"
#include "ModuleHelper.hpp"

using namespace ServerLib;

class CGameRoleObj;

//OSS运营系统日志工具类

//todo jasonxiong 目前测试期间，直接输出到服务器本地目录
//todo jasonxiong 等后面对接平台再修改为对应的平台

//输出运营日志的类型
enum TOssLogType
{
    OSS_LOG_TYPE_INVALID = 0,           //非法的日志操作类型
    OSS_LOG_TYPE_LOGIN = 10001,         //玩家登录帐号
    OSS_LOG_TYPE_LOGOUT = 10002,        //玩家登出帐号
    OSS_LOG_TYPE_DOCOMBAT = 10003,      //玩家进行战斗的日志
    OSS_LOG_TYPE_ITEMTRADE = 10004,     //玩家的物品交易日志
    OSS_LOG_TYPE_ADDLIFESKILL = 10005,  //玩家学习生活技能配方的日志
    OSS_LOG_TYPE_BEGINMINIGAME = 10006, //玩家开始小游戏的日志
    OSS_LOG_TYPE_ENDMINIGAME = 10007,   //玩家结束小游戏的日志
    OSS_LOG_TYPE_UNITFEED = 10008,      //玩家小伙伴冲层的日志
    OSS_LOG_TYPE_FIGHTUNIT = 10009,     //玩家战斗单位记录的日志
};

//物品交易的操作类型
enum TItemTradeType
{
    ITEM_TRADE_TYPE_BUY = 1,        //购买物品
    ITEM_TRADE_TYPE_SELL = 2,       //出售物品
};

class CZoneOssLog
{
public:
    // 初始化
    static int Initialize();

public:
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //Zone日志打印接口

    //记录登录相关日志
    static void TraceLogin(CGameRoleObj& stRoleObj);
    static void TraceLogout(CGameRoleObj& stRoleObj);

    //记录战斗的日志
    static void TraceRoleCombat(unsigned uin, int iCrossID, int iStartTime, bool bIsSelfWin);

    //玩家NPC交易日志
    static void TraceItemTrade(unsigned uin, int iOpType, int iResChange, int iItemID, int iItemNum);

    //玩家学习生活技能配方的日志
    static void TraceAddLifeSkill(unsigned uin, int iSkillType, int iSkillID, int iAddTime);

    //玩家开始小游戏的日志
    static void TraceBeginMiniGame(unsigned uin, int iMiniGameType);

    //玩家结束小游戏的日志
    static void TraceEndMiniGame(unsigned uin, int iMiniGameType);

    //玩家小伙伴冲层的日志
    static void TraceUnitFeed(unsigned uin, int iUnitConfigID, int iNewLevel);
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
