
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "ModuleHelper.hpp"
#include "ZoneMsgHelper.hpp"

#include "RepThingsHandler.hpp"

using namespace ServerLib;

GameProtocolMsg CRepThingsHandler::ms_stZoneMsg;

CRepThingsHandler::~CRepThingsHandler()
{

}

int CRepThingsHandler::OnClientMsg()
{
    switch (m_pRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_ZONE_REPOPERA_REQUEST:
        {
            OnRequestOperaRepItem();
            break;    
        }

    case MSGID_ZONE_GETREPINFO_REQUEST:
        {
            OnRequestGetRepInfo();
            break;
        }

    case MSGID_ZONE_WEAREQUIP_REQUEST:
        {
            OnRequestWearEquipment();
            break;
        }

    case MSGID_ZONE_SELLITEM_REQUEST:
        {
            OnRequestSellItem();
            break;
        }

    default:
        {
            break;
        }
    }

    return 0;
}

//玩家背包物品的操作
int CRepThingsHandler::OnRequestOperaRepItem()
{
    int iRet = SecurityCheck();
    if(iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        
        SendFailedResponse(MSGID_ZONE_REPOPERA_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

        return -1;
    }

    //玩家背包物品的操作
    const Zone_RepOpera_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_repopera_request();
    iRet = CRepThingsFramework::Instance()->OperaRepItem(m_pRequestMsg->m_stmsghead().m_uin(), rstRequest);
    if(iRet)
    {
        LOGERROR("Failed to opera rep item, uin %u, ret %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        SendFailedResponse(MSGID_ZONE_REPOPERA_RESPONSE, iRet, *m_pNetHead);
        return -2;
    }

    //处理成功在Framework中返回，此处不需要返回包

    return T_SERVER_SUCESS;
}

//玩家拉取背包的详细信息
int CRepThingsHandler::OnRequestGetRepInfo()
{
    int iRet = SecurityCheck();
    if(iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        
        SendFailedResponse(MSGID_ZONE_GETREPINFO_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

        return -1;
    }

    //拉取玩家的背包物品信息
    const Zone_GetRepInfo_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_getrepinfo_request();
    iRet = CRepThingsFramework::Instance()->GetRepItemInfo(m_pRequestMsg->m_stmsghead().m_uin(), rstRequest);
    if(iRet)
    {
        LOGERROR("Failed to process get rep info, uin %u, ret %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        SendFailedResponse(MSGID_ZONE_GETREPINFO_RESPONSE, iRet, *m_pNetHead);
        return -2;
    }

    //处理成功在Framework中返回，此处不需要返回包

    return T_SERVER_SUCESS;
}

//玩家穿戴战斗单位装备的操作
int CRepThingsHandler::OnRequestWearEquipment()
{
    int iRet = SecurityCheck();
    if(iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        
        SendFailedResponse(MSGID_ZONE_WEAREQUIP_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

        return -1;
    }

    //玩家穿戴战斗装备的操作
    const Zone_WearEquip_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_wearequip_request();
    iRet = CRepThingsFramework::Instance()->WearEquipment(m_pRequestMsg->m_stmsghead().m_uin(), rstRequest);
    if(iRet)
    {
        LOGERROR("Failed to process wear equipment, uin %u, ret %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        SendFailedResponse(MSGID_ZONE_WEAREQUIP_RESPONSE, iRet, *m_pNetHead);
        return -2;
    }

    //处理成功在Framework中返回，此处不需要返回包

    return T_SERVER_SUCESS;
}

//玩家出售背包物品
int CRepThingsHandler::OnRequestSellItem()
{
    int iRet = SecurityCheck();
    if(iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        
        SendFailedResponse(MSGID_ZONE_SELLITEM_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

        return -1;
    }

    //玩家出售背包物品
    const Zone_SellItem_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_sellitem_request();
    iRet = CRepThingsFramework::Instance()->SellRepItem(m_pRequestMsg->m_stmsghead().m_uin(), rstRequest);
    if(iRet)
    {
        LOGERROR("Failed to process sell item, uin %u, ret %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        SendFailedResponse(MSGID_ZONE_SELLITEM_RESPONSE, iRet, *m_pNetHead);
        return -2;
    }

    //处理成功在Framework中返回，此处不需要返回包

    return T_SERVER_SUCESS;
}

// 发送失败回复
int CRepThingsHandler::SendFailedResponse(unsigned uiMsgID, const unsigned int uiResultID, const TNetHead_V2& rstNetHead)
{
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, uiMsgID);

    switch (uiMsgID)
    {
    case MSGID_ZONE_REPOPERA_RESPONSE:
        {
            ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_repopera_response()->set_iresult(uiResultID);
        }
        break;

    case MSGID_ZONE_GETREPINFO_RESPONSE:
        {
            ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_getrepinfo_response()->set_iresult(uiResultID);
        }
        break;

    case MSGID_ZONE_WEAREQUIP_RESPONSE:
        {
            ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_wearequip_response()->set_iresult(uiResultID);
        }
        break;

    case MSGID_ZONE_SELLITEM_RESPONSE:
        {
            ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_sellitem_response()->set_iresult(uiResultID);
        }
        break;

    default:
        {
            return -1;
        }
        break;
    }

    CZoneMsgHelper::SendZoneMsgToClient(ms_stZoneMsg, rstNetHead);

    return 0;
}

