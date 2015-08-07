
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "GameRole.hpp"
#include "BattlefieldObj.hpp"
#include "ZoneMsgHelper.hpp"
#include "ZoneObjectHelper.hpp"
#include "CombatEventManager.hpp"
#include "CombatUtility.hpp"
#include "CombatUnitAI.hpp"
#include "CombatUnitObj.hpp"
#include "UnitUtility.hpp"

#include "CombatFramework.hpp"

GameProtocolMsg CCombatFramework::m_stGameMsg;

using namespace ServerLib;

CCombatFramework* CCombatFramework::Instance()
{
    static CCombatFramework* pInstance = NULL;
    if(!pInstance)
    {
        pInstance = new CCombatFramework();
    }

    return pInstance;
}

CCombatFramework::CCombatFramework()
{

}

CCombatFramework::~CCombatFramework()
{

}

//玩家开始战斗的请求
int CCombatFramework::DoCombat(CGameRoleObj& stRoleObj, const Zone_DoCombat_Request& rstRequest, bool bNeedResp)
{
    //初始化成员变量
    m_uiUin = stRoleObj.GetUin();
    m_stGameMsg.Clear();

    //判断当前是否在战斗中
    if(stRoleObj.GetBattlefieObjID() >= 0)
    {
        //已经在战斗中，返回
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_DOCOMBAT_RESPONSE);

    //拉取响应的消息体
    Zone_DoCombat_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_docombat_response();
    
    int iRet = T_SERVER_SUCESS;
    switch(rstRequest.etype())
    {
        case COMBAT_TYPE_PVE:
            {
                //PVE战斗
                iRet = InitPveBattlefield(stRoleObj, rstRequest.ilevelid());
                if(iRet)
                {
                    LOGERROR("Failed to init pve battle field, ret %d, uin %u\n", iRet, m_uiUin);
                    return iRet;
                }
            }
            break;

        case COMBAT_TYPE_PVP:
            {
                ;
            }
            break;

        default:
            {
                return -1;
            }
            break;
    }

     //填充并回包给客户端
    pstResp->set_iresult(T_SERVER_SUCESS);

    if(bNeedResp)
    {
        SendSuccessResponse();
    }

    //推送战场初始化信息
    SendBattlefieldInitNotify(stRoleObj);

    //抛出开始战斗事件
    CCombatEventManager::NotifyCombatBegin(stRoleObj);

    return T_SERVER_SUCESS;
}

//玩家主角战斗单位移动的请求
int CCombatFramework::DoCombatMove(CGameRoleObj& stRoleObj, const Zone_CombatMove_Request& rstRequest)
{
    //初始化
    m_uiUin = stRoleObj.GetUin();
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_COMBATMOVE_RESPONSE);

    //拉取响应的消息体
    Zone_CombatMove_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_combatmove_response();

    //拉取战场信息
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to do combat role unit move, no battlefield, uin %u\n", m_uiUin);

        CCombatFramework::FinCombat(stRoleObj);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //判断是否是主角移动，主角当前状态是否正确
    if((pstBattlefieldObj->GetActionUnitType()!=COMBAT_UNIT_TYPE_ROLE
       && pstBattlefieldObj->GetActionUnitType()!=COMBAT_UNIT_TYPE_PARTNER)
       ||pstBattlefieldObj->GetActionUnitStatus()!=ROLE_COMBAT_UNIT_STAT_MOVE)
    {
        LOGERROR("Failed to do combat role unit move, not role unit or invalid stat, uin %u\n", m_uiUin);

        CCombatFramework::FinCombat(stRoleObj);

        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = pstBattlefieldObj->DoCombatUnitMove(rstRequest.stpath());
    if(iRet)
    {
        LOGERROR("Failed to do combat role unit move, ret %d, uin %u\n", iRet, m_uiUin);

        //CCombatFramework::FinCombat(stRoleObj);
        return iRet;
    }

    //填充并回包给客户端
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    //抛出战斗单位开始行动事件
    CCombatEventManager::NotifyCombatUnitAction(stRoleObj);

    return T_SERVER_SUCESS;
}
    
