#ifndef __GM_COMMAND_MANAGER_HPP__
#define __GM_COMMAND_MANAGER_HPP__

#include <string>

#include "GMBaseCommand.hpp"

#include "GMReposityHandler.hpp"
#include "GMFightUnitHandler.hpp"
#include "GMCombatHandler.hpp"

//最多支持100个GM Command
#define MAX_GM_COMMAND_HANDLER_NUM  500

class CGMCommandManager
{
public:

    static CGMCommandManager* Instance();

    //初始化GM命令管理器
    int Init();

    //查找GM命令处理的Handler
    IGMBaseCommand* GetCommandHandler(const std::string& strCommand);

    int GetHandlerType(const std::string& strCommand);

    ~CGMCommandManager();

private:
    CGMCommandManager();

    void RegisterGMCommand(std::string strCommand, IGMBaseCommand* pstCommandHandler);

private:
    static CGMCommandManager* m_pInstance;

    IGMBaseCommand* m_astGMCommandHandler[MAX_GM_COMMAND_HANDLER_NUM];

    //背包相关的GM工具
    CGMReposityHandler m_stReposityHandler;

    //战斗单位相关的GM工具
    CGMFightUnitHandler m_stFightUnitHandler;

    //玩家战斗相关
    CGMCombatHandler m_stCombatHandler;
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
