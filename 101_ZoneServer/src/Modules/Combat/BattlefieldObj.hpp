
#ifndef __BATTLEFIELD_OBJ_HPP__
#define __BATTLEFIELD_OBJ_HPP__

#include <vector>

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"
#include "ZoneObjectAllocator.hpp"
#include "ScenePathManager.hpp"
#include "CombatTrapObj.hpp"

class CCombatUnitObj;
class CGameRoleObj;

using namespace GameConfig;

//主角战斗单位当前的状态类型
enum enRoleCombatUnitStatType
{
    ROLE_COMBAT_UNIT_STAT_INVALID = 0,      //非法的状态
    ROLE_COMBAT_UNIT_STAT_IDLE = 1,         //空闲状态
    ROLE_COMBAT_UNIT_STAT_MOVE = 2,         //等待移动状态
    ROLE_COMBAT_UNIT_STAT_ACTION = 3,       //等待行动状态
    ROLE_COMBAT_UNIT_STAT_SETFORM = 4,      //等待设置阵型
};

//PVP战斗关卡的ID
#define PVP_COMBAT_CROSS_ID 100

//战斗战场对象
class CBattlefieldObj : public CObj
{
public:
    CBattlefieldObj();
    virtual ~CBattlefieldObj();
    virtual int Initialize();

    DECLARE_DYN

public:

    //主动方的uin
    void SetActiveUin(unsigned int uiUin);
    unsigned int GetActiveUin();
    
    //被动方的uin
    void SetPassiveUin(unsigned int uiUin);
    unsigned int GetPassiveUin();

    //战场Obj的唯一ID
    void SetBattlefieldObjID(int iObjID);
    int GetBattlefieldObjID();

    //初始化PVE战场
    int InitPveBattlefield(CGameRoleObj& rstRoleObj, int iCrossID);

    //初始化PVP战场
    int InitPVPBattlefield(CGameRoleObj& stActiveRoleObj, CGameRoleObj& stPassiveRoleObj);

    //打包返回战场的信息
    int PackBattlefiledInfo(Zone_Battlefield_Notify& stNotify);

    //清理战场
    void ClearBattlefield();

    //初始化回合出手单位信息
    void InitRoundActionUnits();

    //判断当前是否有未出手单位
    bool HasActionCombatUnit();

    //设置当前出手的战斗单位
    void SetActionCombatUnit();

    //清除当前出手的战斗单位
    void ClearActionCombatUnit();

    //获取当前出手的战斗单位的类型
    int GetActionUnitType();

    //获取当前出手单位的ID
    int GetActionUnitID();

    //主角战斗单位当前的状态
    int GetActionUnitStatus();
    void SetActionUnitStatus(int iStatus);

    //判断是否能够结束战斗
    bool CanEndCombat();

    //推送客户端主角操作的指令
    void NotifyRoleUnitDirective(int iDirective);

    //开始战斗单位的移动
    int DoCombatUnitMove(const UnitPath& stPath);

    //开始战斗单位的移动,AI移动只有目标点
    int DoCombatUnitMove(int iMoveDistance, const TUNITPOSITION* pstMovePath);

    //战斗单位使用技能
    int DoCombatCastSkill(int iSkillID, const TUNITPOSITION& stTargetPos, int iSkillUseType);

    //战斗单位使用道具
    int DoCombatUseItem(int iItemID, const TUNITPOSITION& stTargetPos);

    //获取战斗单位的阵营
    int GetCombatUnitCamp(int iCombatUnitiD);

    //战斗单位是否同一阵营
    bool IsUnitInSameCamp(CCombatUnitObj& stUnitObj, CCombatUnitObj& stOtherUnitObj);

    //设置伙伴的战斗AI
    int SetPartnerAI(int iPartnerAIID);

    //获取地图ID
    int GetMapID();

    //根据坐标位置获取战斗单位信息
    CCombatUnitObj* GetCombatUnitByPos(const TUNITPOSITION& stPos);

    //或否是伙伴的最后攻击对象
    bool IsTeammateLastAttackUnit(int iCastUnitID, int iTargetUnitID);

    //获取敌人信息
    void GetEnemyUnits(int iCastUnitID, std::vector<int>& vEnemyUnits);

    //处理战场相关的BUFF
    int DoBuffEffectByType(int iTriggerType, int iTargetUnitID=-1, int iTriggerUnitID=-1, int* pDamageNum = NULL);
    int DecreaseBuffRound();