//玩家主角战斗单位行动的请求
int CCombatFramework::DoCombatAction(CGameRoleObj& stRoleObj, const Zone_CombatAction_Request& rstRequest)    
{
    //初始化
    m_uiUin = stRoleObj.GetUin();
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_COMBATACTION_RESPONSE);

    //拉取响应的消息体
    Zone_CombatAction_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_combataction_response();

    //拉取战场信息
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to do combat role unit action, no battlefield, uin %u\n", m_uiUin);

        CCombatFramework::FinCombat(stRoleObj);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //判断是否主角行动以及行动状态
    if((pstBattlefieldObj->GetActionUnitType()!=COMBAT_UNIT_TYPE_ROLE
        && pstBattlefieldObj->GetActionUnitType()!=COMBAT_UNIT_TYPE_PARTNER)
       ||pstBattlefieldObj->GetActionUnitStatus()!=ROLE_COMBAT_UNIT_STAT_ACTION)
    {
        LOGERROR("Failed to do combat role unit action, not role unit or invalid stat, uin %u\n", m_uiUin);

        CCombatFramework::FinCombat(stRoleObj);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //判断操作放是否正确
    if(!pstBattlefieldObj->IsValidActionRole(m_uiUin))
    {
        LOGERROR("Failed to do combat role unit action, unit id %d, uin %u\n", pstBattlefieldObj->GetActionUnitID(), m_uiUin);

        CCombatFramework::FinCombat(stRoleObj);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = T_SERVER_SUCESS;
    TUNITPOSITION stTargetPos(rstRequest.stpos().iposx(), rstRequest.stpos().iposy());
    switch(rstRequest.etype())
    {
        case COMBAT_ACTION_CASTSKILL:
            {
                //对目标释放技能
                CCombatTrapObj* pstTrapObj = pstBattlefieldObj->GetTrapByPos(stTargetPos);
                if(pstTrapObj && pstBattlefieldObj->IsTrapVisibleToActionUnit(*pstTrapObj))
                {    
                    //是机关并且机关可见,攻击机关
                    iRet = pstBattlefieldObj->AttackCombatTrap(stTargetPos);
                }
                else
                {    
                    //走普通攻击流程
                    iRet = pstBattlefieldObj->DoCombatCastSkill(rstRequest.icastskillid(), stTargetPos, SKILL_USE_ACTIVE);
                }
            }
            break;

        case COMBAT_ACTION_USEITEM:
            {
                //使用物品
                iRet = pstBattlefieldObj->DoCombatUseItem(rstRequest.iitemid(), stTargetPos);
            }
            break;

        case COMBAT_ACTION_IDLE:
            {
                //待机协议中需要设置方向
                iRet = pstBattlefieldObj->SetActionUnitDirection(rstRequest.idirection());
            }
            break;

        case COMBAT_ACTION_TRIGGER_TRAP:
            {
                //触发机关
                iRet = pstBattlefieldObj->ManualTriggerTrap(stTargetPos);
            }
            break;

        case COMBAT_ACTION_ATTACK_TRAP:
            {
                //攻击机关 
                iRet = pstBattlefieldObj->AttackCombatTrap(stTargetPos);
            }
            break;

        default:
            {
                LOGERROR("Failed to do role unit action, invalid type %d, uin %u\n", rstRequest.etype(), m_uiUin);

                iRet = T_ZONE_SYSTEM_PARA_ERR;
            }
            break;
    }

    if(iRet)
    {
        LOGERROR("Failed to do role unit action, type %d, ret %d, uin %u\n", rstRequest.etype(), iRet, m_uiUin);

        CCombatFramework::FinCombat(stRoleObj);
        return iRet;
    }

    //填充并回包给客户端
    pstResp->set_etype(rstRequest.etype());
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    //抛出战斗单位行动回合结束事件
    CCombatEventManager::NotifyActionRoundEnd(stRoleObj);

    return T_SERVER_SUCESS;
}

//玩家主角
int CCombatFramework::DoUpdatePartnerAI(CGameRoleObj& stRoleObj, const Zone_UpdatePartnerAI_Request& rstRequest)
{
    //初始化
    m_uiUin = stRoleObj.GetUin();
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_UPDATEPARTNERAI_RESPONSE);

    //拉取响应的消息体
    Zone_UpdatePartnerAI_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_updatepartnerai_response();

    //拉取战场信息
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to update partner ai, no battlefield, uin %u\n", m_uiUin);

        CCombatFramework::FinCombat(stRoleObj);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //设置伙伴的AI
    int iRet = pstBattlefieldObj->SetPartnerAI(rstRequest.ipartneraiid());
    if(iRet)
    {
        LOGERROR("Failed to update partner AI, ret %d, uin %u\n", iRet, m_uiUin);

        CCombatFramework::FinCombat(stRoleObj);
        return iRet;
    }

    //填充并回包给客户端
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    //如果出手战斗单位非主角，则预先计算AI
    if(pstBattlefieldObj->GetActionUnitType() != COMBAT_UNIT_TYPE_ROLE)
    {
        iRet = CCombatUnitAI::DoActionUnitAI(stRoleObj.GetBattlefieObjID(), pstBattlefieldObj->GetActionUnitID());
        if(iRet)
        {
            LOGERROR("Failed to do action unit AI, ret %d, uin %u\n", iRet, m_uiUin);
    
            //出错,结束战斗
            CCombatFramework::FinCombat(stRoleObj);
            return iRet;
        }
    }

    //抛出战斗单位移动事件
    CCombatEventManager::NotifyCombatUnitMove(stRoleObj);

    return T_SERVER_SUCESS;
}

