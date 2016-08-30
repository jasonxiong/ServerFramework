#ifndef __ZONE_ERROR_NUM_DEF_HPP__
#define __ZONE_ERROR_NUM_DEF_HPP__

#include "ErrorNumDef.hpp"

enum EN_ZONE_MODULE_DEF
{
    EN_ZONE_MODULE_COMMON = 0x001,               //通用模块错误码
};

#define TZONE_COMMON_BASE MAKE_ERRNO(EN_SERVER_ZONE, EN_ZONE_MODULE_COMMON, 0)
enum ZONE_SYSTEM_ERR_DEF
{
    T_ZONE_SYSTEM_PARA_ERR         = TZONE_COMMON_BASE + 0x01,        //参数错误
    T_ZONE_PROTOBUFF_ERR           = TZONE_COMMON_BASE + 0x02,        //protobuf 错误
    T_ZONE_LOGINSERVER_FAILED      = TZONE_COMMON_BASE + 0x03,        //登录区服务器错误
    T_ZONE_LOGOUT_KICKOFF          = TZONE_COMMON_BASE + 0x04,        //踢人下线错误
    T_ZONE_SESSION_EXISTS_ERR      = TZONE_COMMON_BASE + 0x05,        //玩家的Session已经存在
    T_ZONE_LOGOUT_UNACTIVE         = TZONE_COMMON_BASE + 0x06,        //踢已经不活跃的玩家下线
    T_ZONE_FIN_PVEFIGHT_FAILED     = TZONE_COMMON_BASE + 0x07,        //完成PVE关卡战斗失败
    T_ZONE_SYSTEM_INVALID_CFG      = TZONE_COMMON_BASE + 0x08,        //非法的配置

    T_ZONE_GAMEROLE_NOT_EXIST      = TZONE_COMMON_BASE + 0x09,        //玩家角色信息不存在
    T_ZONE_GAMEHERO_NOT_EXIST      = TZONE_COMMON_BASE + 0x0a,        //卡牌信息不存在
                                                                      
    T_ZONE_SECURITY_CHECK_FAILED   = TZONE_COMMON_BASE + 0x0b,        //传入参数安全校验错误
    T_ZONE_GM_COMMAND_FAILED       = TZONE_COMMON_BASE + 0x0c,        //传入参数安全校验错误

    T_ZONE_USER_REP_FULL           = TZONE_COMMON_BASE + 0x0d,        //玩家背包仓库满
    T_ZONE_INVALID_EQUIP_TYPE      = TZONE_COMMON_BASE + 0x0e,        //非法的装备类型
    T_ZONE_REACH_LEVEL_MAX         = TZONE_COMMON_BASE + 0x0f,        //达到等级上限

    T_ZONE_PVE_KILLMONSTER_FAILED  = TZONE_COMMON_BASE + 0x10,        //PVE击杀怪物失败
    T_ZONE_INVALID_PVE_CROSS       = TZONE_COMMON_BASE + 0x11,        //非法的PVE战斗关卡

    T_ZONE_INVALID_EQUIPMENT       = TZONE_COMMON_BASE + 0x12,        //非法的装备

    T_ZONE_REVIVAL_HERO_FAILED     = TZONE_COMMON_BASE + 0x13,        //关卡英雄复活失败

    T_ZONE_INVALID_PROPS_NUM       = TZONE_COMMON_BASE + 0x14,        //游戏道具非法的数量
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
