
#ifndef __COMBAT_UTILITY_HPP__
#define __COMBAT_UTILITY_HPP__

#include "GameConfigDefine.hpp"

using namespace GameConfig;

//战斗工具函数类
class CGameRoleObj;
class CBattlefieldObj;
class CCombatUnitObj;
class TUNITPOSITION;
class CCombatUtility
{
public:
    
    static CBattlefieldObj* GetBattlefiledObj(CGameRoleObj& stRoleObj);

    static CBattlefieldObj* GetBattlefiledObj(int iBattlefieldIndex);

    //根据战斗单位ID获取CCombatUnitObj
    static CCombatUnitObj* GetCombatUnitObj(int iCombatUnitID); 

    //清理战场
    static void ClearBattlefield(int iBattlefieldIndex);

    //计算招式命中
    static int GetSkillHit(CCombatUnitObj& stCastUnitObj, const SFightUnitSkillConfig& stSkillConfig, int iDistance);

    //计算招式闪避
    static int GetSkillDodge(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig);

    //计算攻击强度
    static int GetAttackStrength(CCombatUnitObj& stCastUnitObj, const SFightUnitSkillConfig& stSkillConfig);

    //计算防御强度
    static int GetDefenceStrength(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig);

    //获取敌对单位的攻击距离，为0表示不能攻击
    static int GetAttackDistance(const TUNITPOSITION& stCastPos, const TUNITPOSITION& stTargetPos, int iTargetRangeID);

    //根据Score表计算修正值
    static int GetSkillScoreNum(CCombatUnitObj& stCombatUnitObj, const SSkillScoreConfig& stScoreConfig);

    //更新战斗单位的方向
    static void UpdateUnitDirection(CCombatUnitObj& stCastUnitObj, const TUNITPOSITION& stTargetPos);

    //获取战斗单位攻击的方向, <0 表示出错
    static int GetAttackDirection(CCombatUnitObj& stCastUnitObj, const TUNITPOSITION& stTargetPos);

    //修正技能伤害范围的坐标
    static void FixSkillAreaPos(int iCastDirection, const SFightPosInfo& stTargetPos, TUNITPOSITION& stFixedPos);

    //处理战斗单位的特殊位移,bStop表示是否中途遇阻挡停下
    static int ProcessUnitMove(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stUnitObj, UnitPosition& stPosNotify, int iAreaID, int iDirection, bool bStop = true);

    //玩家当前是否可以进行PVP战斗
    static bool CanDoPVPCombat(unsigned int uin);

    //根据武器生效状态处理武器效果，bIsAram表示是否生效武器
    static void ProcessWeaponEffect(CCombatUnitObj& stUnitObj, bool bIsAram);

    //获取坐标范围内某阵营的单位
	static int GetAreaTargetUnits(unsigned uin, const TUNITPOSITION& stPos, int iAreaID, int iCamp, int iDirection, std::vector<int>& vUnits);
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
