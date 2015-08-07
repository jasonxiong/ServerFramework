
#include "GameRole.hpp"
#include "BattlefieldObj.hpp"
#include "ZoneObjectHelper.hpp"
#include "CombatUnitObj.hpp"
#include "ModuleShortcut.hpp"
#include "UnitUtility.hpp"

#include "CombatUtility.hpp"

CBattlefieldObj* CCombatUtility::GetBattlefiledObj(CGameRoleObj& stRoleObj)
{
    return GameType<CBattlefieldObj>::Get(stRoleObj.GetBattlefieObjID());
}

CBattlefieldObj* CCombatUtility::GetBattlefiledObj(int iBattlefieldIndex)
{
    return GameType<CBattlefieldObj>::Get(iBattlefieldIndex);
}

//根据战斗单位ID获取CCombatUnitObj
CCombatUnitObj* CCombatUtility::GetCombatUnitObj(int iCombatUnitID)
{
    return GameType<CCombatUnitObj>::Get(iCombatUnitID);
}

//清理战场
void CCombatUtility::ClearBattlefield(int iBattlefieldIndex)
{
    if(iBattlefieldIndex < 0)
    {
        return;
    }

    //获取战场信息
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(iBattlefieldIndex);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, index %d\n", iBattlefieldIndex);
        return;
    }

    //清理玩家身上的战场ID
    CGameRoleObj* pstActiveRoleObj = CUnitUtility::GetRoleByUin(pstBattlefieldObj->GetActiveUin());
    if(pstActiveRoleObj)
    {
        pstActiveRoleObj->SetBattlefieObjID(-1);
    }

    CGameRoleObj* pstPassiveRoleObj = CUnitUtility::GetRoleByUin(pstBattlefieldObj->GetPassiveUin());
    if(pstPassiveRoleObj)
    {
        pstPassiveRoleObj->SetBattlefieObjID(-1);
    }

    //清理战场
    pstBattlefieldObj->ClearBattlefield();

    //删除战场
    GameType<CBattlefieldObj>::Del(iBattlefieldIndex);

    return;
}

//计算招式命中
int CCombatUtility::GetSkillHit(CCombatUnitObj& stCastUnitObj, const SFightUnitSkillConfig& stSkillConfig, int iDistance)
{
    //读取技能的命中Score表
    const SSkillScoreConfig* pstScoreConfig = SkillScoreCfgMgr().GetConfig(stSkillConfig.iHitModifierID);
    if(!pstScoreConfig)
    {
        LOGERROR("Failed to get skill score config, invalid hit modifier id %d\n", stSkillConfig.iHitModifierID);
        return 0;
    }

    //读取技能的命中Range表
    const SSkillRangeConfig* pstRangeConfig = SkillRangeCfgMgr().GetConfig(stSkillConfig.iTargetRangeID);
    if(!pstRangeConfig)
    {
        LOGERROR("Failed to get skill range config, invalid range id %d\n", stSkillConfig.iTargetRangeID);
        return 0;
    }

    //计算基本命中
    int iBaseHit = CCombatUtility::GetSkillScoreNum(stCastUnitObj, *pstScoreConfig);

    //计算攻方到守方的距离
    int iRangeIndex = iDistance;
    if(iRangeIndex >= MAX_SKILL_RANGE_INFO_NUM)
    {
        //距离MAX_SKILL_RANGE_INFO_NUM特殊含义，这边最大到MAX_SKILL_RANGE_INFO_NUM-1
        iRangeIndex = MAX_SKILL_RANGE_INFO_NUM-1;
    }

    //距离转换成下标需要减1
    iRangeIndex = iRangeIndex - 1;

    //计算招式命中

    return iBaseHit*pstRangeConfig->aiRangeInfo[iRangeIndex]/10000;
}

//计算招式闪避
int CCombatUtility::GetSkillDodge(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig)
{
    //读取技能的命中Score表
    const SSkillScoreConfig* pstScoreConfig = SkillScoreCfgMgr().GetConfig(stSkillConfig.iDodgeModifierID);
    if(!pstScoreConfig)
    {
        LOGERROR("Failed to get skill score config, invalid hit modifier id %d\n", stSkillConfig.iDodgeModifierID);
        return 0;
    }

    //计算基本闪避
    int iBaseDodge = CCombatUtility::GetSkillScoreNum(stTargetUnitObj, *pstScoreConfig);

    return (iBaseDodge);
}

