
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "CombatUtility.hpp"
#include "BattlefieldObj.hpp"
#include "CombatEventManager.hpp"
#include "GameRole.hpp"
#include "ZoneObjectHelper.hpp"
#include "CombatUnitAI.hpp"
#include "ZoneMsgHelper.hpp"
#include "ModuleHelper.hpp"
#include "CombatReward.hpp"
#include "DropRewardUtility.hpp"
#include "ZoneOssLog.hpp"
#include "CombatUnitObj.hpp"
#include "UnitUtility.hpp"

#include "CombatEventListener.hpp"

using namespace ServerLib;

CCombatEventListener::CCombatEventListener()
{
    m_uiUin = 0;
}

CCombatEventListener::~CCombatEventListener()
{

}

void CCombatEventListener::OnCombatBeginEvent(CGameRoleObj& stRoleObj)
{
    m_uiUin = stRoleObj.GetUin();

    //获取战场信息
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, uin %u\n", m_uiUin);

        //出错,结束战斗
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //清空回合数
    pstBattlefieldObj->SetCombatRoundNum(0);

    //todo jasonxiong4 脚本重做
    //调用战斗脚本
    //CModuleHelper::GetStoryFramework()->DoCombatScript(m_uiUin, pstBattlefieldObj->GetCrossID(), 1);

    //判断战斗胜负
    if(pstBattlefieldObj->CanEndCombat())
    {
        //能够结束战斗，抛出战斗结束事件
        CCombatEventManager::NotifyCombatEnd(stRoleObj);

        return;
    }

    if(pstBattlefieldObj->IsNeedSetForm())
    {
        //需要设置阵型,推送设置阵型的消息
        pstBattlefieldObj->NotifyRoleUnitDirective(UNIT_DIRECTIVE_SETFORM);

        pstBattlefieldObj->SetActionUnitStatus(ROLE_COMBAT_UNIT_STAT_SETFORM);
    }
    else
    {
        //抛出回合开始事件
        CCombatEventManager::NotifyRoundBegin(stRoleObj);
    }

    return;
}

void CCombatEventListener::OnCombatEndEvent(CGameRoleObj& stRoleObj)
{
    m_uiUin = stRoleObj.GetUin();

    //获取战场信息
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, uin %u\n", m_uiUin);

        //出错,结束战斗
        return;
    }

    //处理战斗结束相关信息，如奖励等
    static GameProtocolMsg stFinCombatNotify;
    CZoneMsgHelper::GenerateMsgHead(stFinCombatNotify, MSGID_ZONE_FINCOMBAT_NOTIFY);

    Zone_FinCombat_Notify* pstNotify = stFinCombatNotify.mutable_m_stmsgbody()->mutable_m_stzone_fincombat_notify();
    pstNotify->set_bisactivewin(pstBattlefieldObj->GetIsActiveWin());

    //设置玩家身上的战斗胜负结果
    unsigned uiWinUin = pstBattlefieldObj->GetIsActiveWin() ? pstBattlefieldObj->GetActiveUin() : pstBattlefieldObj->GetPassiveUin();
    unsigned uiLoseUin = pstBattlefieldObj->GetIsActiveWin() ? pstBattlefieldObj->GetPassiveUin() : pstBattlefieldObj->GetActiveUin();

    CGameRoleObj* pstWinRoleObj = CUnitUtility::GetRoleByUin(uiWinUin);
    CGameRoleObj* pstLoseRoleObj = CUnitUtility::GetRoleByUin(uiLoseUin);

    //设置玩家身上的战斗结果
    if(pstWinRoleObj)
    {
        pstWinRoleObj->SetCombatResult(true);
    }

    if(pstLoseRoleObj)
    {
        pstLoseRoleObj->SetCombatResult(false);
    }

    //给获胜方加奖励
    if(pstWinRoleObj)
    {
        CCombatReward::GetCombatReward(pstBattlefieldObj->GetCrossID(), *pstWinRoleObj, *pstNotify->mutable_streward());
    }

    pstBattlefieldObj->SendNotifyToBattlefield(stFinCombatNotify);

    //如果是PVE战斗
    if(pstBattlefieldObj->GetPassiveUin() == 0)
    {
        //保存血量和蓝量
        pstBattlefieldObj->SaveCombatUnitInfo();
    }

    //打印战斗运营日志
    CZoneOssLog::TraceRoleCombat(m_uiUin, pstBattlefieldObj->GetCrossID(), pstBattlefieldObj->GetCombatStartTime(), pstBattlefieldObj->GetIsActiveWin());

    //清理战场
    CCombatUtility::ClearBattlefield(stRoleObj.GetBattlefieObjID());

    return;
}

