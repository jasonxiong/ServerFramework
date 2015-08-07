
#include "GameProtocol.hpp"
#include "ZoneErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "ZoneMsgHelper.hpp"
#include "CombatFramework.hpp"
#include "GameRole.hpp"
#include "ModuleHelper.hpp"
#include "UnitUtility.hpp"

#include "CombatHandler.hpp"

GameProtocolMsg CCombatHandler::ms_stZoneMsg;

CCombatHandler::~CCombatHandler()
{

}

int CCombatHandler::OnClientMsg()
{
    switch (m_pRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_ZONE_DOCOMBAT_REQUEST:
        {
            OnRequestBeginCombat();
            break;    
        }

    case MSGID_ZONE_COMBATMOVE_REUQEST:
        {
            OnRequestMovePosition();
            break;
        }

    case MSGID_ZONE_COMBATACTION_REQUEST:
        {
            OnRequestCombatAction();
            break;
        }

    case MSGID_ZONE_UPDATEPARTNERAI_REQUEST:
        {
            OnRequestUpdatePartnerAI();
        }
        break;

    case MSGID_ZONE_STARTPVPCOMBAT_REQUEST:
        {
            OnRequestStartPVPCombat();
        }
        break;

    case MSGID_ZONE_ACCEPTPVPCOMBAT_REQUEST:
        {
            OnRequestAcceptPVPCombat();
        }
        break;

    case MSGID_ZONE_SETCOMBATFORM_REQUEST:
        {
            OnRequestSetCombatForm();
        }
        break;

    default:
        {
            break;
        }
    }

    return 0;
}