    //获取战斗胜负结果
    bool GetIsActiveWin();

    //设置战斗结果,是否己方获胜
    int SetCampActiveWin(int iIsActiveWin);

    //获取关卡的ID
    int GetCrossID();

    //设置当前行动单位的方向
    int SetActionUnitDirection(int iDirection);

    //获取阵营战斗单位的UnitID
    int GetCombatUnitByCamp(int iCampType, int iUnitIndex);

    //战斗单位的回合数
    int GetCombatRoundNum();
    void SetCombatRoundNum(int iRoundNum);

    //增加战场上的战斗单位
    int AddCombatUnit(int iCampType, int iConfigID, int iPosX, int iPosY, int iDirection, int iUnitType, int& iUnitID);

    //切换战斗单位的AI
    int ChangeCombatUnitAI(int iCombatUnitID, int iNewAI);

    //强制杀死战斗单位
    int KillCombatUnit(int iCombatUnitID, bool bSendNotify=true);

    //获取战斗开始时间
    int GetCombatStartTime();

    //处理行动回合开始前的技能
    int DoRoundActionSkill();

    //获取最大移动距离，考虑围困逻辑
    int GetMaxMoveDistance(CCombatUnitObj& stUnitObj);

    //获取战斗单位周围的阵营单位数量
    int GetNearByUnitNum(CCombatUnitObj& stUnitObj, bool bIsEnemy);

    //获取战斗单位斜角位置的阵营单位数量
    int GetAngleNearByUnitNum(CCombatUnitObj& stUnitObj, bool bIsEnemy);

    //战场上某个位置是否可以行走, iUnitID表示如果位置上是该单位可以行走
    bool IsPosCanWalk(const TUNITPOSITION& stPos, int iUnitID = -1);

    //获取战斗地图阻挡配置
    const CScenePathManager& GetBattlePathManager();

    //战场上增加机关,返回机关的ObjID，小于0表示增加失败
    int AddCombatTrap(int iConfigID, int iCamp, const TUNITPOSITION& stPos, int iDirection);

    //触发战场上的机关
    int TriggerCombatTrap(int iTriggerObjID, int iTrapObjID, int iTriggerType);

    //清除战场上的机关
    void DeleteCombatTrap(int iTrapObjID);

    //触发陷阱类型的机关
    int TriggerPitfallTrap(int iTriggerType);

    //手动触发机关
    int ManualTriggerTrap(const TUNITPOSITION& stPos);

    //攻击战场机关
    int AttackCombatTrap(const TUNITPOSITION& stPos);

    //判断机关是否可见
    bool IsTrapVisible(CCombatUnitObj& stUnitObj, CCombatTrapObj& stTrapObj);
    bool IsTrapVisibleToActionUnit(CCombatTrapObj& stTrapObj);

    //获取战场机关详细信息
	void GetCombatTrapInfo(std::vector<TCOMBATTRAPINFO>& vTrapInfo);

    //根据机关的位置获取机关
    CCombatTrapObj* GetTrapByPos(const TUNITPOSITION& stPos);
    int GetTrapIndexByPos(const TUNITPOSITION& stPos);

    //战场状态，供脚本使用
    bool GetCombatStatus(int iStatusType);
    void SetCombatStatus(int iStatusType, bool bIsSet);

    //直接改变战斗单位的属性
    void AddCombatUnitAttr(CCombatUnitObj& stUnitObj, int iAttrType, int iAddValue);

    //删除战斗单位身上的BUFF
    void DelUnitBuff(int iUnitID, int iBuffID);

    //推送消息给战场上所有玩家
    void SendNotifyToBattlefield(GameProtocolMsg& stNotify);

    //当前行动的玩家角色是否有效
    bool IsValidActionRole(unsigned int uin);

    //是否需要设置阵型
    bool IsNeedSetForm();

    //设置战斗阵型
    int SetCombatForm(unsigned int uin, const CombatForm& stForm);

    //推送战斗阵型的通知
    void SendCombatFormNotify();

    //判断战斗单位是否死亡
    bool IsCombatUnitDead(int iUnitID);

    //推送当前战场的出手信息
    void SendActionUnitNotify();

    //保存战场战斗单位详细信息
    void SaveCombatUnitInfo();

    //清理战场上死掉的单位
    void ClearDeadCombatUnit();