void CCombatEventListener::OnRoundBeginEvent(CGameRoleObj& stRoleObj)
{
    //设置uin
    m_uiUin = stRoleObj.GetUin();

    //获取战场信息
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, uin %u\n", m_uiUin);

        //出错,结束战斗
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //增加战斗的回合数
    pstBattlefieldObj->SetCombatRoundNum(pstBattlefieldObj->GetCombatRoundNum()+1);

    //初始化可行动列表
    pstBattlefieldObj->InitRoundActionUnits();

    //处理回合开始前生效的buff
    int iRet = pstBattlefieldObj->DoBuffEffectByType(BUFF_TRIGGER_ROUNDBEGIN);
    if(iRet)
    {
        LOGERROR("Failed to do round begin buff effect, ret %d, uin %u\n", iRet, m_uiUin);

        //出错,结束战斗
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //判断战斗胜负
    if(pstBattlefieldObj->CanEndCombat())
    {
        //能够结束战斗，抛出战斗结束事件
        CCombatEventManager::NotifyCombatEnd(stRoleObj);

        return;
    }

    //抛出战斗单位开始行动回合事件
    CCombatEventManager::NotifyActionRoundBegin(stRoleObj);

    return;
}

void CCombatEventListener::OnRoundEndEvent(CGameRoleObj& stRoleObj)
{
    m_uiUin = stRoleObj.GetUin();

    //获取战场信息
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefiled obj, uin  %u\n", m_uiUin);

        //出错,结束战斗
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //处理回合结束生效的buff
    int iRet = pstBattlefieldObj->DoBuffEffectByType(BUFF_TRIGGER_ROUNDEND);
    if(iRet)
    {
        LOGERROR("Failed to do round begin buff effect, ret %d, uin %u\n", iRet, m_uiUin);

        //出错,结束战斗
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //判断战斗胜负
    if(pstBattlefieldObj->CanEndCombat())
    {
        //能够结束战斗，抛出战斗结束事件
        CCombatEventManager::NotifyCombatEnd(stRoleObj);

        return;
    }

    //抛出回合开始事件，开始新的战斗回合
    CCombatEventManager::NotifyRoundBegin(stRoleObj);

    return;
}

void CCombatEventListener::OnActionRoundBeginEvent(CGameRoleObj& stRoleObj)
{
    //设置角色uin
    m_uiUin = stRoleObj.GetUin();

    //获取战场信息
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefiled obj, uin  %u\n", m_uiUin);

        //出错,结束战斗
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //判断战斗胜负
    if(pstBattlefieldObj->CanEndCombat())
    {
        //能够结束战斗，抛出战斗结束事件
        CCombatEventManager::NotifyCombatEnd(stRoleObj);

        return;
    }

    //判断当前是否有未出手单位，如果没有则抛出回合结束事件
    if(!pstBattlefieldObj->HasActionCombatUnit())
    {
        //没有未出手单位，抛出战斗回合结束事件
        CCombatEventManager::NotifyRoundEnd(stRoleObj);

        return;
    }

    //推送当前战场战斗单位出手信息
    pstBattlefieldObj->SendActionUnitNotify();

    //设置当前出手的战斗单位
    pstBattlefieldObj->SetActionCombatUnit();

    //减少单位技能CD回合
    pstBattlefieldObj->DecreaseActionSkillCDRound();

    //行动回合开始时，处理行动单位的行动回合开始技能
    int iRet = pstBattlefieldObj->DoRoundActionSkill();
    if(iRet)
    {
        LOGERROR("Failed to do round action begin skill, ret %d, uin %u\n", iRet, m_uiUin);
        
        //出错，结束战斗
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //处理战斗单位行动前生效的buff
    int iActionUnitID = pstBattlefieldObj->GetActionUnitID();
    iRet = pstBattlefieldObj->DoBuffEffectByType(BUFF_TRIGGER_ACTIONROUNDBEGIN, iActionUnitID, iActionUnitID);
    if(iRet)
    {
        LOGERROR("Failed to do action round begin buff effect, ret %d, uin %u\n", iRet, m_uiUin);

        //出错,结束战斗
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //当前出手的如果是伙伴，通知客户端更新伙伴AI
    /*
    if(pstBattlefieldObj->GetActionUnitType() == COMBAT_UNIT_TYPE_PARTNER)
    {
        pstBattlefieldObj->NotifyRoleUnitDirective(UNIT_DIRECTIVE_PARTNERAI);

        return;
    }
    */

    //获取当前行动单位
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(iActionUnitID);
    if(!pstActionUnitObj)
    {
        //出错,结束战斗
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //战斗单位是否可以开始回合
    if(pstActionUnitObj->GetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTROUND))
    {
        pstActionUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTROUND, false);

        //不能开始回合，直接结束回合
        CCombatEventManager::NotifyActionRoundEnd(stRoleObj);
        return;
    }

    //如果出手战斗单位非主角非伙伴，则预先计算AI
    if(pstBattlefieldObj->GetActionUnitType() != COMBAT_UNIT_TYPE_ROLE &&
       pstBattlefieldObj->GetActionUnitType() != COMBAT_UNIT_TYPE_PARTNER)
    {
        iRet = CCombatUnitAI::DoActionUnitAI(stRoleObj.GetBattlefieObjID(), pstBattlefieldObj->GetActionUnitID());
        if(iRet)
        {
            LOGERROR("Failed to do action unit AI, ret %d, uin %u\n", iRet, m_uiUin);
    
            //出错,结束战斗
            CCombatEventManager::NotifyCombatEnd(stRoleObj);
            return;
        }
    }

    if(pstActionUnitObj->IsCombatUnitDead())
    {
        //行动的战斗单位已经死亡，直接行动回合结束
        CCombatEventManager::NotifyActionRoundEnd(stRoleObj);
        return;
    }

    //抛出战斗单位移动事件
    CCombatEventManager::NotifyCombatUnitMove(stRoleObj);

    return;
}

void CCombatEventListener::OnActionRoundEndEvent(CGameRoleObj& stRoleObj)
{
    m_uiUin = stRoleObj.GetUin();

    //获取战场信息
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefiled obj, uin  %u\n", m_uiUin);

        //出错,结束战斗
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //处理战斗单位行动后生效的buff
    int iActionUnitID = pstBattlefieldObj->GetActionUnitID();
    int iRet = pstBattlefieldObj->DoBuffEffectByType(BUFF_TRIGGER_ACTIONROUNDEND, iActionUnitID, iActionUnitID);
    if(iRet)
    {
        LOGERROR("Failed to do action round end buff effect, ret %d, uin %u\n", iRet, m_uiUin);

        //出错,结束战斗
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //判断战斗胜负
    if(pstBattlefieldObj->CanEndCombat())
    {
        //能够结束战斗，抛出战斗结束事件
        CCombatEventManager::NotifyCombatEnd(stRoleObj);

        return;
    }

    //处理BUFF的回合数
    iRet = pstBattlefieldObj->DecreaseBuffRound();
    if(iRet)
    {
        LOGERROR("Failed to decrease buff round, ret %d, uin %u\n", iRet, m_uiUin);

        //出错,结束战斗
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    if(pstBattlefieldObj->GetActionUnitType() == COMBAT_UNIT_TYPE_ROLE ||
       pstBattlefieldObj->GetActionUnitType() == COMBAT_UNIT_TYPE_PARTNER)
    {
        //设置主角战斗单位的当前状态
        pstBattlefieldObj->SetActionUnitStatus(ROLE_COMBAT_UNIT_STAT_IDLE);
    }

    //清除当前出手的战斗单位
    pstBattlefieldObj->ClearActionCombatUnit();

    //todo jasonxiong4 脚本重做
    //调用战斗脚本
    //CModuleHelper::GetStoryFramework()->DoCombatScript(m_uiUin, pstBattlefieldObj->GetCrossID(), 2);

    //抛出战斗单位行动回合开始事件
    CCombatEventManager::NotifyActionRoundBegin(stRoleObj);

    return;
}

void CCombatEventListener::OnCombatUnitMoveEvent(CGameRoleObj& stRoleObj)
{
    m_uiUin = stRoleObj.GetUin();

    //获取战场
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefiled obj, uin %u\n", m_uiUin);

        //出错,结束战斗
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //尝试触发离开位置的机关
    int iRet = pstBattlefieldObj->TriggerPitfallTrap(TRAP_TRIGGER_LEAVE);
    if(iRet)
    {
        LOGERROR("Failed to trigger leave trap, ret %d, uin %u\n", iRet, m_uiUin);
        
        //出错，结束战斗
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //处理移动
    if(pstBattlefieldObj->GetActionUnitType() == COMBAT_UNIT_TYPE_ROLE ||
       pstBattlefieldObj->GetActionUnitType() == COMBAT_UNIT_TYPE_PARTNER)
    {
        //推送客户端主角移动的指令
        pstBattlefieldObj->NotifyRoleUnitDirective(UNIT_DIRECTIVE_MOVE);
        
        pstBattlefieldObj->SetActionUnitStatus(ROLE_COMBAT_UNIT_STAT_MOVE);
    }
    else
    {
        //执行移动AI
        int iRet = pstBattlefieldObj->DoCombatUnitMove(CCombatUnitAI::GetMoveTargetPath());
        if(iRet)
        {
            LOGERROR("Failed to do comat unit move AI, ret %d, uin %u\n", iRet, m_uiUin);

            //出错,结束战斗
            CCombatEventManager::NotifyCombatEnd(stRoleObj);

            return;
        }

        //抛出战斗单位行动事件
        CCombatEventManager::NotifyCombatUnitAction(stRoleObj);
    }

    return;
}

void CCombatEventListener::OnCombatUnitActionEvent(CGameRoleObj& stRoleObj)
{
    m_uiUin = stRoleObj.GetUin();

    //获取战场信息
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, uin %u\n", m_uiUin);

        //出错,结束战斗
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    if(pstBattlefieldObj->IsCombatUnitDead(pstBattlefieldObj->GetActionUnitID()))
    {
        //行动的单位已经死亡,抛出战斗单位结束行动事件
        CCombatEventManager::NotifyActionRoundEnd(stRoleObj);

        return;
    }

    //尝试触发停止在位置的机关
    int iRet = pstBattlefieldObj->TriggerPitfallTrap(TRAP_TRIGGER_MOVEON);
    if(iRet)
    {
        LOGERROR("Failed to trigger moveon trap, ret %d, uin %u\n", iRet, m_uiUin);
        
        //出错，结束战斗
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //处理战斗单位行动
    if(pstBattlefieldObj->GetActionUnitType() == COMBAT_UNIT_TYPE_ROLE ||
       pstBattlefieldObj->GetActionUnitType() == COMBAT_UNIT_TYPE_PARTNER)
    {
        //是主角行动，通知客户端进行行动
        pstBattlefieldObj->NotifyRoleUnitDirective(UNIT_DIRECTIVE_ACTION);

        pstBattlefieldObj->SetActionUnitStatus(ROLE_COMBAT_UNIT_STAT_ACTION);
    }
    else
    {
        //如果可以攻击，执行攻击AI
        if(CCombatUnitAI::CanDoAttack())
        {
            int iRet = pstBattlefieldObj->DoCombatCastSkill(CCombatUnitAI::GetUseSkill(), CCombatUnitAI::GetTargetUnitPos(), SKILL_USE_AI);
            if(iRet)
            {
                LOGERROR("Failed to do combat cast skill AI, ret %d, skill %d, uin %u\n", iRet, CCombatUnitAI::GetUseSkill(), m_uiUin);

                //出错,结束战斗
                CCombatEventManager::NotifyCombatEnd(stRoleObj);
                return;
            }
        }

        //抛出战斗单位结束行动事件
        CCombatEventManager::NotifyActionRoundEnd(stRoleObj);
    }

    return;
}