//计算攻击强度
int CCombatUtility::GetAttackStrength(CCombatUnitObj& stCastUnitObj, const SFightUnitSkillConfig& stSkillConfig)
{
    //伤害配置表
    const SSkillDamageConfig* pstDamageConfig = SkillDamageCfgMgr().GetConfig(stSkillConfig.iDamageID);
    if(!pstDamageConfig)
    {
        LOGERROR("Failed to get skill damage config, invalid damage id %d\n", stSkillConfig.iDamageID);

        return 0;
    }

    //Score表
    const SSkillScoreConfig* pstScoreConfig = SkillScoreCfgMgr().GetConfig(pstDamageConfig->iAttack);
    if(!pstScoreConfig)
    {
        LOGERROR("Failed to get skill score config, invalid score id %d\n", pstDamageConfig->iAttack);

        return 0;
    }
    
    //攻击强度

    return CCombatUtility::GetSkillScoreNum(stCastUnitObj, *pstScoreConfig);
}

//计算防御强度
int CCombatUtility::GetDefenceStrength(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig)
{
    //伤害配置表
    const SSkillDamageConfig* pstDamageConfig = SkillDamageCfgMgr().GetConfig(stSkillConfig.iDamageID);
    if(!pstDamageConfig)
    {
        LOGERROR("Failed to get skill damage config, invalid damage id %d\n", stSkillConfig.iDamageID);

        return 0;
    }

    //Score表
    const SSkillScoreConfig* pstScoreConfig = SkillScoreCfgMgr().GetConfig(pstDamageConfig->iDefence);
    if(!pstScoreConfig)
    {
        LOGERROR("Failed to get skill score config, invalid score id %d\n", pstDamageConfig->iDefence);

        return 0;
    }

    //防御强度
    int iDefenceStrength = CCombatUtility::GetSkillScoreNum(stTargetUnitObj, *pstScoreConfig);

    return (iDefenceStrength);
}

//获取敌对单位的攻击距离，为0表示不能攻击
int CCombatUtility::GetAttackDistance( const TUNITPOSITION& stCastPos, const TUNITPOSITION& stTargetPos, int iTargetRangeID)
{
    //先计算攻守方的X,Y距离
    int iDeltaX = stTargetPos.iPosX - stCastPos.iPosX;
    int iDeltaY = stTargetPos.iPosY - stCastPos.iPosY;

    //获取Range配置
    const SSkillRangeConfig* pstRangeConfig = SkillRangeCfgMgr().GetConfig(iTargetRangeID);
    if(!pstRangeConfig)
    {
        LOGERROR("Failed to get target range config, invalid target range id %d\n", iTargetRangeID);
        return 0;
    }

    //遍历Range配置
    for(int i=0; i<pstRangeConfig->iRangeNum; ++i)
    {
        if(pstRangeConfig->aiRangeInfo[i] == 0)
        {
            //该位置的目标打不到
            continue;
        }

        int iAreaID = i+1+pstRangeConfig->iRangeAreaParam;

        //读取Area表配置
        const SSkillAreaConfig* pstAreaConfig = SkillAreaCfgMgr().GetConfig(iAreaID);
        if(!pstAreaConfig)
        {
            LOGERROR("Failed to get skill area config, invalid area id %d\n", iAreaID);
            return 0;
        }

        //遍历Area中的坐标位置
        for(int j=0; j<pstAreaConfig->iAreaPosNum; ++j)
        {
            if(pstAreaConfig->astTargetPosInfo[j].iPosX==iDeltaX
               && pstAreaConfig->astTargetPosInfo[j].iPosY==iDeltaY)
            {
                return (i+1);
            }
        }
    }

    return 0;
}

//根据Score表计算修正值
int CCombatUtility::GetSkillScoreNum(CCombatUnitObj& stCombatUnitObj, const SSkillScoreConfig& stScoreConfig)
{
    //计算基本Score
    int iBaseScore = stScoreConfig.iScoreBaseNum;
    for(int i=0; i<FIGHT_ATTR_MAX; ++i)
    {
        //乘的参数是百分比，下面需要除100
        iBaseScore += stScoreConfig.aiAttribute[i] * stCombatUnitObj.GetFightAttr(i);
    }

    //iScoreParam是百分比，需要除100
    return iBaseScore*stScoreConfig.iScoreParam/(100*100);
}

//更新战斗单位的方向
void CCombatUtility::UpdateUnitDirection(CCombatUnitObj& stCastUnitObj, const TUNITPOSITION& stTargetPos)
{
    if(stCastUnitObj.GetUnitPosition().iPosX==stTargetPos.iPosX
       && stCastUnitObj.GetUnitPosition().iPosY==stTargetPos.iPosY)
    {
        //如果目标是自身，不转向
        return;
    }

    int iNewDirection = GetAttackDirection(stCastUnitObj, stTargetPos);
    if(iNewDirection > 0)
    {
        stCastUnitObj.SetUnitDirection(iNewDirection);
    }

    return;
}

