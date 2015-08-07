
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "GameRole.hpp"
#include "GMBaseCommand.hpp"

#include "GMReposityHandler.hpp"

using namespace ServerLib;

CGMReposityHandler::CGMReposityHandler()
{
    m_pRoleObj = NULL;
}

//执行相应GM命令的功能
int CGMReposityHandler::Run(CGameRoleObj *pRoleObj, int iCommandType, std::vector<std::string> &vParams)
{
    m_pRoleObj = pRoleObj;
    if(!m_pRoleObj)
    {
        LOGERROR("Failed  to run reposity GM command, invalid role obj!\n");
        return -1;
    }

    //首先进行参数检查
    int iRet = SecurityCheck(iCommandType, vParams);
    if(iRet)
    {
        LOGERROR("Failed to check GM param, uin %u\n", pRoleObj->GetUin());
        return iRet;
    }

    CRepThingsManager& stRepThingsManager = m_pRoleObj->GetRepThingsManager();

    //执行相应的GM操作
    switch(iCommandType)
    {
        case GM_COMMAND_ADD_REPITEM:
            {
                //背包增加物品
                int iItemID = atoi(vParams[1].c_str());
                int iItemNum = atoi(vParams[2].c_str());

                iRet = stRepThingsManager.AddRepItem(iItemID, iItemNum);
                if(iRet)
                {
                    LOGERROR("Failed to add rep item, ret %d, uin %u\n", iRet, pRoleObj->GetUin());
                    return iRet;
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
int CGMReposityHandler::SecurityCheck(int iCommandType, std::vector<std::string>& vParams)
{
    if(iCommandType == GM_COMMAND_ADD_REPITEM)
    {
        if(vParams.size() != 4)
        {
            LOGERROR("Failed to check param of AddRepItem, param num %zu\n", vParams.size());
            return T_ZONE_SYSTEM_PARA_ERR;
        }
    }

    return T_SERVER_SUCESS;
}
