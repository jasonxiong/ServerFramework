#include <math.h>

#include "GameProtocol.hpp"
#include "MathHelper.hpp"
#include "ZoneErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "BattlefieldObj.hpp"
#include "CombatUnitObj.hpp"
#include "ModuleShortcut.hpp"
#include "Random.hpp"
#include "CombatUtility.hpp"
#include "ZoneMsgHelper.hpp"
#include "UnitUtility.hpp"
#include "FightUnitUtility.hpp"
#include "GameRole.hpp"
#include "ModuleHelper.hpp"

#include "CombatSkill.hpp"

using namespace ServerLib;

int CCombatSkill::m_iBattlefiledObjID = -1;
int CCombatSkill::m_iCastUnitID = -1;
int CCombatSkill::m_iTargetUnitID = -1;
int CCombatSkill::m_iSkillID = 0;
unsigned CCombatSkill::m_uiActiveUin = 0;
unsigned CCombatSkill::m_uiPassiveUin = 0;
int CCombatSkill::m_iDistance = 0;
int CCombatSkill::m_iChiefTargetDodge = SKILL_HIT_INVALID;
bool CCombatSkill::m_bIsScriptCast = false;
int CCombatSkill::m_iUseType = SKILL_USE_ACTIVE;
CGameRoleObj* CCombatSkill::m_pstCastRoleObj = NULL;

GameProtocolMsg CCombatSkill::m_stCombatActionNotify;
GameProtocolMsg CCombatSkill::m_stCombatAddBuffNotify;

CCombatSkill::CCombatSkill()
{

}

CCombatSkill::~CCombatSkill()
{

}

