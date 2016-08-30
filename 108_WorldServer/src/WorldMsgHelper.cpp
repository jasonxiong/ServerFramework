#include <string.h>

#include "ModuleHelper.hpp"

#include "WorldMsgHelper.hpp"

GameProtocolMsg CWorldMsgHelper::m_stWorldMsg;

int CWorldMsgHelper::GenerateMsgHead(GameProtocolMsg& rstWorldMsg,
                                     unsigned int uiSessionFD, unsigned int msgID, unsigned int uiUin)
{
    GameCSMsgHead* pMsgHead = rstWorldMsg.mutable_m_stmsghead();
    pMsgHead->set_m_uimsgid((ProtocolMsgID)msgID);
    pMsgHead->set_m_uin(uiUin);
    pMsgHead->set_m_uisessionfd(uiSessionFD);

    rstWorldMsg.mutable_m_stmsgbody()->Clear();

    return 0;
}

int CWorldMsgHelper::SendWorldMsgToRoleDB(const GameProtocolMsg& rstWorldMsg)
{
    int iRet = CModuleHelper::GetWorldProtocolEngine()->SendWorldMsg(rstWorldMsg, GAME_SERVER_ROLEDB);
    if (iRet < 0)
    {
        LOGERROR("Send Failed: ret = %d\n", iRet);
    }

    return iRet;
}

int CWorldMsgHelper::SendWorldMsgToWGS(const GameProtocolMsg& rstWorldMsg, int iZoneID, bool bHomeZone)
{
    if ((!CModuleHelper::GetZoneTick()->IsZoneActive(iZoneID))
        && !bHomeZone)
    {
        LOGERROR("iZoneID:%d is dead, not send\n", iZoneID);
        return 0;
    }

    int iRet = CModuleHelper::GetWorldProtocolEngine()->SendWorldMsg(rstWorldMsg, GAME_SERVER_ZONE,iZoneID);
    if (iRet < 0)
    {
        LOGERROR("Send Failed: ret = %d\n", iRet);
    }

    return iRet;
}

int CWorldMsgHelper::SendWorldMsgToAccount(const GameProtocolMsg& rstWorldMsg)
{
    int iRet = CModuleHelper::GetWorldProtocolEngine()->SendWorldMsg(rstWorldMsg, GAME_SERVER_ACCOUNT);
    if (iRet < 0)
    {
        LOGERROR("Failed to send World msg, ret = %d\n", iRet);
    }

    return iRet;
}

int CWorldMsgHelper::SendWorldMsgToMailDB(const GameProtocolMsg& rstWorldMsg)
{
    int iRet = CModuleHelper::GetWorldProtocolEngine()->SendWorldMsg(rstWorldMsg, GAME_SERVER_MAILDB);
    if (iRet < 0)
    {
        LOGERROR("Send Failed: ret = %d\n", iRet);
    }

    return iRet;
}

int CWorldMsgHelper::SendWorldMsgToGuildDB(const GameProtocolMsg& rstWorldMsg)
{
    int iRet = CModuleHelper::GetWorldProtocolEngine()->SendWorldMsg(rstWorldMsg, GAME_SERVER_GUILDDB);
    if (iRet < 0)
    {
        LOGERROR("Send Failed: ret = %d\n", iRet);
    }

    return iRet;
}

int CWorldMsgHelper::SendWorldMsgToNameDB(const GameProtocolMsg& rstWorldMsg)
{
    int iRet = CModuleHelper::GetWorldProtocolEngine()->SendWorldMsg(rstWorldMsg, GAME_SERVER_NAMEDB);
    if (iRet < 0)
    {
        LOGERROR("Send Failed: ret = %d\n", iRet);
    }

    return iRet;
}

int CWorldMsgHelper::SendWorldMsgToCluster(const GameProtocolMsg& rstWorldMsg)
{
    //去掉Cluster
    /*
    int iRet = CModuleHelper::GetWorldProtocolEngine()->SendWorldMsg(rstWorldMsg, GAME_SERVER_CLUSTER);
    if (iRet < 0)
    {
        TRACESVR("Send Failed: ret = %d\n", iRet);
    }

    return iRet;
    */

    return 0;
}

int CWorldMsgHelper::BroadcastNewZoneConf()
{
    //todo jasonxiong 这个函数后续需要考虑详细的实现
    static GameProtocolMsg stMsg;
    GenerateMsgHead(stMsg, 0, MSGID_WORLD_ZONECHANGED_NOTIFY, 0);

    World_ZoneChanged_Notify* pstZoneChangedNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stworld_zonechanged_notify();
    pstZoneChangedNotify->set_worldid(CModuleHelper::GetWorldID());

    //CListZoneHandler::GenerateZoneList(*(pstZoneChangedNotify->mutable_stzones()));

    for (int iZoneID = 1; iZoneID < MAX_ZONE_PER_WORLD; ++iZoneID)
    {
        CWorldMsgHelper::SendWorldMsgToWGS(stMsg, iZoneID);
    }

    return 0;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
