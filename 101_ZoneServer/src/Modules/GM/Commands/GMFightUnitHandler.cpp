
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "GameRole.hpp"
#include "GMBaseCommand.hpp"
#include "FightUnitObj.hpp"

#include "GMFightUnitHandler.hpp"

using namespace ServerLib;

CGMFightUnitHandler::CGMFightUnitHandler()
{
    m_pRoleObj = NULL;
}

//执行相应GM命令的功能
int CGMFightUnitHandler::Run(CGameRoleObj *pRoleObj, int iCommandType, std::vector<std::string> &vParams)
{
    m_pRoleObj = pRoleObj;
    if(!m_pRoleObj)
    {
        LOGERROR("Failed  to run FightUnit GM command, invalid role obj!\n");
        return -1;
    }

    //首先进行参数检查
    int iRet = SecurityCheck(iCommandType, vParams);
    if(iRet)
    {
        LOGERROR("Failed to check GM param, uin %u\n", pRoleObj->GetUin());
        return iRet;
    }

    CFightUnitManager& stFightUnitManager = m_pRoleObj->GetFightUnitManager();

    //执行相应的GM操作
    switch(iCommandType)
    {
        case GM_COMMAND_ADD_FIGHTUNIT:
            {
                //增加战斗单位
                int iFightUnitID = atoi(vParams[1].c_str());

                iRet = stFightUnitManager.AddFightUnit(iFightUnitID);
                if(iRet)
                {
                    LOGERROR("Failed to add fight unit %d, ret %d, uin %u\n", iFightUnitID, iRet, m_pRoleObj->GetUin());
                    return iRet;
                }
            }
            break;

        case GM_COMMAND_ADD_UNITATTR:
            {
                int iFightUnitID = atoi(vParams[1].c_str());
                int iAttrType = atoi(vParams[2].c_str());
                int iAddNum = atoi(vParams[3].c_str());

                CFightUnitObj* pstUnitObj = stFightUnitManager.GetFightUnitByID(iFightUnitID);
                if(pstUnitObj)
                {
                    pstUnitObj->AddFightUnitAttr(iAttrType, iAddNum);
                }
            }
            break;

        default:
            {
                return T_ZONE_SYSTEM_PARA_ERR;
            }
            break;
    }

    LOGDEBUG("Success to run reposty GM command, uin %u\n", m_pRoleObj->GetUin());

    return T_SERVER_SUCESS;
}

//进行参数检查
int CGMFightUnitHandler::SecurityCheck(int iCommandType, std::vector<std::string>& vParams)
{
    switch(iCommandType)
    {
        case GM_COMMAND_ADD_FIGHTUNIT:
            {
                if(vParams.size() != 2)
                {
                    LOGERROR("Failed to check param of AddFightUnit, param num %zu\n", vParams.size());
                    return T_ZONE_SYSTEM_PARA_ERR;
                }
            }
            break;

        case GM_COMMAND_ADD_UNITATTR:
            {
                if(vParams.size() != 4)
                {
                    LOGERROR("Failed to check param of AddUnitAttr, param num %zu\n", vParams.size());
                    return T_ZONE_SYSTEM_PARA_ERR;
                }
            }
            break;

        default:
            {
                return T_ZONE_SYSTEM_PARA_ERR;
            }
            break;
    }
    
    return T_SERVER_SUCESS;
}
