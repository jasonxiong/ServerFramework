
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "ModuleShortcut.hpp"
#include "FightUnitManager.hpp"
#include "GameRole.hpp"
#include "ZoneMsgHelper.hpp"
#include "UnitUtility.hpp"
#include "Random.hpp"

#include "CombatUnitObj.hpp"

using namespace ServerLib;
using namespace GameConfig;

IMPLEMENT_DYN(CCombatUnitObj)

CCombatUnitObj::CCombatUnitObj()
{
    Initialize();
}

CCombatUnitObj::~CCombatUnitObj()
{
    //ClearUnitBuff();
}

int CCombatUnitObj::Initialize()
{
    m_iCombatUnitType = COMBAT_UNIT_TYPE_INVALID;
    m_iCombatUnitID = -1;
    m_iConfigID = 0;

    m_iSize = 0;
    m_stPos.iPosX = 0;
    m_stPos.iPosY = 0;
    m_iDirection = 0;

    memset(m_aiAttributes, 0, sizeof(m_aiAttributes));

    //技能
    m_iNormalSkill = 0;
    memset(m_astUnitItems, 0, sizeof(m_astUnitItems));
    memset(m_aiCDRounds, 0, sizeof(m_aiCDRounds));

    //战斗单位AI
    m_iUnitAIID = 0;

    //Buffer管理器
    m_stBuffManager.Initialize();

    //最后攻击的单位ID
    m_iLastAttackUnitID = -1;

    //战斗单位的状态
    m_ucUnitStatus = 0;

    //单位机动模式相关
    m_stMotorTargetPos.iPosX = -1;
    m_stMotorTargetPos.iPosY = 0;

    m_iMotorDistance = -1;

    //战斗单位所属玩家
    m_uiUin = 0;

    return T_SERVER_SUCESS;
}