//发起PVP战斗的请求
int CCombatFramework::StartPVPCombat(CGameRoleObj& stRoleObj, const Zone_StartPVPCombat_Request& rstRequest)
{
    //初始化
    m_uiUin = stRoleObj.GetUin();
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_STARTPVPCOMBAT_RESPONSE);

    //拉取响应的消息体
    Zone_StartPVPCombat_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_startpvpcombat_response();

    //双方是否可以战斗
    if(!CCombatUtility::CanDoPVPCombat(m_uiUin) || !CCombatUtility::CanDoPVPCombat(rstRequest.uipassiveuin()))
    {
        LOGERROR("Failed to do pvp combat, active:passive %u:%u\n", m_uiUin, rstRequest.uipassiveuin());
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //填充并回包给客户端
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    //推送消息给对方
    static GameProtocolMsg stMsg;
    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_STARTPVPCOMBAT_NBOTIFY);

    Zone_StartPVPCombat_Notify* pstNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_startpvpcombat_notify();
    pstNotify->set_uiactiveuin(m_uiUin);
    pstNotify->set_stractivename(stRoleObj.GetNickName());

    CZoneMsgHelper::SendZoneMsgToRole(stMsg, CUnitUtility::GetRoleByUin(rstRequest.uipassiveuin()));

    return T_SERVER_SUCESS;
}