//使用战斗技能
int CCombatSkill::CastSkill(int iBattlefieldObjID, int iCastUnitID, const TUNITPOSITION& stTargetPos, int iSkillID, int iSkillUseType, bool bIsScriptCast)
{
    //设置传入数据信息
    m_iBattlefiledObjID = iBattlefieldObjID;
    m_iCastUnitID = iCastUnitID;
    m_iSkillID = iSkillID;
    m_uiActiveUin = 0;
    m_uiPassiveUin = 0;
    m_iDistance = 0;
    m_iChiefTargetDodge = SKILL_HIT_INVALID;
    m_bIsScriptCast = bIsScriptCast;
    m_iUseType = iSkillUseType;
    m_pstCastRoleObj = NULL;

    //获取战场BattlefieldObj
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(m_iBattlefiledObjID);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, invalid obj id %d\n", m_iBattlefiledObjID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    m_uiActiveUin = pstBattlefieldObj->GetActiveUin();
    m_uiPassiveUin = pstBattlefieldObj->GetPassiveUin();

    //行动方的uin
    unsigned uiCastUin = (pstBattlefieldObj->GetCombatUnitCamp(iCastUnitID)==FIGHT_CAMP_ACTIVE) ? m_uiActiveUin : m_uiPassiveUin;
    m_pstCastRoleObj = CUnitUtility::GetRoleByUin(uiCastUin);
    if(uiCastUin!=0 && !m_pstCastRoleObj)
    {
        LOGERROR("Failed to get cast role obj, uin %u\n", uiCastUin);
        return T_ZONE_GAMEROLE_NOT_EXIST;
    }

    //推送战斗单位开始行动的通知
    static GameProtocolMsg stBeginActionMsg;
    CZoneMsgHelper::GenerateMsgHead(stBeginActionMsg, MSGID_ZONE_BEGINCOMBATACTION_NOTIFY);

    pstBattlefieldObj->SendNotifyToBattlefield(stBeginActionMsg);

    //获取施法者对象
    CCombatUnitObj* pstCastUnitObj = CCombatUtility::GetCombatUnitObj(iCastUnitID);
    if(!pstCastUnitObj)
    {
        LOGERROR("Failed to get cast unit obj, invalid unit id %d\n", iCastUnitID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //获取目标对象
    int iTargetUnitID = -1;
    CCombatUnitObj* pstTargetUnitObj = pstBattlefieldObj->GetCombatUnitByPos(stTargetPos);
    if(pstTargetUnitObj)
    {
        iTargetUnitID = pstTargetUnitObj->GetCombatUnitID();
    }

    if(pstCastUnitObj->IsCombatUnitDead() || (pstTargetUnitObj&&pstTargetUnitObj->IsCombatUnitDead()))
    {
        //攻方或者守方死亡，直接返回
        return T_SERVER_SUCESS;
    }

    //读取技能表的配置
    const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(iSkillID);
    if(!pstSkillConfig)
    {
        LOGERROR("Failed to get fight skill config, invalid skill id %d\n", iSkillID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //如果是非脚本中主动使用,处理CD相关逻辑
    if(m_iUseType==SKILL_USE_ACTIVE && !bIsScriptCast)
    {
        if(pstCastUnitObj->GetSkillCDRound(iSkillID) != 0)
        {
            //技能还在CD状态
            LOGERROR("Failed to cast active skill, active uin %u, unit id %d, skill %d\n", m_uiActiveUin, iCastUnitID, iSkillID);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //设置技能CD状态
        static GameProtocolMsg stSkillCDMsg;
        CZoneMsgHelper::GenerateMsgHead(stSkillCDMsg, MSGID_ZONE_SKILLCDROUND_NOTIFY);
        Zone_SkillCDRound_Notify* pstCDNotify = stSkillCDMsg.mutable_m_stmsgbody()->mutable_m_stzone_skillcdround_notify();

        pstCastUnitObj->SetSkillCDRound(iSkillID, pstSkillConfig->iCDRound);
        pstCDNotify->set_iunitid(iCastUnitID);
        SkillCDInfo* pstInfo = pstCDNotify->add_stcdinfos();
        pstInfo->set_iskillid(iSkillID);
        pstInfo->set_icdround(pstSkillConfig->iCDRound);

        pstBattlefieldObj->SendNotifyToBattlefield(stSkillCDMsg);
    }

    //判断攻守双方位置是否合法
    std::vector<TUNITPOSITION> vCheckPos;
    if(pstTargetUnitObj)
    {
        for(int x=0; x<=pstTargetUnitObj->GetUnitSize(); ++x)
        {
            for(int y=0; y<=pstTargetUnitObj->GetUnitSize(); ++y)
            {
                vCheckPos.push_back(TUNITPOSITION(stTargetPos.iPosX+x, stTargetPos.iPosY-y));
            }
        }
    }
    else
    {
        vCheckPos.push_back(stTargetPos);
    }

    for(unsigned i=0; i<vCheckPos.size(); ++i)
    {
        m_iDistance = CCombatUtility::GetAttackDistance(pstCastUnitObj->GetUnitPosition(), vCheckPos[i], pstSkillConfig->iTargetRangeID);
        if(m_iDistance != 0)
        {
            break;
        }
    }

    if(m_iDistance == 0)
    {
        if(m_iUseType!=SKILL_USE_LIANJI &&
           m_iUseType!=SKILL_USE_JIAJI &&
           m_iUseType!=SKILL_USE_FANJI &&
           m_iUseType!=SKILL_USE_HEJI &&
           m_iUseType!=SKILL_USE_AI)
        {
            //必然命中的技能，报错返回
            LOGERROR("Failed to cast skill, invalid position, skill id %d, uin %u\n", iSkillID, m_uiActiveUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        return T_SERVER_SUCESS;
    }

    //处理攻击方的方向更新
    CCombatUtility::UpdateUnitDirection(*pstCastUnitObj, stTargetPos);

    //初始化推送的消息
    CZoneMsgHelper::GenerateMsgHead(m_stCombatActionNotify, MSGID_ZONE_COMBATACTION_NOTIFY);
    CZoneMsgHelper::GenerateMsgHead(m_stCombatAddBuffNotify, MSGID_ZONE_COMBATADDBUFF_NOTIFY);

    //获取增加BUFF推送的消息体
    Zone_CombatAddBuff_Notify* pstAddBuffNotify = m_stCombatAddBuffNotify.mutable_m_stmsgbody()->mutable_m_stzone_combataddbuff_notify();

    //获取技能效果的消息体
    Zone_CombatAction_Notify* pstNotify = m_stCombatActionNotify.mutable_m_stmsgbody()->mutable_m_stzone_combataction_notify();

    //设置技能释放相关信息
    pstNotify->set_iactionunitid(m_iCastUnitID);
    pstNotify->set_etype(COMBAT_ACTION_CASTSKILL);
    pstNotify->set_icastskillid(m_iSkillID);
    pstNotify->set_iuseskilltype(m_iUseType);
    pstNotify->mutable_sttargetpos()->set_iposx(stTargetPos.iPosX);
    pstNotify->mutable_sttargetpos()->set_iposy(stTargetPos.iPosY);

    //增加给自己的buff
    int iRet = pstCastUnitObj->AddUnitBuff(m_uiActiveUin, pstBattlefieldObj->GetCrossID(), pstSkillConfig->iSelfBuff, iCastUnitID, *pstAddBuffNotify);
    if(iRet)
    {
        LOGERROR("Failed to add self unit buff, ret %d, buff id %d, uin %u\n", iRet, pstSkillConfig->iSelfBuff, m_uiActiveUin);
        return iRet;
    }

    //读取伤害范围表
    const SSkillAreaConfig* pstAreaConfig = SkillAreaCfgMgr().GetConfig(pstSkillConfig->iTargetAreaID);
    if(!pstAreaConfig)
    {
        LOGERROR("Failed to get skill area config, invalid area id %d\n", pstSkillConfig->iTargetAreaID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    TUNITPOSITION stTmpPos;
    TUNITPOSITION stTmpTargetPos;
    stTmpTargetPos.iPosX = stTargetPos.iPosX;
    stTmpTargetPos.iPosY = stTargetPos.iPosY;

    bool bIsChiefTarget = false;
    std::vector<int> vProcessedTargets;

    for(int i=0; i<pstAreaConfig->iAreaPosNum; ++i)
    {
        //根据方向修正策划配置的坐标
        CCombatUtility::FixSkillAreaPos(pstCastUnitObj->GetUnitDirection(), pstAreaConfig->astTargetPosInfo[i], stTmpPos);

        stTmpPos.iPosX += stTmpTargetPos.iPosX;
        stTmpPos.iPosY += stTmpTargetPos.iPosY;

        CCombatUnitObj* pstOneUnitObj = pstBattlefieldObj->GetCombatUnitByPos(stTmpPos);
        if(!pstOneUnitObj || pstOneUnitObj->IsCombatUnitDead())
        {
            //目标位置无单位或已死亡
            continue;
        }

        //判断是否正确的释放目标
        if(!CheckIsValidTarget(*pstBattlefieldObj, pstSkillConfig->iTargetType, iCastUnitID, pstOneUnitObj->GetCombatUnitID()))
        {
            continue;
        }

        //判断目标是否已经处理过，适用于大体积单位
        if(std::find(vProcessedTargets.begin(),vProcessedTargets.end(), pstOneUnitObj->GetCombatUnitID()) != vProcessedTargets.end())
        {
            //已经处理过
            continue;
        }

        vProcessedTargets.push_back(pstOneUnitObj->GetCombatUnitID());

        if(pstOneUnitObj->GetCombatUnitID() == iTargetUnitID)
        {
            bIsChiefTarget = true;
        }
        else
        {
            bIsChiefTarget = false;
        }

        //计算技能效果
        iRet = CastSkillToOneTarget(iCastUnitID, pstOneUnitObj->GetCombatUnitID(), iSkillID, bIsChiefTarget);
        if(iRet)
        {
            LOGERROR("Failed to cast skill to one target, skill id %d, ret %d, uin %u\n", iSkillID, iRet, m_uiActiveUin);
            return iRet;
        }
    }

    //推送伤害的消息
    pstBattlefieldObj->SendNotifyToBattlefield(m_stCombatActionNotify);

    //推送增加BUFF的消息
    if(pstAddBuffNotify->staddbuffs_size() != 0)
    {
        pstBattlefieldObj->SendNotifyToBattlefield(m_stCombatAddBuffNotify);
    }

    //技能处理结束后调用脚本
    //int iExecuteType = (m_iChiefTargetDodge!=SKILL_HIT_CHIEFNODODGE) ? SKILL_SCRIPT_CALL_CHIEFDODGE : SKILL_SCRIPT_CALL_SKILLDONE;

    //todo jasonxiong4 脚本重做
    //CModuleHelper::GetStoryFramework()->DoSkillScript(m_uiActiveUin, pstBattlefieldObj->GetCrossID(), iSkillID, iCastUnitID, 
                                                      //stTmpTargetPos.iPosX, stTmpTargetPos.iPosY, iExecuteType);

    return T_SERVER_SUCESS;
}

//对单个目标使用战斗技能
int CCombatSkill::CastSkillToOneTarget(int iCastUnitID, int iTargetUnitID, int iSkillID, bool bIsChiefTarget)
{
    m_iTargetUnitID = iTargetUnitID;

    //获取技能效果的消息体
    Zone_CombatAction_Notify* pstNotify = m_stCombatActionNotify.mutable_m_stmsgbody()->mutable_m_stzone_combataction_notify();

    //获取增加BUFF的消息体
    Zone_CombatAddBuff_Notify* pstAddBuffNotify = m_stCombatAddBuffNotify.mutable_m_stmsgbody()->mutable_m_stzone_combataddbuff_notify();

    //设置攻击目标的相关消息
    ActionTarget* pstTargets = pstNotify->add_sttargets();
    pstTargets->set_itargetunitid(m_iTargetUnitID);

    //初始化状态数组
    for(int i=ACTION_EFFECT_STATUS_MINZHONG; i<ACTION_EFFECT_STATUS_MAX; ++i)
    {
        pstTargets->add_bisstatset(false);
    }

    //初始化单位属性的效果影响
    for(int i=0; i<FIGHT_ATTR_MAX; ++i)
    {
        pstTargets->add_iattreffect(0);
        pstTargets->add_icastattreffect(0);
    }

    //获取战场
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(m_iBattlefiledObjID);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, invalid battlefield index %d\n", m_iBattlefiledObjID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //读取技能的配置
    const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(iSkillID);
    if(!pstSkillConfig)
    {
        LOGERROR("Failed to cast skill, invalid skill id %d\n", iSkillID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //获取使用技能的战斗单位
    CCombatUnitObj* pstCastUnitObj = CCombatUtility::GetCombatUnitObj(iCastUnitID);
    if(!pstCastUnitObj)
    {
        LOGERROR("Failed to get cast skill combat unit obj, index %d, uin %u\n", iCastUnitID, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //获取技能释放的目标单位
    CCombatUnitObj* pstTargetUnitObj = CCombatUtility::GetCombatUnitObj(iTargetUnitID);
    if(!pstTargetUnitObj)
    {
        LOGERROR("Failed to get skill target combat unit obj, index %d, uin %u\n", iTargetUnitID, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = T_SERVER_SUCESS;

    //判断技能命中修正
    if(pstSkillConfig->iHitModifierID == 0)
    {
        //技能没有命中修正

        //如果是首要目标，则设置首要目标不检查命中
        if(bIsChiefTarget)
        {
            m_iChiefTargetDodge = SKILL_HIT_NOHIT;
        }

        //处理伤害
        int iRealDamageNum = 0;
        iRet = pstTargetUnitObj->AddFightAttr(FIGHT_ATTR_HP, -pstSkillConfig->iDamageBaseNum, &iRealDamageNum);
        if(iRet)
        {
            LOGERROR("Failed to add fight attr, ret %d, uin %u\n", iRet, m_uiActiveUin);
            return iRet;
        }

        //给客户端掉血的返回
        pstTargets->set_iattreffect(FIGHT_ATTR_HP, iRealDamageNum);

        //给目标增加buff
        iRet = pstTargetUnitObj->AddUnitBuff(m_uiActiveUin, pstBattlefieldObj->GetCrossID(), pstSkillConfig->iTargetBuff, iCastUnitID, *pstAddBuffNotify);
        if(iRet)
        {
            LOGERROR("Failed to add target unit buff, ret %d, buff id %d, uin %u\n", iRet, pstSkillConfig->iTargetBuff, m_uiActiveUin);
            return iRet;
        }

        //如果产生伤害，则触发目标受伤害类型的BUFF
        if(pstSkillConfig->iDamageBaseNum > 0)
        {
            iRet = pstBattlefieldObj->DoBuffEffectByType(BUFF_TRIGGER_UNDERDAMAGE, iTargetUnitID, iCastUnitID);
            if(iRet)
            {
                LOGERROR("Failed to do under damage buff effect, target unit %d, trigger unit %d, uin %u\n", iTargetUnitID, iCastUnitID, m_uiActiveUin);
                return iRet;
            }
        }
    }
    else
    {
        //计算是否命中
        bool bIsHit = CheckCanHit(*pstCastUnitObj, *pstTargetUnitObj, *pstSkillConfig);
        if(!bIsHit)
        {
            //未命中,则为闪避状态
            pstTargets->set_bisstatset(ACTION_EFFECT_STATUS_SHANBI, true);

            //如果是首要目标，则设置首要目标闪避
            if(bIsChiefTarget)
            {
                m_iChiefTargetDodge = SKILL_HIT_CHIEFDODGE;
            }

            //处理目标转向为面朝攻击方的方向
            TargetUnderAttackDirection(*pstTargetUnitObj, pstCastUnitObj->GetUnitDirection());

            //产生闪避，触发闪避类型的BUFF
            iRet = pstBattlefieldObj->DoBuffEffectByType(BUFF_TRIGGER_DODGE, iTargetUnitID, iCastUnitID);
            if(iRet)
            {
                LOGERROR("Failed to do dodge buff effect, ret %d, target unit %d, cast unit %d, uin %u\n", iRet, iTargetUnitID, iCastUnitID, m_uiActiveUin);
                return iRet;
            }
        }
        else
        {
            //命中
            pstTargets->set_bisstatset(ACTION_EFFECT_STATUS_MINZHONG, true);

            //如果是首要目标，则设置首要目标未闪避
            if(bIsChiefTarget)
            {
                m_iChiefTargetDodge = SKILL_HIT_CHIEFNODODGE;
            }

            //处理防守方的受击buff
            iRet = pstBattlefieldObj->DoBuffEffectByType(BUFF_TRIGGER_UNDERATTACK, iTargetUnitID, iCastUnitID);
            if(iRet)
            {
                LOGERROR("Failed to do under attack buff effect, ret %d, target unit %d, cast unit %d, uin %u\n", iRet, iTargetUnitID, iCastUnitID, m_uiActiveUin);
                return iRet;
            }

            int iDamageNum = 0;
            if(pstSkillConfig->iDamageID != 0)
            {
                //判断是否格挡
                bool bIsBlock = CheckCanBlock(*pstCastUnitObj, *pstTargetUnitObj, *pstSkillConfig);
    
                //判断是否暴击
                bool bIsCrit = CheckCanCrit(*pstCastUnitObj, *pstTargetUnitObj, *pstSkillConfig);
    
                //计算伤害
                iDamageNum = GetSkillHurtNum(*pstCastUnitObj, *pstTargetUnitObj, *pstSkillConfig, bIsChiefTarget);

                //增加暴击修正
                if(bIsCrit)
                {
                    iDamageNum = iDamageNum * pstSkillConfig->iCritEffect / 10000;
                    pstTargets->set_bisstatset(ACTION_EFFECT_STATUS_BAOJI, true);
                }
    
                //增加格挡修正
                if(bIsBlock)
                {
                    iDamageNum = iDamageNum * pstSkillConfig->iParryEffect / 10000;
                    pstTargets->set_bisstatset(ACTION_EFFECT_STATUS_GEDANG, true);
    
                    //处理目标转向为面朝攻击方的方向
                    TargetUnderAttackDirection(*pstTargetUnitObj, pstCastUnitObj->GetUnitDirection());
                }
    
                iDamageNum = (iDamageNum<=0) ? 1 : iDamageNum;
    
                //处理协防的逻辑
                float fCoordEffectParam = ProcessCoordDefense(*pstBattlefieldObj, *pstTargetUnitObj);
                iDamageNum = (int)(iDamageNum*fCoordEffectParam);
    
                //处理技能伤害的脚本逻辑
                //const TUNITPOSITION& stTargetPos = pstTargetUnitObj->GetUnitPosition();
                //todo jasonxiong4 脚本重做
                /*
                int iDamageRate = CModuleHelper::GetStoryFramework()->DoSkillScript(m_uiActiveUin, pstBattlefieldObj->GetCrossID(), iSkillID, 
                                                                                iCastUnitID, stTargetPos.iPosX, stTargetPos.iPosY, SKILL_SCRIPT_CALL_DAMAGE);
                if(iDamageRate != 0)
                {
                    iDamageNum = iDamageNum*iDamageRate/100;
                }
                */
                    
                //伤害的处理，包括目标的伤害和攻击方的反馈
                iRet = ProcessRealDamage(*pstBattlefieldObj, *pstCastUnitObj, *pstTargetUnitObj, iDamageNum, *pstTargets, *pstSkillConfig);
                if(iRet)
                {
                    LOGERROR("Failed to process real damage, skill id %d, uin %u\n", pstSkillConfig->iConfigID, m_uiActiveUin);
                    return iRet;
                }
            }

            //处理技能造成的攻守双方的位移,只对主要目标有效
            if(bIsChiefTarget)
            {
                iRet = ProcessSkillMove(*pstBattlefieldObj, *pstCastUnitObj, *pstTargetUnitObj, *pstTargets, *pstSkillConfig);
                if(iRet)
                {
                    LOGERROR("Failed to process skill move, skill id %d, ret %d, uin %u\n", iSkillID, iRet, m_uiActiveUin);
                    return iRet;
                }
            }

            //目标增加Targetbuff
            iRet = pstTargetUnitObj->AddUnitBuff(m_uiActiveUin, pstBattlefieldObj->GetCrossID(), pstSkillConfig->iTargetBuff, iCastUnitID, *pstAddBuffNotify);
            if(iRet)
            {
                LOGERROR("Failed to add unit buff, ret %d, uin %u\n", iRet, m_uiActiveUin);
                return iRet;
            }

            //防守方触发受伤害buff
            if(iDamageNum > 0)
            {
                //产生伤害，目标触发受伤害BUFF
                iRet = pstBattlefieldObj->DoBuffEffectByType(BUFF_TRIGGER_UNDERDAMAGE, iTargetUnitID, iCastUnitID);
                if(iRet)
                {
                    LOGERROR("Failed to do under damage buff effect, ret %d, target unit %d, cast unit %d, uin %u\n", iRet, iTargetUnitID, iCastUnitID, m_uiActiveUin);
                    return iRet;
                }
            }
        }

        //设置目标为自己最后攻击单位
        pstCastUnitObj->SetLastAttackUnitID(iTargetUnitID);
    }

    //是主要目标并且技能会产生特殊效果，处理特殊效果
    if(bIsChiefTarget && pstSkillConfig->iSpecialFuncType!=0)
    {
        iRet = ProcessSkillSpecailFunc(*pstCastUnitObj, *pstTargetUnitObj, *pstTargets, *pstSkillConfig);
        if(iRet)
        {
            LOGERROR("Failed to process skill specail func, ret %d, skill id %d, uin %u\n", iRet, iSkillID, m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//命中判定
bool CCombatSkill::CheckCanHit(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig)
{
    //处理攻击目标的受伤状态,当目标HP < 60%*MAXHP 时无法闪避
    if(stTargetUnitObj.GetFightAttr(FIGHT_ATTR_HP) < stTargetUnitObj.GetFightAttr(FIGHT_ATTR_HPMAX)*60/100)
    {
        return true;
    }

    //计算招式命中
    int iSkillHit = CCombatUtility::GetSkillHit(stCastUnitObj, stSkillConfig, m_iDistance);

    //计算招式闪避
    int iSkillDodge = CCombatUtility::GetSkillDodge(stCastUnitObj, stTargetUnitObj, stSkillConfig);

    //判定公式 RAND(0,1) <= (招式命中)/(招式命中+招式闪避)
    float fHitRate = (float)iSkillHit/(float)(iSkillDodge+iSkillHit)*100;

    int iRandNum = CRandomCalculator::GetRandomNumberInRange(100);
    if(iRandNum <= (int)fHitRate)
    {
        return true;
    }

    return false;
}

//格挡判定
bool CCombatSkill::CheckCanBlock(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig)
{
    //处理目标的受伤状态,当目标 HP < 30%*MAXHP 时无法格挡
    if(stTargetUnitObj.GetFightAttr(FIGHT_ATTR_HP) < stTargetUnitObj.GetFightAttr(FIGHT_ATTR_HPMAX)*30/100)
    {
        return false;
    }

    //招式精准
    const SSkillScoreConfig* pstScoreConfig = SkillScoreCfgMgr().GetConfig(stSkillConfig.iPrecisionModifierID);
    if(!pstScoreConfig)
    {
        LOGERROR("Failed to get skill score config, invalid precision id %d\n", stSkillConfig.iPrecisionModifierID);
        return false;
    }

    int iSkillPrecision = CCombatUtility::GetSkillScoreNum(stCastUnitObj, *pstScoreConfig);

    //招式格挡
    pstScoreConfig = SkillScoreCfgMgr().GetConfig(stSkillConfig.iCopeModifierID);
    if(!pstScoreConfig)
    {
        LOGERROR("Failed to get skill score config, invalid cope id %d\n", stSkillConfig.iCopeModifierID);
        return false;
    }

    int iSkillBlock = CCombatUtility::GetSkillScoreNum(stTargetUnitObj, *pstScoreConfig);

    //格挡判定
    int iRandomNum = CRandomCalculator::GetRandomNumberInRange(100);

    //计算公式： RAND(0,1) < 招式精准/(招式精准+招式格挡)  此时为格挡失败
    if(iRandomNum*iSkillBlock >= (100-iRandomNum)*iSkillPrecision)
    {
        return true;
    }

    return false;
}

//暴击判定
bool CCombatSkill::CheckCanCrit(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig)
{
    //招式暴击
    const SSkillScoreConfig* pstScoreConfig = SkillScoreCfgMgr().GetConfig(stSkillConfig.iCritModifierID);
    if(!pstScoreConfig)
    {
        LOGERROR("Failed to get skill score config, invalid crit id %d\n", stSkillConfig.iCritModifierID);
        return false;
    }

    int iSkillCrit = CCombatUtility::GetSkillScoreNum(stCastUnitObj, *pstScoreConfig);

    //招式坚韧
    pstScoreConfig = SkillScoreCfgMgr().GetConfig(stSkillConfig.iToughModifierID);
    if(!pstScoreConfig)
    {
        LOGERROR("Failed to get skill score config, invalid tough id %d\n", stSkillConfig.iToughModifierID);
        return false;
    }

    int iSkillTough = CCombatUtility::GetSkillScoreNum(stTargetUnitObj, *pstScoreConfig);

    //暴击判定
    int iRandomNum = CRandomCalculator::GetRandomNumberInRange(100);

    //计算公式： RAND(0,1) < 招式精准/(招式精准+招式格挡)  此时为格挡失败
    if(iRandomNum*iSkillTough <= (100-iRandomNum)*iSkillCrit)
    {
        return true;
    }

    return false;
}

//计算伤害
int CCombatSkill::GetSkillHurtNum(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig, bool bIsChiefTarget)
{
    //基本威力
    int iBaseHurt = stSkillConfig.iDamageBaseNum;

    if(stSkillConfig.iDamageModifierID != 0)
    {
        //增加伤害修正
        const SSkillScoreConfig* pstScoreConfig = SkillScoreCfgMgr().GetConfig(stSkillConfig.iDamageModifierID);
        if(!pstScoreConfig)
        {
            LOGERROR("Failed to get score config, damage modifier score id %d\n", stSkillConfig.iDamageModifierID);
            return T_ZONE_SYSTEM_INVALID_CFG;
        }
    
        int iDamageModifier = CCombatUtility::GetSkillScoreNum(stCastUnitObj, *pstScoreConfig);

        iBaseHurt += iDamageModifier;
    }

    //攻击威力 = 基本威力*range[伤害分布].Rx	//x为目标距离
    int iRangeIndex = m_iDistance;
    if(iRangeIndex >= MAX_SKILL_RANGE_INFO_NUM)
    {
        iRangeIndex = MAX_SKILL_RANGE_INFO_NUM - 1;
    }

    //距离转换为下标需要减1
    iRangeIndex = iRangeIndex - 1;
    
    //读取Range表
    const SSkillRangeConfig* pstRangeConfig = SkillRangeCfgMgr().GetConfig(stSkillConfig.iDamageRangeID);
    if(!pstRangeConfig)
    {
        LOGERROR("Failed to get skill range config, invalid range id %d\n", stSkillConfig.iDamageRangeID);

        return 0;
    }

    int iAttackHurt = iBaseHurt*pstRangeConfig->aiRangeInfo[iRangeIndex]/10000;
    
    //攻击强度
    int iAttackStrength = CCombatUtility::GetAttackStrength(stCastUnitObj, stSkillConfig);

    //防御强度
    int iDefenceStrength = CCombatUtility::GetDefenceStrength(stCastUnitObj, stTargetUnitObj, stSkillConfig);

	int iDenominator = iAttackStrength+iDefenceStrength;
	if(iDenominator <= 0)
	{
		iDenominator = 1;
	}

    if(!bIsChiefTarget)
    {
        //次要目标
        return (iAttackHurt * iAttackStrength /iDenominator)*stSkillConfig.iSecondaryTarget/10000;
    }

    //主要目标
    return (iAttackHurt * iAttackStrength /iDenominator);
}

//检查是否有效的技能释放目标
bool CCombatSkill::CheckIsValidTarget(CBattlefieldObj& stBattlefieldObj, int iTargetType, int iCastUnitID, int iTargetUnitID)
{
    //检查战斗单位的阵营信息
    FightCampType eCastCampType = (FightCampType)stBattlefieldObj.GetCombatUnitCamp(iCastUnitID);
    FightCampType eTargetCampType = (FightCampType)stBattlefieldObj.GetCombatUnitCamp(iTargetUnitID);
    if(eCastCampType == FIGHT_CAMP_INVALID || eTargetCampType == FIGHT_CAMP_INVALID)
    {
        LOGERROR("Failed to cast skill, invalid camp type, active uin %u\n", m_uiActiveUin);
        return false;
    }

    //判断目标是否是敌人
    bool bIsEnemy = (eCastCampType != eTargetCampType);
	
	switch(iTargetType)
    {
    case SKILL_TARGET_SELF:
        {
            //只能对自己使用
            if(iTargetUnitID != iCastUnitID)
            {
                return false;
            }
        }
        break;

    case SKILL_TARGET_CAMP:
        {
            //只能对友方使用
            if((iTargetUnitID==iCastUnitID) || bIsEnemy)
            {
                return false;
            }
        }
        break;

    case SKILL_TARGET_NOT_ENEMY:
        {
            //可以对非敌方使用
            if(bIsEnemy)
            {
                return false;
            }
        }
        break;

    case SKILL_TARGET_ENEMY:
        {
            //可以对敌人使用
            if(!bIsEnemy)
            {
                return false;
            }
        }
        break;

    case SKILL_TARGET_ALL:
        {
            //可以对所有人使用
            return true;
        }
        break;

    default:
        {
            return false;
        }
        break;
    }

    return true;
}

//处理受击单位的转向
void CCombatSkill::TargetUnderAttackDirection(CCombatUnitObj& stTargetUnitObj, int iCastUnitDirection)
{
    switch(iCastUnitDirection)
    {
    case COMBAT_DIRECTION_EAST:
        {
            stTargetUnitObj.SetUnitDirection(COMBAT_DIRECTION_WEST);
        }
        break;

    case COMBAT_DIRECTION_NORTH:
        {
            stTargetUnitObj.SetUnitDirection(COMBAT_DIRECTION_SOUTH);
        }
        break;

    case COMBAT_DIRECTION_WEST:
        {
            stTargetUnitObj.SetUnitDirection(COMBAT_DIRECTION_EAST);
        }
        break;

    case COMBAT_DIRECTION_SOUTH:
        {
            stTargetUnitObj.SetUnitDirection(COMBAT_DIRECTION_NORTH);
        }
        break;

    default:
        {
            ;
        }
        break;
    }

    return;
}

//处理目标单位的伤害
int CCombatSkill::ProcessRealDamage(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, int iDamageNum, ActionTarget& stNotify, const SFightUnitSkillConfig& stSkillConfig)
{
    //读取伤害表
    const SSkillDamageConfig* pstDamageConfig = SkillDamageCfgMgr().GetConfig(stSkillConfig.iDamageID);
    if(!pstDamageConfig)
    {
        LOGERROR("Failed to get skill damage config, skill id %d, damage id %d\n", stSkillConfig.iConfigID, stSkillConfig.iDamageID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //先处理攻击方的反馈
    int iRealAddHP = 0;

    stCastUnitObj.AddFightAttr(FIGHT_ATTR_HP, iDamageNum*pstDamageConfig->aiVampireAttr[FIGHT_ATTR_HP]/100, &iRealAddHP);

    //设置推送消息的返回
    stNotify.set_icastattreffect(FIGHT_ATTR_HP, iRealAddHP);

    //处理守护类型的BUFF,这个函数中会修改iDamageNum的值
    stBattlefieldObj.DoBuffEffectByType(BUFF_TRIGGER_PROCESSDAMAGE, stTargetUnitObj.GetCombatUnitID(), stCastUnitObj.GetCombatUnitID(), &iDamageNum);

    //处理生命的伤害
    ProcessTargetDamage(stTargetUnitObj, *pstDamageConfig, stNotify, FIGHT_ATTR_HP, iDamageNum);

    return T_SERVER_SUCESS;
}

//处理目标单位单项属性的伤害
void CCombatSkill::ProcessTargetDamage(CCombatUnitObj& stTargetUnitObj, const SSkillDamageConfig& stDamageConfig, ActionTarget& stNotify, int iAttrType, int& iDamageNum)
{
    if(iDamageNum<=0 || iAttrType<0 || iAttrType>=FIGHT_ATTR_MAX || stDamageConfig.aiSpreadAttr[iAttrType]<=0)
    {
        return;
    }

    int iAttrDamage = iDamageNum*stDamageConfig.aiSpreadAttr[iAttrType]*stDamageConfig.aiStrengthAttr[iAttrType]/100/100;
    iAttrDamage = stTargetUnitObj.GetFightAttr(iAttrType)>iAttrDamage ? iAttrDamage : stTargetUnitObj.GetFightAttr(iAttrType);

    int iRealDamage = 0;
    stTargetUnitObj.AddFightAttr(iAttrType, -iAttrDamage, &iRealDamage);

    iDamageNum -= (iAttrDamage*100/stDamageConfig.aiStrengthAttr[iAttrType]);

    stNotify.set_iattreffect(iAttrType, iRealDamage);

    return;
}

//处理受击方的队友协防,返回值为协防的效果系数
float CCombatSkill::ProcessCoordDefense(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stTargetUnitObj)
{
    int iNearByPartnerNum = stBattlefieldObj.GetNearByUnitNum(stTargetUnitObj, false);
    if(iNearByPartnerNum == 0)
    {
        return 1.0f;
    }

    int iAngleNearByPartnerNum = stBattlefieldObj.GetAngleNearByUnitNum(stTargetUnitObj, false);

    return pow(0.8f, iNearByPartnerNum+iAngleNearByPartnerNum*0.5f);
}

//处理攻击技能造成的单位位移
int CCombatSkill::ProcessSkillMove(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stCastUnitObj, 
                                   CCombatUnitObj& stTargetUnitObj, ActionTarget& stNotify, const SFightUnitSkillConfig& stSkillConfig)
{
    int iRet = T_SERVER_SUCESS;

    //先处理攻击方的
    if(stSkillConfig.iSelfMoveID != 0)
    {
        iRet = CCombatUtility::ProcessUnitMove(stBattlefieldObj, stCastUnitObj, *stNotify.mutable_stcastpos(), stSkillConfig.iSelfMoveID, stCastUnitObj.GetUnitDirection());
        if(iRet)
        {
            LOGERROR("Failed to process one skill move, skill id %d, ret %d, uin %u\n", stSkillConfig.iConfigID, iRet, m_uiActiveUin);
            return iRet;
        }
    }

    //再处理受击方的
    if(stSkillConfig.iTargetMoveID != 0)
    {
        iRet = CCombatUtility::ProcessUnitMove(stBattlefieldObj, stTargetUnitObj, *stNotify.mutable_sttargetpos(), stSkillConfig.iTargetMoveID, stCastUnitObj.GetUnitDirection());
        if(iRet)
        {
            LOGERROR("Failed to process one skill move, skill id %d, ret %d, uin %u\n", stSkillConfig.iConfigID, iRet, m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//处理技能的特殊效果
int CCombatSkill::ProcessSkillSpecailFunc(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, ActionTarget& stNotify, const SFightUnitSkillConfig& stSkillConfig)
{
    if(stCastUnitObj.GetUnitSize()!=0 || stTargetUnitObj.GetUnitSize()!=0)
    {
        //大体积单位不支持位移
        return 0;
    }

    stNotify.set_ispecialfunctype(stSkillConfig.iSpecialFuncType);

    switch(stSkillConfig.iSpecialFuncType)
    {
    case SKILL_SPECIAL_FUNC_EXCHANGEPOS:
        {
            //移行换位，和目标位置互换
            TUNITPOSITION stCastOldPos = stCastUnitObj.GetUnitPosition();
            stCastUnitObj.SetUnitPosition(stTargetUnitObj.GetUnitPosition());
            stTargetUnitObj.SetUnitPosition(stCastOldPos);

            stNotify.mutable_stcastpos()->set_iposx(stCastUnitObj.GetUnitPosition().iPosX);
            stNotify.mutable_stcastpos()->set_iposy(stCastUnitObj.GetUnitPosition().iPosY);
            stNotify.mutable_sttargetpos()->set_iposx(stTargetUnitObj.GetUnitPosition().iPosX);
            stNotify.mutable_sttargetpos()->set_iposy(stTargetUnitObj.GetUnitPosition().iPosY);
        }
        break;

    default:
        {
            return T_ZONE_SYSTEM_INVALID_CFG;
        }
        break;
    }

    return 0;
}

