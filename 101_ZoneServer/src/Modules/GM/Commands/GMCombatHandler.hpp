#ifndef __GM_COMBAT_HANDLER_HPP__
#define __GM_COMBAT_HANDLER_HPP__

#include "GMBaseCommand.hpp"

class CGameRoleObj;
class CGMCombatHandler : public IGMBaseCommand
{
public:
    CGMCombatHandler();
    ~CGMCombatHandler() { };

public:
    //执行相应GM命令的功能
    virtual int Run(CGameRoleObj *pRoleObj, int iCommandType, std::vector<std::string>& vParams);

protected:
    //进行参数检查
    virtual int SecurityCheck(int iCommandType, std::vector<std::string>& vParams);

private:
    CGameRoleObj* m_pRoleObj;
};

#endif