//处理开始战斗的请求
int CCombatHandler::OnRequestBeginCombat()
{
    int iRet = SecurityCheck();
    if(iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        
        SendFailedResponse(MSGID_ZONE_DOCOMBAT_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

        return -1;
    }

    //玩家开始战斗的请求
    const Zone_DoCombat_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_docombat_request();
    iRet = CCombatFramework::Instance()->DoCombat(*m_pRoleObj, rstRequest);
    if(iRet)
    {
        LOGERROR("Failed to do game combat, uin %u, ret %d\n", m_pRoleObj->GetUin(), iRet);
        SendFailedResponse(MSGID_ZONE_DOCOMBAT_RESPONSE, iRet, *m_pNetHead);
        return -2;
    }

    //处理成功在Framework中返回，此处不需要返回包

    return T_SERVER_SUCESS;
}

//处理主角移动的请求
int CCombatHandler::OnRequestMovePosition()
{
    int iRet = SecurityCheck();
    if(iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        
        SendFailedResponse(MSGID_ZONE_COMBATMOVE_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

        return -1;
    }

    //战斗主角移动位置的操作
    const Zone_CombatMove_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_combatmove_request();
    iRet = CCombatFramework::Instance()->DoCombatMove(*m_pRoleObj, rstRequest);
    if(iRet)
    {
        LOGERROR("Failed to do combat unit move, uin %u, ret %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        SendFailedResponse(MSGID_ZONE_COMBATMOVE_RESPONSE, iRet, *m_pNetHead);
        return -2;
    }

    //处理成功在Framework中返回，此处不需要返回包

    return T_SERVER_SUCESS;
}

//处理主角行动的请求
int CCombatHandler::OnRequestCombatAction()
{
    int iRet = SecurityCheck();
    if(iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        
        SendFailedResponse(MSGID_ZONE_COMBATACTION_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

        return -1;
    }

    //主角战斗单位行动的请求
    const Zone_CombatAction_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_combataction_request();
    iRet = CCombatFramework::Instance()->DoCombatAction(*m_pRoleObj, rstRequest);
    if(iRet)
    {
        LOGERROR("Failed to do role combat action, uin %u, ret %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        SendFailedResponse(MSGID_ZONE_COMBATACTION_RESPONSE, iRet, *m_pNetHead);
        return -2;
    }

    //处理成功在Framework中返回，此处不需要返回包

    return T_SERVER_SUCESS;
}

//更新伙伴AI的请求
int CCombatHandler::OnRequestUpdatePartnerAI()
{
    int iRet = SecurityCheck();
    if(iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        
        SendFailedResponse(MSGID_ZONE_UPDATEPARTNERAI_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

        return -1;
    }

    //更新伙伴AI的请求
    const Zone_UpdatePartnerAI_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_updatepartnerai_request();
    iRet = CCombatFramework::Instance()->DoUpdatePartnerAI(*m_pRoleObj, rstRequest);
    if(iRet)
    {
        LOGERROR("Failed to update partner ai, uin %u, ret %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        SendFailedResponse(MSGID_ZONE_UPDATEPARTNERAI_RESPONSE, iRet, *m_pNetHead);
        return -2;
    }

    //处理成功在Framework中返回，此处不需要返回包

    return T_SERVER_SUCESS;
}

//发起PVP战斗的请求
int CCombatHandler::OnRequestStartPVPCombat()
{
    int iRet = SecurityCheck();
    if(iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        
        SendFailedResponse(MSGID_ZONE_STARTPVPCOMBAT_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

        return -1;
    }

    //处理发起PVP战斗的请求
    const Zone_StartPVPCombat_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_startpvpcombat_request();
    iRet = CCombatFramework::Instance()->StartPVPCombat(*m_pRoleObj, rstRequest);
    if(iRet)
    {
        LOGERROR("Failed to start pvp combat, uin %u, ret %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        SendFailedResponse(MSGID_ZONE_STARTPVPCOMBAT_RESPONSE, iRet, *m_pNetHead);
        return -2;
    }

    //处理成功在Framework中返回，此处不需要返回包

    return T_SERVER_SUCESS;
}

//接受PVP战斗的请求
int CCombatHandler::OnRequestAcceptPVPCombat()
{
    int iRet = SecurityCheck();
    if(iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        
        SendFailedResponse(MSGID_ZONE_ACCEPTPVPCOMBAT_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

        return -1;
    }

    //处理接受PVP战斗
    const Zone_AcceptPVPCombat_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_acceptpvpcombat_request();
    iRet = CCombatFramework::Instance()->AcceptPVPCombat(*m_pRoleObj, rstRequest);
    if(iRet)
    {
        LOGERROR("Failed to accept pvp combat, uin %u, ret %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        SendFailedResponse(MSGID_ZONE_ACCEPTPVPCOMBAT_RESPONSE, iRet, *m_pNetHead);
        return -2;
    }

    //处理成功在Framework中返回，此处不需要返回包

    return T_SERVER_SUCESS;
}

//设置战斗阵型的请求
int CCombatHandler::OnRequestSetCombatForm()
{
    int iRet = SecurityCheck();
    if(iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        
        SendFailedResponse(MSGID_ZONE_SETCOMBATFORM_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

        return -1;
    }

    //处理设置战斗阵型的请求
    const Zone_SetCombatForm_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_setcombatform_request();
    iRet = CCombatFramework::Instance()->SetCombatForm(*m_pRoleObj, rstRequest);
    if(iRet)
    {
        LOGERROR("Failed to set combat form, uin %u, ret %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        SendFailedResponse(MSGID_ZONE_SETCOMBATFORM_RESPONSE, iRet, *m_pNetHead);
        return -2;
    }

    //处理成功在Framework中返回，此处不需要返回包

    return T_SERVER_SUCESS;
}

// 发送失败回复
int CCombatHandler::SendFailedResponse(unsigned uiMsgID, const unsigned int uiResultID, const TNetHead_V2& rstNetHead)
{
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, uiMsgID);

    switch (uiMsgID)
    {
    case MSGID_ZONE_DOCOMBAT_RESPONSE:
        {
            ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_docombat_response()->set_iresult(uiResultID);
        }
        break;

    case MSGID_ZONE_COMBATMOVE_RESPONSE:
        {
            ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_combatmove_response()->set_iresult(uiResultID);
        }
        break;

    case MSGID_ZONE_COMBATACTION_RESPONSE:
        {
            ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_combataction_response()->set_iresult(uiResultID);
        }
        break;

    case MSGID_ZONE_UPDATEPARTNERAI_RESPONSE:
        {
            ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_updatepartnerai_response()->set_iresult(uiResultID);
        }
        break;

    case MSGID_ZONE_STARTPVPCOMBAT_RESPONSE:
        {
            ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_startpvpcombat_response()->set_iresult(uiResultID);
        }
        break;

    case MSGID_ZONE_ACCEPTPVPCOMBAT_RESPONSE:
        {
            ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_acceptpvpcombat_response()->set_iresult(uiResultID);
        }
        break;

    case MSGID_ZONE_SETCOMBATFORM_RESPONSE:
        {
            ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_setcombatform_response()->set_iresult(uiResultID);
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
