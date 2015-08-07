#ifndef __GM_BASE_COMMAND_HPP__
#define __GM_BASE_COMMAND_HPP__

//所有GM命令功能处理的基类
#include <vector>
#include <string>

#include "GameProtocol.hpp"

enum GMCommandType
{
    GM_COMMAND_TYPE_INVALID         = 0,        //非法的GM command 类型
    GM_COMMAND_ADD_REPITEM          = 1,        //背包增加物品
    GM_COMMAND_ADD_FIGHTUNIT        = 2,        //增加玩家的战斗单位
    GM_COMMAND_ADD_UNITATTR         = 5,        //增加战斗单位的属性
    GM_COMMAND_ADD_COMBATUNIT       = 13,       //增加战场上的战斗单位
    GM_COMMAND_ADD_SKILL            = 14,       //增加指定招式
};

class CGameRoleObj;

class IGMBaseCommand
{
public:
    virtual ~IGMBaseCommand();

    //执行相应GM命令的功能
    virtual int Run(CGameRoleObj* pRoleObj, int iCommandType, std::vector<std::string>& vParams) = 0;

protected:

    //进行参数检查
    virtual int SecurityCheck(int iCommandType, std::vector<std::string>& vParams) = 0;

protected:
    static GameProtocolMsg m_stGameMsg;
};

#endif