//接受PVP战斗的请求
int CCombatFramework::AcceptPVPCombat(CGameRoleObj& stRoleObj, const Zone_AcceptPVPCombat_Request& rstRequest)
{
    //初始化
    m_uiUin = stRoleObj.GetUin();
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_ACCEPTPVPCOMBAT_RESPONSE);

    //拉取响应的消息体
    Zone_AcceptPVPCombat_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_acceptpvpcombat_response();

    if(!rstRequest.bisaccept())
    {
        //是拒绝战斗
        static GameProtocolMsg stMsg;
        CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_REJECTPVPCOMBAT_NOTIFY);

        Zone_RejectPVPCombat_Notify* pstRejectNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_rejectpvpcombat_notify();
        pstRejectNotify->set_ipassiveuin(m_uiUin);
        pstRejectNotify->set_strpassivename(stRoleObj.GetNickName());

        //推送拒绝的通知给对方
        CZoneMsgHelper::SendZoneMsgToRole(stMsg, CUnitUtility::GetRoleByUin(rstRequest.uiactiveuin()));
    }
    else
    {    
        //是接受战斗

        //双方是否可以战斗
        if(!CCombatUtility::CanDoPVPCombat(m_uiUin) || !CCombatUtility::CanDoPVPCombat(rstRequest.uiactiveuin()))
        {
            LOGERROR("Failed to do pvp combat, active:passive %u:%u\n", rstRequest.uiactiveuin(), m_uiUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }
    
        //开始PVP战斗
        CGameRoleObj* pstActiveRoleObj = CUnitUtility::GetRoleByUin(rstRequest.uiactiveuin());
        if(!pstActiveRoleObj)
        {
            return T_ZONE_SYSTEM_PARA_ERR;
        }
    
        int iRet = InitPVPBattlefield(*pstActiveRoleObj, stRoleObj);
        if(iRet)
        {
            LOGERROR("Failed to init pvp battlefield, ret %d, active %u, passive %u\n", iRet, rstRequest.uiactiveuin(), m_uiUin);
            return iRet;
        }
    }

    //填充并回包给客户端
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    if(rstRequest.bisaccept())
    {
        //如果是接受战斗

        //推送战场初始化信息
        SendBattlefieldInitNotify(stRoleObj);
    
        //抛出开始战斗事件
        CCombatEventManager::NotifyCombatBegin(stRoleObj);
    }

    return T_SERVER_SUCESS;
}

//todo jasonxiong4 阵型设置重做
//设置战斗阵型的请求
int CCombatFramework::SetCombatForm(CGameRoleObj& stRoleObj, const Zone_SetCombatForm_Request& rstRequest)
{
    //初始化
    m_uiUin = stRoleObj.GetUin();
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_SETCOMBATFORM_RESPONSE);

    //拉取响应的消息体
    Zone_SetCombatForm_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_setcombatform_response();

   //获取战场
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, uin %u\n", stRoleObj.GetUin());
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = pstBattlefieldObj->SetCombatForm(m_uiUin, rstRequest.stform());
    if(iRet)
    {
        LOGERROR("Failed to set combat form, ret %d, uin %u\n", iRet, m_uiUin);
        return iRet;
    }

    //填充并回包给客户端
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    if(!pstBattlefieldObj->IsNeedSetForm())
    {
        //推送设置的阵型信息
        pstBattlefieldObj->SendCombatFormNotify();

        //所有的都已经设置完成，则开始战斗
        CCombatEventManager::NotifyRoundBegin(stRoleObj);
    }

    return T_SERVER_SUCESS;
}

//结束玩家战斗
void CCombatFramework::FinCombat(CGameRoleObj& stRoleObj)
{
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        return;
    }

    if(stRoleObj.GetUin() == pstBattlefieldObj->GetActiveUin())
    {
        //主动方出错
        pstBattlefieldObj->SetCampActiveWin(false);
    }
    else
    {
        //被动方出错
        pstBattlefieldObj->SetCampActiveWin(true);
    }

    //推送战斗结束的通知
    CCombatEventManager::NotifyCombatEnd(stRoleObj);

    return;
}

//初始化PVE战斗战场信息
int CCombatFramework::InitPveBattlefield(CGameRoleObj& stRoleObj, int iLevelID)
{
    //创建战场
    int iBattlefieldObjIndex = -1;
    CBattlefieldObj* pstBattlefieldObj = CreateBattlefieldObj(iBattlefieldObjIndex);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to create battlefied, uin %u\n", m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //初始化战场
    int iRet = pstBattlefieldObj->InitPveBattlefield(stRoleObj, iLevelID);
    if(iRet)
    {
        LOGERROR("Failed to init pve battlefield, cross id %d, ret %d, uin %u\n", iLevelID, iRet, m_uiUin);

        CCombatUtility::ClearBattlefield(iBattlefieldObjIndex);

        return iRet;
    }

    //设置玩家身上的战场信息
    stRoleObj.SetBattlefieObjID(iBattlefieldObjIndex);

    return T_SERVER_SUCESS;
}