//初始化怪物战斗对象
int CCombatUnitObj::InitMonsterUnitObj(int iCombatUnitObjIndex, const OneCrossMonster& stOneConfig, const SFightPosInfo& stPosInfo, int iUnitType)
{
    m_iCombatUnitType = iUnitType;
    m_iConfigID = stOneConfig.iMonsterID;
    m_iCombatUnitID = iCombatUnitObjIndex;

    //设置没有机动模式
    m_iMotorDistance = -1;

    m_iLastAttackUnitID = -1;

    //读取怪物配置
    const SMonsterConfig* pstMonsterConfig = MonsterCfgMgr().GetConfig(m_iConfigID);
    if(!pstMonsterConfig)
    {
        LOGERROR("Failed to get monster config, invalid id %d\n", m_iConfigID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    m_iSize = pstMonsterConfig->iSize;

    //设置怪物的基础属性
    for(int i=0; i<FIGHT_ATTR_MAX; ++i)
    {
        m_aiAttributes[i] = pstMonsterConfig->aiAttribute[i];
    }

    //设置怪物技能信息
    m_iNormalSkill = pstMonsterConfig->iNormalSkill;

    //设置怪物物品信息
    for(int i=0; i<MAX_UNIT_ITEM_SLOT; ++i)
    {
        m_astUnitItems[i].iItemID = stOneConfig.aiItemInfo[i];
    }

    memset(m_aiCDRounds, 0, sizeof(m_aiCDRounds));

    //设置怪物使用的AI
    m_iUnitAIID = pstMonsterConfig->iMonsterAIID;

    //设置怪物的起始位置信息
    m_stPos.iPosX = stPosInfo.iPosX;
    m_stPos.iPosY = stPosInfo.iPosY;
    m_iDirection = stPosInfo.iDirection;

    //BUFF管理器
    m_stBuffManager.SetOwnerUnitID(m_iCombatUnitID);

    return T_SERVER_SUCESS;
}

//初始化角色战斗对象
int CCombatUnitObj::InitRoleUnitObj(int iCombatUnitObjIndex, CGameRoleObj& stRoleObj, int iFightUnitID, const SFightPosInfo& stPosInfo)
{
    //设置战斗对象类型
    if(iFightUnitID == 1)
    {
        //默认Index为0的位置是主角
        m_iCombatUnitType = COMBAT_UNIT_TYPE_ROLE;
    }
    else
    {
        //其他的是伙伴
        m_iCombatUnitType = COMBAT_UNIT_TYPE_PARTNER;
    }

    m_uiUin = stRoleObj.GetUin();

    //获取战斗单位管理器
    CFightUnitManager& rstFightUnitManager = stRoleObj.GetFightUnitManager();

    //获取战斗单位信息
    CFightUnitObj* pstFightUnitObj = rstFightUnitManager.GetFightUnitByID(iFightUnitID);
    if(!pstFightUnitObj)
    {
        LOGERROR("Failed to get fight unit obj, index %d, uin %u\n", iFightUnitID, stRoleObj.GetUin());
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //设置战斗单位唯一ID
    m_iCombatUnitID = iCombatUnitObjIndex;

    //设置配置的ID
    m_iConfigID = pstFightUnitObj->GetFightUnitID();

    //设置战斗单位的AI
    m_iUnitAIID = pstFightUnitObj->GetFightUnitAIID();

    //设置战斗对象在战场上的位置
    m_stPos.iPosX = stPosInfo.iPosX;
    m_stPos.iPosY = stPosInfo.iPosY;
    m_iDirection = stPosInfo.iDirection;

    //设置没有机动模式
    m_iMotorDistance = -1;

    m_iLastAttackUnitID = -1;

    //设置战斗对象的属性
    int iRet = pstFightUnitObj->InitFightAttr(m_aiAttributes, FIGHT_ATTR_MAX);
    if(iRet)
    {
        LOGERROR("Failed to init combat unit attr, unit index %d, ret %d, uin %u\n", iFightUnitID, iRet, stRoleObj.GetUin());
        return iRet;
    }

    //设置战斗单位普攻和物品
    m_iNormalSkill = pstFightUnitObj->GetNormalSkill();

    pstFightUnitObj->InitFightUnitItem(m_astUnitItems, sizeof(m_astUnitItems));

    memset(m_aiCDRounds, 0, sizeof(m_aiCDRounds));

    //设置BUFF管理器
    m_stBuffManager.SetOwnerUnitID(m_iCombatUnitID);

    return T_SERVER_SUCESS;
}

//位置相关
void CCombatUnitObj::SetUnitPosition(const TUNITPOSITION& rstUnitPos)
{
    m_stPos.iPosX = rstUnitPos.iPosX;
    m_stPos.iPosY = rstUnitPos.iPosY;

    return;
}

TUNITPOSITION& CCombatUnitObj::GetUnitPosition()
{
    return m_stPos;
}

//清理战斗单位的Buff信息
void CCombatUnitObj::ClearUnitBuff()
{
    m_stBuffManager.ClearBuffObj();
}

//打包返回战斗对象信息
void CCombatUnitObj::PackCombatUnitInfo(int iCamp, CombatUnit& stUnitInfo)
{
    //战斗单位类型
    stUnitInfo.set_etype((CombatUnitType)m_iCombatUnitType);

    //战斗单位的唯一ID
    stUnitInfo.set_iunitid(m_iCombatUnitID);

    //战斗单位所属的阵营
    stUnitInfo.set_icamp(iCamp);

    //战斗单位的配置ID
    stUnitInfo.set_iconfigid(m_iConfigID);

    //战斗单位战场上的位置
    UnitPosition* pstPos = stUnitInfo.mutable_stpos();
    pstPos->set_iposx(m_stPos.iPosX);
    pstPos->set_iposy(m_stPos.iPosY);
    
    //战斗单位战场上的方向
    stUnitInfo.set_idirection(m_iDirection);

    //设置战斗单位的属性
    for(int i=0; i<FIGHT_ATTR_MAX; ++i)
    {
        stUnitInfo.add_iattributes(m_aiAttributes[i]);
    }

    //设置战斗单位的AI
    stUnitInfo.set_iunitaiid(m_iUnitAIID);

    //设置单位普攻和物品
    stUnitInfo.set_inormalskill(m_iNormalSkill);

    for(int i=0; i<MAX_UNIT_ITEM_SLOT; ++i)
    {
        OneSlotInfo* pstOneItem = stUnitInfo.add_stitems();
        pstOneItem->set_iitemid(m_astUnitItems[i].iItemID);
        pstOneItem->set_iitemnum(m_astUnitItems[i].iItemNum);
    }

    return;
}

//战斗单位是否死亡
bool CCombatUnitObj::IsCombatUnitDead()
{
    return (m_aiAttributes[FIGHT_ATTR_HP] <= 0);
}

//获取战斗单位的战斗属性
int CCombatUnitObj::GetFightAttr(int iAttrType)
{
    if(iAttrType<0 || iAttrType>=FIGHT_ATTR_MAX)
    {
        return 0;
    }

    return m_aiAttributes[iAttrType];
}

//增加战斗单位的战斗属性
int CCombatUnitObj::AddFightAttr(int iAttrType, int iAddNum, int* iRealAddNum)
{
    if(iAddNum == 0)
    {
        return T_SERVER_SUCESS;
    }

    if(iAttrType<0 || iAttrType>=FIGHT_ATTR_MAX)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    if(iRealAddNum)
    {
        *iRealAddNum = -m_aiAttributes[iAttrType];
    }

    m_aiAttributes[iAttrType] += iAddNum;
    if(m_aiAttributes[iAttrType] < 0)
    {
        m_aiAttributes[iAttrType] = 0;
    }

    //如果是血量，不能超过当前最大血量
    if(iAttrType==FIGHT_ATTR_HP && m_aiAttributes[iAttrType]>m_aiAttributes[FIGHT_ATTR_HPMAX])
    {
        m_aiAttributes[iAttrType] = m_aiAttributes[FIGHT_ATTR_HPMAX];

    }

    //如果战斗单位血量为0
    if(iAttrType==FIGHT_ATTR_HP && m_aiAttributes[iAttrType]==0)
    {
        if(GetCombatUnitStatus(COMBAT_UNIT_STATUS_UNBENDING))
        {
            //有不屈状态，处理不屈
            SetCombatUnitStatus(COMBAT_UNIT_STATUS_UNBENDING, false);

            m_aiAttributes[iAttrType] = 1;
        }
        else
        {
            //从战场上删除
            m_stPos.iPosX = -1;
        }
    }

    if(iRealAddNum)
    {
        *iRealAddNum += m_aiAttributes[iAttrType];
    }

    return T_SERVER_SUCESS;
}

//获取战斗单位的类型
int CCombatUnitObj::GetCombatUnitType()
{
    return m_iCombatUnitType;
}

//获取战斗单位的ID
int CCombatUnitObj::GetCombatUnitID()
{
    return m_iCombatUnitID;
}

//获取战斗单位身上所有buff的配置ID
void CCombatUnitObj::GetUnitBuffID(std::vector<int>& vBuffIDs)
{
    return m_stBuffManager.GetUnitBuffID(vBuffIDs);
}

//是否有相同ID的BUFF
bool CCombatUnitObj::HasBuffOfSameID(int iBuffID)
{
    return m_stBuffManager.HasBuffOfSameID(iBuffID);
}

void CCombatUnitObj::SetUnitDirection(int iDirection)
{
    m_iDirection = iDirection;
}

//获取战斗单位的方向
int CCombatUnitObj::GetUnitDirection()
{
    return m_iDirection;
}

//设置战斗单位的AI
int CCombatUnitObj::SetFightAI(int iFightAIID)
{
    //判断是否是合法的AI ID
    const SFightUnitConfig* pstUnitConfig = FightUnitCfgMgr().GetConfig(m_iConfigID);
    if(!pstUnitConfig)
    {
        LOGERROR("Failed to set fight ai , invalid unit config id %d\n", m_iConfigID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    for(int i=0; i<pstUnitConfig->iAINum; ++i)
    {
        if(iFightAIID == pstUnitConfig->aiUnitAIID[i])
        {
            m_iUnitAIID = iFightAIID;

            return T_SERVER_SUCESS;
        }
    }

    return T_ZONE_SYSTEM_PARA_ERR;
}

int CCombatUnitObj::ChangeFightAI(int iFightAIID)
{
    //判断该AI是否合法
    const SFightUnitAIConfig* pstAIConfig = FightUnitAICfgMgr().GetConfig(iFightAIID);
    if(!pstAIConfig)
    {
        LOGERROR("Failed to get fight unit AI config, invalid AI id %d\n", iFightAIID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    m_iUnitAIID = iFightAIID;

    return T_SERVER_SUCESS;
}

int CCombatUnitObj::GetFightAI()
{
    return m_iUnitAIID;
}

void CCombatUnitObj::GetValidFightSkills(std::vector<int>& vValidSkills)
{
    vValidSkills.clear();

    //先放入普攻
    vValidSkills.push_back(m_iNormalSkill);

    //todo jasonxiong5 这边后面放入物品增加的技能信息

    return;
}

int CCombatUnitObj::GetNormalSkillID()
{
    return m_iNormalSkill;
}

void CCombatUnitObj::GetSkillByType(int iSkillType, std::vector<int>& vSkillIDs)
{
    //todo jasonxiong5 这边类型确认下怎么分
    /*
    for(int i=0; i<m_iSkillNum; ++i)
    {
        const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(m_aiSkills[i]);
        if(!pstSkillConfig)
        {
            //LOGERROR("Failed to get fight skill, skill id %d\n", m_astSkills[i].iSkillID);
            break;
        }

        if(pstSkillConfig->iSkillType == iSkillType)
        {
            vSkillIDs.push_back(m_aiSkills[i]);
        }
    }
    */

    return;
}

//最后攻击的单位ID
int CCombatUnitObj::GetLastAttackUnitID()
{
    return m_iLastAttackUnitID;
}

void CCombatUnitObj::SetLastAttackUnitID(int iUnitID)
{
    m_iLastAttackUnitID = iUnitID;
}

//BUFF相关
int CCombatUnitObj::AddUnitBuff(unsigned int uin, int iCrossID, int iBuffID, int iCastUnitID, Zone_CombatAddBuff_Notify& stNotify)
{
    if(IsCombatUnitDead())
    {
        //单位已经死亡，不能加BUFF
        return T_SERVER_SUCESS;
    }

    return m_stBuffManager.AddUnitBuff(uin, iCrossID, iBuffID, iCastUnitID, stNotify);
}

int CCombatUnitObj::DoBuffEffectByType(unsigned int uin, int iCrossID, int iTriggerType, int iTriggerUnitID, Zone_DoBuffEffect_Notify& stNotify, int* pDamageNum)
{
    return m_stBuffManager.DoUnitBuffEffect(uin, iCrossID, iTriggerType, iTriggerUnitID, stNotify, pDamageNum);
}

//减少BUFF的回合数
int CCombatUnitObj::DecreaseBuffRound(Zone_RemoveBuff_Notify& stNotify)
{
    return m_stBuffManager.DecreaseAllBuffRound(stNotify);
}

bool CCombatUnitObj::HasBuffOfSameType(int iBuffID)
{
    return m_stBuffManager.HasBuffOfSameType(iBuffID);
}

//删除战斗单位身上的BUFF
void CCombatUnitObj::DelUnitBuff(int iBuffID, RemoveBuffEffect& stEffect)
{
    m_stBuffManager.DelUnitBuffByID(iBuffID, stEffect);

    return;
}

//获取配置的单位ID
int CCombatUnitObj::GetConfigID()
{
    return m_iConfigID;
}

//获取配置单位的Size
int CCombatUnitObj::GetUnitSize()
{
    return m_iSize;
}

//获取反击技能的ID
int CCombatUnitObj::GetCounterAtkSkill()
{
    switch(m_iCombatUnitType)
    {
        case COMBAT_UNIT_TYPE_MONSTER:
            {
                //怪物表
                const SMonsterConfig* pstMonsterConfig = MonsterCfgMgr().GetConfig(m_iConfigID);
                if(!pstMonsterConfig)
                {
                    LOGERROR("Failed to get monster config, monster id %d\n", m_iConfigID);
                    return 0;
                }

                return pstMonsterConfig->iCounterAtkSkillID;
            }
            break;

        default:
            {
                //角色单位表
                const SFightUnitConfig* pstUnitConfig = FightUnitCfgMgr().GetConfig(m_iConfigID);
                if(!pstUnitConfig)
                {
                    LOGERROR("Failed to get fight unit config, unit id %d\n", m_iConfigID);
                    return 0;
                }

                return pstUnitConfig->iCounterAtkSkillID;
            }
            break;
    }

    return 0;
}

//判断战斗单位是否拥有该技能
bool CCombatUnitObj::HasFightSkill(int iSkillID)
{
    //todo jasonxiong5 后面确认下物品相关的怎么搞
    return (m_iNormalSkill==iSkillID);
}

//战斗单位身上的状态
void CCombatUnitObj::SetCombatUnitStatus(int iType, bool bSet)
{
    if(bSet)
    {
        //设置状态
        m_ucUnitStatus = (m_ucUnitStatus | (0x01<<iType));
    }
    else
    {
        //清除状态
        m_ucUnitStatus = (m_ucUnitStatus & ~(0x01<<iType));
    }

    return;
}

bool CCombatUnitObj::GetCombatUnitStatus(int iType)
{
    return ((m_ucUnitStatus>>iType) & 0x01);
}

//获取战斗单位所属的uin
unsigned int CCombatUnitObj::GetUin()
{
    return m_uiUin;
}

//战斗单位机动模式
void CCombatUnitObj::SetMotorMode(const TUNITPOSITION& stTargetPos, int iDistance, int iNewAIID)
{
    m_stMotorTargetPos = stTargetPos;
    
    m_iMotorDistance = iDistance;

    if(iNewAIID != 0)
    {
        m_iUnitAIID = iNewAIID;
    }

    return;
}

const TUNITPOSITION& CCombatUnitObj::GetMotorTargetPos()
{
    return m_stMotorTargetPos;
}

int CCombatUnitObj::GetMotorDistance()
{
    return m_iMotorDistance;
}

int CCombatUnitObj::GetMotorLeftLen()
{
    return ABS(m_stMotorTargetPos.iPosX,m_stPos.iPosX) + ABS(m_stMotorTargetPos.iPosY,m_stPos.iPosY);
}

//单位技能的CD状态
int CCombatUnitObj::GetSkillCDRound(int iSkillID)
{
    //todo jasonxiong5 先保留，后面看下怎么用

    return 0;
}

void CCombatUnitObj::SetSkillCDRound(int iSkillID, int iRound)
{
    //todo jasonxiong5 先保留，后面看下怎么用

    return;
}

void CCombatUnitObj::DecreaseSkillCDRound(Zone_SkillCDRound_Notify& stNotify)
{
    stNotify.set_iunitid(m_iCombatUnitID);

    //todo jasonxiong5 先保留，后面看下怎么用
    /*
    for(int i=0; i<m_iSkillNum; ++i)
    {
        if(m_aiSkills[i]!=0 && m_aiCDRounds[i]>0)
        {
            --m_aiCDRounds[i];

            SkillCDInfo* pstInfo = stNotify.add_stcdinfos();
            pstInfo->set_iskillid(m_aiSkills[i]);
            pstInfo->set_icdround(m_aiCDRounds[i]);
        }
    }
    */

    return;
}