//获取战斗单位攻击的方向, <0 表示出错
int CCombatUtility::GetAttackDirection(CCombatUnitObj& stCastUnitObj, const TUNITPOSITION& stTargetPos)
{
    const TUNITPOSITION& stCastPos = stCastUnitObj.GetUnitPosition();

    int iDeltaX = stTargetPos.iPosX - stCastPos.iPosX;
    int iDeltaY = stTargetPos.iPosY - stCastPos.iPosY;

    if(iDeltaX==0 && iDeltaY==0)
    {
        return -1;
    }
    else if(iDeltaY>0 && ABS(iDeltaY,0)>=ABS(iDeltaX,0))
    {
        return COMBAT_DIRECTION_WEST;
    }
    else if(iDeltaY<0 && ABS(iDeltaY,0)>=ABS(iDeltaX,0))
    {
        return COMBAT_DIRECTION_EAST;
    }
    else if(iDeltaX>0 && ABS(iDeltaX,0)>ABS(iDeltaY,0))
    {
        return COMBAT_DIRECTION_NORTH;
    }
    else if(iDeltaX<0 && ABS(iDeltaX,0)>ABS(iDeltaY,0))
    {
        return COMBAT_DIRECTION_SOUTH;
    }

    return -1;
}

//修正技能伤害范围的坐标
void CCombatUtility::FixSkillAreaPos(int iCastDirection, const SFightPosInfo& stTargetPos, TUNITPOSITION& stFixedPos)
{
    switch(iCastDirection)
    {
        case COMBAT_DIRECTION_EAST:
            {
                stFixedPos.iPosX = stTargetPos.iPosY;
                stFixedPos.iPosY = -stTargetPos.iPosX;
            }
            break;

        case COMBAT_DIRECTION_NORTH:
            {
                stFixedPos.iPosX = stTargetPos.iPosX;
                stFixedPos.iPosY = stTargetPos.iPosY;
            }
            break;

        case COMBAT_DIRECTION_WEST:
            {
                stFixedPos.iPosX = stTargetPos.iPosY;
                stFixedPos.iPosY = stTargetPos.iPosX;
            }
            break;

        case COMBAT_DIRECTION_SOUTH:
            {
                stFixedPos.iPosX = -stTargetPos.iPosX;
                stFixedPos.iPosY = stTargetPos.iPosY;
            }
            break;

        default:
            {
                stFixedPos.iPosX = 0;
                stFixedPos.iPosY = 0;
            }
            break;
    }

    return;
}

//处理战斗单位的特殊位移
int CCombatUtility::ProcessUnitMove(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stUnitObj, UnitPosition& stPosNotify, int iAreaID, int iDirection, bool bStop)
{
    if(stUnitObj.GetUnitSize() != 0)
    {
        //大体积单位不支持特殊位移
        return 0;
    }

    const SSkillAreaConfig* pstAreaConfig = SkillAreaCfgMgr().GetConfig(iAreaID);
    if(!pstAreaConfig)
    {
        LOGERROR("Failed to get skill area config, invalid id %d\n", iAreaID);
        return -1;
    }
    
    bool bCanMove = false;
    TUNITPOSITION stTmpPos;
    TUNITPOSITION stCanMovePos;
    for(int i=0; i<pstAreaConfig->iAreaPosNum; ++i)
    {
        //根据方向修正配置的坐标
        CCombatUtility::FixSkillAreaPos(iDirection,pstAreaConfig->astTargetPosInfo[i], stTmpPos);

        stTmpPos.iPosX += stUnitObj.GetUnitPosition().iPosX;
        stTmpPos.iPosY += stUnitObj.GetUnitPosition().iPosY;

        //该位置不能行走并且需要停下来
        if(!stBattlefieldObj.IsPosCanWalk(stTmpPos,-1) && bStop)
        {
            //不检查后续位置
            break;
        }

        if(stBattlefieldObj.IsPosCanWalk(stTmpPos,-1))
        {
            stCanMovePos.iPosX = stTmpPos.iPosX;
            stCanMovePos.iPosY = stTmpPos.iPosY;

            bCanMove = true;
        }
    }

    if(bCanMove)
    {
        stUnitObj.SetUnitPosition(stCanMovePos);

        stPosNotify.set_iposx(stCanMovePos.iPosX);
        stPosNotify.set_iposy(stCanMovePos.iPosY);
    }
    else
    {
        //移动到当前位置
        stPosNotify.set_iposx(stUnitObj.GetUnitPosition().iPosX);
        stPosNotify.set_iposy(stUnitObj.GetUnitPosition().iPosY);
    }

    //当前位置如果是特殊阻挡，则杀死单位
    if(stBattlefieldObj.IsPosCanWalk(stUnitObj.GetUnitPosition(), stUnitObj.GetCombatUnitID()) 
       && !stBattlefieldObj.IsPosCanWalk(stUnitObj.GetUnitPosition(), stUnitObj.GetCombatUnitID()))
    {
        //特殊阻挡位置,杀死单位
        stBattlefieldObj.KillCombatUnit(stUnitObj.GetCombatUnitID(), false);
    }

    return T_SERVER_SUCESS;
}