//初始化PVP战斗战场信息
int CCombatFramework::InitPVPBattlefield(CGameRoleObj& stActiveRoleObj, CGameRoleObj& stPassiveRoleObj)
{
    //创建战场
    int iBattlefieldObjIndex = -1;
    CBattlefieldObj* pstBattlefieldObj = CreateBattlefieldObj(iBattlefieldObjIndex);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to create battlefied, uin %u\n", m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //初始化战场
    int iRet = pstBattlefieldObj->InitPVPBattlefield(stActiveRoleObj, stPassiveRoleObj);
    if(iRet)
    {
        LOGERROR("Failed to init pvp battlefield, ret %d, active uin %u, passive uin %u\n", iRet, stActiveRoleObj.GetUin(), m_uiUin);

        CCombatUtility::ClearBattlefield(iBattlefieldObjIndex);
        return iRet;
    }

    //设置玩家身上的战场信息
    stActiveRoleObj.SetBattlefieObjID(iBattlefieldObjIndex);
    stPassiveRoleObj.SetBattlefieObjID(iBattlefieldObjIndex);

    return T_SERVER_SUCESS;
}

//创建战场对象，返回CBattlefieldObj的index
CBattlefieldObj* CCombatFramework::CreateBattlefieldObj(int& iBattlefieldObjIndex)
{
    iBattlefieldObjIndex = GameType<CBattlefieldObj>::Create();
    if(iBattlefieldObjIndex < 0)
    {
        LOGERROR("Failed to create battlefield obj!\n");
        return NULL;
    }

    CBattlefieldObj* pstBattlefieldObj = GameType<CBattlefieldObj>::Get(iBattlefieldObjIndex);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj!\n");
        return NULL;
    }

    pstBattlefieldObj->SetBattlefieldObjID(iBattlefieldObjIndex);

    return pstBattlefieldObj;
}

//推送战场初始化信息
int CCombatFramework::SendBattlefieldInitNotify(CGameRoleObj& stRoleObj)
{
    static GameProtocolMsg stBattlefieldNotify;

    CZoneMsgHelper::GenerateMsgHead(stBattlefieldNotify, MSGID_ZONE_BATTLEFIELD_NOTIFY);

    Zone_Battlefield_Notify* pstNotify = stBattlefieldNotify.mutable_m_stmsgbody()->mutable_m_stzone_battlefield_notify();

    //获取当前的战场对象
    CBattlefieldObj* pstBattlefieldObj = GameType<CBattlefieldObj>::Get(stRoleObj.GetBattlefieObjID());
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, uin %u\n", m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = pstBattlefieldObj->PackBattlefiledInfo(*pstNotify);
    if(iRet)
    {
        LOGERROR("Failed to send battlefield notify, ret %d, uin %u\n", iRet, m_uiUin);
        return iRet;
    }

    //推送通知给客户端
    pstBattlefieldObj->SendNotifyToBattlefield(stBattlefieldNotify);

    return T_SERVER_SUCESS;
}

//发送处理成功的返回
int CCombatFramework::SendSuccessResponse()
{
    CGameRoleObj* pstRoleObj = GameTypeK32<CGameRoleObj>::GetByKey(m_uiUin);
    if(!pstRoleObj)
    {
        LOGERROR("Failed to get game role obj, uin %u\n", m_uiUin);
        return -1;
    }

    CZoneMsgHelper::SendZoneMsgToRole(m_stGameMsg, pstRoleObj);

    return T_SERVER_SUCESS;
}