    //减少行动单位技能CD回合
    void DecreaseActionSkillCDRound();

private:

    //初始化战场信息
    int InitBattlefieldInfo(int iCrossID);

    //初始化怪物信息
    int InitMonsterInfo(int iCrossID);
    
    //初始化单只怪物信息
    int InitOneMonsterInfo(int iUnitIndex, const OneCrossMonster& stOneConfig, const SFightPosInfo& stPosInfo);

    //初始化单个阵营信息
    int InitFightCampInfo(CGameRoleObj& rstRoleObj, bool bIsActive = true);    

    //初始化单个战斗单位信息
    int InitOneFightUnitInfo(int iUnitIndex, CGameRoleObj& rstRoleObj, int iFightUnitID, const SFightPosInfo& stPosInfo, bool bIsActive);

    //打包返回单个战斗对象信息
    int PackOneCombatUnitInfo(int iCamp, int iUnitObjIndex, CombatUnit& stUnitInfo);

    //添加一个战斗单位到回合出手列表
    void AddOneUnitToRoundActionList(int iUnitObjIndex);

    //处理技能的连击
    int DoComboAttack(CCombatUnitObj& stTargetUnitObj, int iSkillID);

    //处理技能的夹击
    int DoPincerAttack(CCombatUnitObj& stTargetUnitObj);

    //处理技能的反击
    int DoCounterAttack(CCombatUnitObj& stCounterAtkUnitObj, CCombatUnitObj& stTargetObj);

    //处理伙伴技能的合击
    int DoJointAttack(CCombatUnitObj& stTargetUnitObj, int iSkillID);

    //获取阻挡类型的机关
    CCombatTrapObj* GetBlockTrapByPos(const TUNITPOSITION& stPos);

    //获取地图上有效站位位置
    void GetBattlefieldValidPos(const SBattleCrossConfig& stCrossConfig);

private:
    
    //创建CCombatUnitObj,参数返回ObjIndex
    CCombatUnitObj* CreateCombatUnitObj(int& iCombatUnitObjIndex);

    //删除一个战场单位
    void ClearOneCombatUnitObj(int iCombatUnitID);

///////////////////////////////////////////////////////////////////////
private:

    //战场对象的唯一ID
    int m_iBattlefieldObjID;

    //主动战斗方
    unsigned int m_uiActiveUin;
    int m_aiActiveCombatObjIndex[MAX_CAMP_FIGHT_UNIT_NUM];
    SFightPosInfo m_astActivePos[MAX_CAMP_FIGHT_UNIT_NUM];
    bool m_bActiveNeedSetForm;

    //被动战斗方
    unsigned int m_uiPassiveUin;
    int m_aiPassiveCombatObjIndex[MAX_CAMP_FIGHT_UNIT_NUM];
    SFightPosInfo m_astPassivePos[MAX_CAMP_FIGHT_UNIT_NUM];
    bool m_bPassiveNeedSetForm;

    //主动方脚本强制杀死的单位配置ID
    int m_iActiveScriptKilledNum;
    int m_aiActiveScriptKilledConfigID[MAX_CAMP_FIGHT_UNIT_NUM];

    //当前行动单位相关的信息
    int m_iActionUnit;
    int m_iCanCombatUnitNum;
    int m_aiCanCombatUnit[MAX_CAMP_FIGHT_UNIT_NUM*2];
    int m_iActionDoneUnitNum;
    int m_aiActionDoneUnit[MAX_CAMP_FIGHT_UNIT_NUM*2];

    //战场上的机关信息
    int m_iTrapNum;
    int m_aiCombatTrapIndex[MAX_COMBAT_TRAP_NUM];

    //当前行动单位的状态
    int m_iRoleCombatUnitStatus;

	//本次战斗的关卡ID
	int m_iCrossID;

    //本次战斗的地图ID
    int m_iMapID;

    //本次战斗地图的阻挡管理器
    const CScenePathManager* m_pstBattlePathManager;

    //本次战斗的胜负结果
    int m_iCombatResult;

    //设置当前战斗的回合数
    int m_iCombatRoundNum;

    //战斗开始的时间
    int m_iStartTime;

    //本场战斗的状态，供脚本使用
    unsigned char m_ucCombatStatus;

private:

    //BUFF相关推送给客户端的消息
    static GameProtocolMsg m_stRemoveBuff_Notify;               //移除BUFF
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