//玩家当前是否可以进行PVP战斗
bool CCombatUtility::CanDoPVPCombat(unsigned int uin)
{
    CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(uin);
    if(!pstRoleObj)
    {
        return false;
    }

    //玩家当前是否在战斗中
    if(pstRoleObj->GetBattlefieObjID() >= 0)
    {
        return false;
    }

    return true;
}

//todo jasonxiong5 后面需要修改为物品
//根据武器生效状态处理武器效果，bIsAram表示是否生效武器
void CCombatUtility::ProcessWeaponEffect(CCombatUnitObj& stUnitObj, bool bIsAram)
{
    CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(stUnitObj.GetUin());
    if(!pstRoleObj)
    {
        return;
    }

    //获取战斗单位信息
    CFightUnitObj* pstFightUnitObj = pstRoleObj->GetFightUnitManager().GetFightUnitByID(stUnitObj.GetConfigID());
    if(!pstFightUnitObj)
    {
        return;
    }

    /*
    //获取武器信息
    const SEquipmentConfig* pstEquipConfig = EquipmentCfgMgr().GetConfig(pstFightUnitObj->GetItemInfo(0).iItemID);
    if(!pstEquipConfig)
    {
        return;
    }

    //获取战场
    CBattlefieldObj* pstBattlefiledObj = CCombatUtility::GetBattlefiledObj(*pstRoleObj);
    if(!pstBattlefiledObj)
    {
        return;
    }

    //处理武器效果
    if(bIsAram)
    {
        for(int i=0; i<FIGHT_ATTR_MAX; ++i)
        {
            pstBattlefiledObj->AddCombatUnitAttr(stUnitObj, i, pstEquipConfig->aiAttribute[i]);
        }
    }
    else
    {
        for(int i=0; i<FIGHT_ATTR_MAX; ++i)
        {
            pstBattlefiledObj->AddCombatUnitAttr(stUnitObj, i, -pstEquipConfig->aiAttribute[i]);
        }
    }
    */

    return;
}

//获取坐标范围内某阵营的单位
int CCombatUtility::GetAreaTargetUnits(unsigned uin, const TUNITPOSITION& stPos, int iAreaID, int iCamp, int iDirection, std::vector<int>& vUnits)
{
    vUnits.clear();

    CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(uin);
    if(!pstRoleObj)
    {
        LOGERROR("Failed to get game role obj, uin %u\n", uin);
        return -1;
    }

    CBattlefieldObj* pstBattlefieldObj = GetBattlefiledObj(*pstRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, uin %u\n", uin);
        return -2;
    }

    //读取Area表
    const SSkillAreaConfig* pstAreaConfig = SkillAreaCfgMgr().GetConfig(iAreaID);
    if(!pstAreaConfig)
    {
        LOGERROR("Failed to get skill area config, invalid area id %d\n", iAreaID);
        return -3;
    }

    //遍历Area中的所有位置
    TUNITPOSITION stTmpPos;
    for(int j=0; j<pstAreaConfig->iAreaPosNum; ++j)
    {
        //根据方向修正配置的坐标
        CCombatUtility::FixSkillAreaPos(iDirection, pstAreaConfig->astTargetPosInfo[j], stTmpPos);
        stTmpPos.iPosX += stPos.iPosX;
        stTmpPos.iPosY += stPos.iPosY;

        //获取战斗单位
        CCombatUnitObj* pstUnitObj = pstBattlefieldObj->GetCombatUnitByPos(stTmpPos);
        if(pstUnitObj && pstBattlefieldObj->GetCombatUnitCamp(pstUnitObj->GetCombatUnitID())==iCamp)
        {
            vUnits.push_back(pstUnitObj->GetCombatUnitID());
        }
    }

    return 0;
}

