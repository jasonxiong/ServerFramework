#ifndef __COMBAT_SKILL_HPP__
#define __COMBAT_SKILL_HPP__

//战斗中技能处理类

#include "GameConfigDefine.hpp"

using namespace GameConfig;

class CGameRoleObj;
class CCombatUnitObj;
class CCombatSkill
{
public:
    CCombatSkill();
    ~CCombatSkill();

public:

    //使用战斗技能,最后一个参数表示是否必然命中
    static int CastSkill(int iBattlefieldObjID, int iCastUnitID, const TUNITPOSITION& stTargetPos, int iSkillID, int iSkillUseType, bool bIsScriptCast = false);

private:

    //对单个目标使用战斗技能
    static int CastSkillToOneTarget(int iCastUnitID, int iTargetUnitID, int iSkillID, bool bIsChiefTarget);

    //命中判定
    static bool CheckCanHit(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig);

    //格挡判定
    static bool CheckCanBlock(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig);

    //暴击判定
    static bool CheckCanCrit(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig);

    //计算伤害
    static int GetSkillHurtNum(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig, bool bIsChiefTarget);

    //检查是否有效的技能释放目标
    static bool CheckIsValidTarget(CBattlefieldObj& stBattlefieldObj, int iTargetType, int iCastUnitID, int iTargetUnitID);

    //处理受击单位的转向
    static void TargetUnderAttackDirection(CCombatUnitObj& stTargetUnitObj, int iCastUnitDirection);

    //处理目标单位的伤害
    static int ProcessRealDamage(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, int iDamageNum, ActionTarget& stNotify, const SFightUnitSkillConfig& stSkillConfig);

    //处理目标单位单项属性的伤害
    //注意：这个函数中会修改iDamageNum的值
    static void ProcessTargetDamage(CCombatUnitObj& stTargetUnitObj, const SSkillDamageConfig& stDamageConfig, ActionTarget& stNotify, int iAttrType, int& iDamageNum);

    //处理受击方的队友协防,返回值为协防的效果系数
    static float ProcessCoordDefense(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stTargetUnitObj);

    //处理攻击技能造成的单位位移
    static int ProcessSkillMove(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, ActionTarget& stNotify, const SFightUnitSkillConfig& stSkillConfig);

    //处理技能的特殊效果
    static int ProcessSkillSpecailFunc(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, ActionTarget& stNotify, const SFightUnitSkillConfig& stSkillConfig);

public:

    //首要目标的命中状态
    static int m_iChiefTargetDodge;

private:

    //战场的ID
    static int m_iBattlefiledObjID;
    
    //释放技能的战斗单位ID
    static int m_iCastUnitID;
    
    //技能释放目标的战斗单位ID
    static int m_iTargetUnitID;
    
    //使用的技能ID
    static int m_iSkillID;  

    //战斗发起方的uin
    static unsigned m_uiActiveUin;

    //战斗接受方的uin
    static unsigned m_uiPassiveUin;

    //技能释放方的RoleObj,可能为空
    static CGameRoleObj* m_pstCastRoleObj;

    //攻守双方的距离
    static int m_iDistance;

    //是否脚本中使用的技能
    static bool m_bIsScriptCast;

    //技能的使用类型
    static int m_iUseType;

    //技能使用给客户端的通知消息
    static GameProtocolMsg m_stCombatActionNotify;

    //增加BUFF给客户端的通知
    static GameProtocolMsg m_stCombatAddBuffNotify;
};

#endif
