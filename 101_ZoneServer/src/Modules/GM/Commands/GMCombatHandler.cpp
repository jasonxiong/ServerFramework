
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "GameRole.hpp"
#include "GMBaseCommand.hpp"
#include "BattlefieldObj.hpp"
#include "CombatUtility.hpp"

#include "GMCombatHandler.hpp"

using namespace ServerLib;

CGMCombatHandler::CGMCombatHandler()
{
    m_pRoleObj = NULL;
}

//执行相应GM命令的功能
int CGMCombatHandler::Run(CGameRoleObj *pRoleObj, int iCommandType, std::vector<std::string> &vParams)
{
    m_pRoleObj = pRoleObj;
    if(!m_pRoleObj)
    {
        LOGERROR("Failed  to run scene GM command, invalid role obj!\n");
        return -1;
    }

    //首先进行参数检查
    int iRet = SecurityCheck(iCommandType, vParams);
    if(iRet)
    {
        LOGERROR("Failed to check GM param, uin %u\n", pRoleObj->GetUin());
        return iRet;
    }

    //执行相应的GM操作
    switch(iCommandType)
    {
        case GM_COMMAND_ADD_COMBATUNIT:
            {
                //增加战场战斗单位
                int iCampType = atoi(vParams[1].c_str());
                int iMonsterID = atoi(vParams[2].c_str());
                int iPosX = atoi(vParams[3].c_str());
                int iPosY = atoi(vParams[4].c_str());
                int iDirection = atoi(vParams[5].c_str());

                CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(*m_pRoleObj);
                if(pstBattlefieldObj)
                {
                    int iUnitID = -1;
                    pstBattlefieldObj->AddCombatUnit(iCampType, iMonsterID, iPosX, iPosY, iDirection, COMBAT_UNIT_TYPE_PARTNER, iUnitID);
                }
            }
            break;

        default:
            {
                return T_ZONE_SYSTEM_PARA_ERR;
            }
            break;
    }

    LOGDEBUG("Success to run combat GM command, uin %u\n", m_pRoleObj->GetUin());

    return T_SERVER_SUCESS;
}

//进行参数检查
int CGMCombatHandler::SecurityCheck(int iCommandType, std::vector<std::string>& vParams)
{
    switch(iCommandType)
    {
        case GM_COMMAND_ADD_COMBATUNIT:
            {
                if(vParams.size() != 6)
                {
                    LOGERROR("Failed to check param of AddCombatUnit, param num %zu\n", vParams.size());
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
