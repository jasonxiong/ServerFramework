
#ifndef __COMBAT_UNIT_AI_HPP__
#define __COMBAT_UNIT_AI_HPP__

#include <vector>

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"
#include "ModuleShortcut.hpp"

//AI可移动的位置的价值信息
struct FightAIPosValue
{
    TUNITPOSITION stPos;        //可移动到的位置点信息
    TUNITPOSITION stTargetPos;  //最佳攻击点的位置信息
    int iAttackValue;           //行动点的攻击价值
    int iDefenceValue;          //行动点的防御价值
    int iMoveValue;             //行动点的移动价值

    FightAIPosValue()
    {
       stPos.iPosX = -1;
       stTargetPos.iPosX = -1;

       iAttackValue = 0;
       iDefenceValue = 0;
       iMoveValue = 0;
    };
};

//所有非控制单位的AI逻辑处理
class CCombatUnitAI
{
public:
    CCombatUnitAI();
    ~CCombatUnitAI();

    static int DoActionUnitAI(int iBattlefiledObjID, int iActionUnitID);

    //获取AI移动的路径
    static const UnitPath& GetMoveTargetPath();

    //当前执行的AI是否可以攻击
    static bool CanDoAttack();

    //当前使用的技能
    static int GetUseSkill();

    //当前攻击的目标单位
    static const TUNITPOSITION& GetTargetUnitPos();

private:

    //获取当前所有可移动点的集合
    static int GetAllValidMovePos(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj);

    //获取当前点周围的可移动点
    static void GetNearByValidMovePos(CBattlefieldObj& stBattlefieldObj, TUNITPOSITION& stPos, int iMoveStep);

    //判断当前点是否有效的可移动点
    static bool ProcessCurrentMovePos(CBattlefieldObj& stBattlefieldObj, TUNITPOSITION& stPos);

    //计算所有可移动位置点的攻击价值
    static int ProcessAttackValue(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig);

    //计算所有可移动位置点的防御价值
    static int ProcessDefenceValue(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig);

    //计算所有可移动位置点的移动价值
    static int ProcessMoveValue(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig);

    //计算某个可移动位置点的攻击价值
    static int ProcessOnePosAttackValue(CBattlefieldObj& stBattlefieldObj, FightAIPosValue& stPosValue, int& iMaxValue);

    //计算某个可移动位置点的防御价值
    static int ProcessOnePosDefenceValue(CBattlefieldObj& stBattlefiledObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig, FightAIPosValue& stPosValue);

    //计算某个可移动位置点的移动价值
    static int ProcessOnePosMoveValue(CBattlefieldObj& stBattlefiledObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig, FightAIPosValue& stPosValue);

    //获取当前使用的技能
    static void GetAIUseSkill(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig);

private:

    //计算技能的攻击价值
    static int ProcessSkillAttackValue(CCombatUnitObj& stActionObj, CCombatUnitObj& stTargetObj, const SFightUnitAIConfig& stAIConfig, 
                                       const SFightUnitSkillConfig& stSkillConfig, bool bIsToEnemy, const TUNITPOSITION& stPos);

    //获取位置到所有敌人位置的平均距离
    static int GetEnemyDisance(const TUNITPOSITION& stPos, std::vector<int>& vEnemyUnits);

    //获取两点之间可行走的距离
    static int GetWalkDistance(CBattlefieldObj& stBattlefieldObj, const TUNITPOSITION& stCurPos, const TUNITPOSITION& stTargetPos);

public:

    //当前执行AI的战场ID
    static int m_iBattlefiledObjID;
    
    //主动战斗方的uin
    static int m_uiActiveUin;

    //当前计算AI的单位ID
    static int m_iActionUnitID;

    //当前战场的地图ID
    static int m_iMapID;

    //当前计算的AI的ID
    static int m_iUnitAIID;

    //AI使用的技能ID
    static int m_iUseSkillID;

    //有效的AI行动点价值信息
    static int m_iValidPosNum;
    static FightAIPosValue m_astValidPos[MAX_FIGHT_AI_POS_NUM];

    //选中的移动目标点在有效点数组中的位置
    static int m_iMovePosIndex;
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
