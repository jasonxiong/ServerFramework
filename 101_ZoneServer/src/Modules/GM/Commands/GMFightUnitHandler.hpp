
#ifndef __GM_FIGHT_UNIT_HANDLER_HPP__
#define __GM_FIGHT_UNIT_HANDLER_HPP__

#include "GMBaseCommand.hpp"

class CGameRoleObj;
class CGMFightUnitHandler : public IGMBaseCommand
{
public:
    CGMFightUnitHandler();
    ~CGMFightUnitHandler() { };

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

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
