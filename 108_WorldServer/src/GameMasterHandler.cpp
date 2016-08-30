#include <assert.h>
#include <string.h>

#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "LogAdapter.hpp"

#include "GameMasterHandler.hpp"

CGameMasterHandler::~CGameMasterHandler()
{

}

int CGameMasterHandler::OnClientMsg(const void* pMsg)
{
    ASSERT_AND_LOG_RTN_INT(pMsg);

    m_pRequestMsg = (GameProtocolMsg*)pMsg;

    switch (m_pRequestMsg->m_stmsghead().m_uimsgid())
    {
        //todo jasonxiong 暂时先注释掉，后续根据需要再开发
        /*
    case MSGID_GAMEMASTERDB_GRANT_REQUEST:
        {
            OnRequestGameMaster();
            break;
        }
    case MSGID_GAMEMASTERDB_GRANT_RESPONSE:
        {
            OnResponseGameMaster();
            break;
        } 
        */ 
    default:
        {
            break;
        }
    }

    return 0;
}

int CGameMasterHandler::OnRequestGameMaster()
{
    LOGDEBUG("OnRequestGameMaster\n");

    int iRet = CWorldMsgHelper::SendWorldMsgToRoleDB(*m_pRequestMsg);

    return iRet;
}


int CGameMasterHandler::OnResponseGameMaster()
{
    //todo jasonxiong 这边等后续需要添加GM处理权限时再修改添加
    /*
    LOGDEBUG("OnResponseGameMaster\n");

    GAMEMASTERDB_GRANT_RESPONSE& rstResponse = m_pRequestMsg->m_stMsgBody.m_stGameMasterDB_Grant_Response;

    const ROLEID& rstRoleID = rstResponse.m_stRoleID;

    CWorldRoleStatusWObj* pWorldRoleStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(rstRoleID);
    if (!pWorldRoleStatusObj)
    {
        return -1;
    }

    unsigned int uiZoneID = pWorldRoleStatusObj->GetZoneID();

    int iRet = CWorldMsgHelper::SendWorldMsgToWGS(*m_pRequestMsg, uiZoneID);

    return iRet;
    */

    return 0;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
