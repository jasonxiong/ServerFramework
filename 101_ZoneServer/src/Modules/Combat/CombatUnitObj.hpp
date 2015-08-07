

#ifndef __COMBAT_UNIT_OBJ_HPP__
#define __COMBAT_UNIT_OBJ_HPP__

#include <vector>

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"
#include "ZoneObjectAllocator.hpp"
#include "UnitBufferManager.hpp"

#include "FightUnitObj.hpp"

class CGameRoleObj;

//战斗单位对象
class CCombatUnitObj : public CObj
{
public:
    CCombatUnitObj();
    virtual ~CCombatUnitObj();
    virtual int Initialize();

    DECLARE_DYN

public:

    //初始化怪物战斗对象
    int InitMonsterUnitObj(int iCombatUnitObjIndex, const OneCrossMonster& stOneConfig, const SFightPosInfo& stPosInfo, int iUnitType);

    //初始化角色战斗对象
    int InitRoleUnitObj(int iCombatUnitObjIndex, CGameRoleObj& stRoleObj, int iFightUnitID, const SFightPosInfo& stPosInfo);

    //位置相关
    void SetUnitPosition(const TUNITPOSITION& rstUnitPos);
    TUNITPOSITION& GetUnitPosition();

    //清理战斗单位的Buff信息
    void ClearUnitBuff();
    
    //打包返回战斗对象信息
    void PackCombatUnitInfo(int iCamp, CombatUnit& stUnitInfo);

    //战斗单位是否死亡
    bool IsCombatUnitDead();

    //获取战斗单位的战斗属性
    int GetFightAttr(int iAttrType);
    int AddFightAttr(int iAttrType, int iAddNum, int* iRealAddNum = NULL);

    //获取战斗单位的类型
    int GetCombatUnitType();

    //获取战斗单位的ID
    int GetCombatUnitID();

    //获取战斗单位身上所有buff的配置ID
    void GetUnitBuffID(std::vector<int>& vBuffIDs);

    //是否有相同ID的BUFF
    bool HasBuffOfSameID(int iBuffID);

    //战斗单位的方向
    void SetUnitDirection(int iDirection);
    int GetUnitDirection();

    //设置战斗单位的AI
    int SetFightAI(int iFightAIID);
    int ChangeFightAI(int iFightAIID);
    int GetFightAI();

    //技能相关
    void GetValidFightSkills(std::vector<int>& vValidSkills);
    int GetNormalSkillID();
    void GetSkillByType(int iSkillType, std::vector<int>& vSkillIDs);

    //最后攻击的单位ID
    int GetLastAttackUnitID();
    void SetLastAttackUnitID(int iUnitID);

    //BUFF相关
    int AddUnitBuff(unsigned int uin, int iCrossID, int iBuffID, int iCastUnitID, Zone_CombatAddBuff_Notify& stNotify);
    int DoBuffEffectByType(unsigned int uin, int iCrossID, int iTriggerType, int iTriggerUnitID, Zone_DoBuffEffect_Notify& stNotify, int* pDamageNum = NULL);
    int DecreaseBuffRound(Zone_RemoveBuff_Notify& stNotify);
    bool HasBuffOfSameType(int iBuffID);
    void DelUnitBuff(int iBuffID, RemoveBuffEffect& stEffect);

    //获取配置的单位ID
    int GetConfigID();

    //获取配置单位的Size
    int GetUnitSize();

    //获取反击技能的ID
    int GetCounterAtkSkill();

    //判断战斗单位是否拥有该技能
    bool HasFightSkill(int iSkillID);

    //战斗单位身上的状态
    void SetCombatUnitStatus(int iType, bool bSet);
    bool GetCombatUnitStatus(int iType);

    //获取战斗单位所属的uin
    unsigned int GetUin();

    //战斗单位机动模式，iDistance=-1表示取消机动模式
    void SetMotorMode(const TUNITPOSITION& stTargetPos, int iDistance, int iNewAIID = 0);

    //获取战斗单位机动模式
    const TUNITPOSITION& GetMotorTargetPos();
    int GetMotorDistance();
    int GetMotorLeftLen();

    //单位技能的CD状态
    int GetSkillCDRound(int iSkillID);
    void SetSkillCDRound(int iSkillID, int iRound);
    void DecreaseSkillCDRound(Zone_SkillCDRound_Notify& stNotify);

///////////////////////////////////////////////////////////////////////
private:

    //战斗单位的类型
    int m_iCombatUnitType;

    //战场上战斗单位的唯一ID
    int m_iCombatUnitID;

    //战斗单位配置的ID
    int m_iConfigID;

    //战斗单位的体积大小
    int m_iSize;

    //战斗单位在战场上的位置
    TUNITPOSITION m_stPos;

    //战斗单位在战场上的朝向
    int m_iDirection;
        
    //战斗单位的属性
    int m_aiAttributes[FIGHT_ATTR_MAX];

    //战斗单位的技能
    int m_iNormalSkill;
    RepItem m_astUnitItems[MAX_UNIT_ITEM_SLOT];

    int m_aiCDRounds[MAX_FIGHT_CD_NUM];

    //战斗单位的AI
    int m_iUnitAIID;

    //战斗单位身上的Buff信息
    CUnitBufferManager m_stBuffManager;

    //最后攻击的单位ID
    int m_iLastAttackUnitID;

    //战斗单位当前的状态,每个bit表示对应状态是否被设置
    unsigned char m_ucUnitStatus;

    //单位机动模式移动目标位置
    TUNITPOSITION m_stMotorTargetPos;

    //单位机动模式移动截至距离
    int m_iMotorDistance;

    //战斗单位所属玩家uin,怪为0
    unsigned int m_uiUin;
};

#endif
