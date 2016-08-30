
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "BattlefieldObj.hpp"
#include "CombatUnitObj.hpp"
#include "CombatUtility.hpp"
#include "Random.hpp"
#include "AStarPathUtility.hpp"
#include "ModuleHelper.hpp"

#include "CombatUnitAI.hpp"

using namespace ServerLib;

int CCombatUnitAI::m_iBattlefiledObjID = -1;
int CCombatUnitAI::m_uiActiveUin = 0;
int CCombatUnitAI::m_iActionUnitID = -1;
int CCombatUnitAI::m_iMapID = 0;
int CCombatUnitAI::m_iUnitAIID = 0;
int CCombatUnitAI::m_iUseSkillID = 0;

//战斗AI有效移动点数量
int CCombatUnitAI::m_iValidPosNum = 0;
FightAIPosValue CCombatUnitAI::m_astValidPos[MAX_FIGHT_AI_POS_NUM];
int CCombatUnitAI::m_iMovePosIndex = 0;

CCombatUnitAI::CCombatUnitAI()
{

}

CCombatUnitAI::~CCombatUnitAI()
{

}

int CCombatUnitAI::DoActionUnitAI(int iBattlefiledObjID, int iActionUnitID)
{
    m_iBattlefiledObjID = iBattlefiledObjID;
    m_iActionUnitID = iActionUnitID;

    m_uiActiveUin = 0;
    m_iMapID = 0;
    m_iUnitAIID = 0;
    m_iUseSkillID = 0;
    
    m_iValidPosNum = 0;
    memset(m_astValidPos, 0, sizeof(m_astValidPos));
    m_iMovePosIndex = 0;

    //获取战场信息
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(m_iBattlefiledObjID);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, invalid battlefield id %d\n", m_iBattlefiledObjID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //获取主动行动者的uin
    m_uiActiveUin = pstBattlefieldObj->GetActiveUin();

    //获取战场地图文件ID
    m_iMapID = pstBattlefieldObj->GetMapID();

    //获取当前行动的战斗单位信息
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnitID);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get combat unit obj, unit id %d, uin %u\n", m_iActionUnitID, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    if(pstActionUnitObj->GetMotorDistance() >= 0)
    {
        //战斗单位当前开启机动模式
        return T_SERVER_SUCESS;
    }

    //设置行动AI的ID
    m_iUnitAIID = pstActionUnitObj->GetFightAI();

    //读取行动AI的配置
    const SFightUnitAIConfig* pstAIConfig = FightUnitAICfgMgr().GetConfig(m_iUnitAIID);
    if(!pstAIConfig)
    {
        LOGERROR("Failed to get fight unit AI config, invalid AI id %d\n", m_iUnitAIID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //获取单位的所有可移动位置
    int iRet = GetAllValidMovePos(*pstBattlefieldObj, *pstActionUnitObj);
    if(iRet)
    {
        LOGERROR("Failed to get all valid move pos, ret %d, uin %u\n", iRet, m_uiActiveUin);
        return iRet;
    }

    //计算所有可移动位置的攻击价值
    iRet = ProcessAttackValue(*pstBattlefieldObj, *pstActionUnitObj, *pstAIConfig);
    if(iRet)
    {
        LOGERROR("Failed to process attack value, ret %d, uin %u\n", iRet, m_uiActiveUin);
        return iRet;
    }

    //计算所有可移动位置的防御价值
    iRet = ProcessDefenceValue(*pstBattlefieldObj, *pstActionUnitObj, *pstAIConfig);
    if(iRet)
    {
        LOGERROR("Failed to process defence value, ret %d, uin %u\n", iRet, m_uiActiveUin);
        return iRet;
    }

    //计算所有可移动位置的移动价值
    iRet = ProcessMoveValue(*pstBattlefieldObj, *pstActionUnitObj, *pstAIConfig);
    if(iRet)
    {
        LOGERROR("Failed to process move value, ret %d, uin %u\n", iRet, m_uiActiveUin);
        return iRet;
    }

    //获取目标移动点位
    if(m_iValidPosNum == 0)
    {
        LOGERROR("Failed to get AI unit move pos, invalid pos num zero, uin %u\n", m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iMaxValue = 100*(m_astValidPos[0].iAttackValue + m_astValidPos[0].iDefenceValue) + m_astValidPos[0].iMoveValue;
    int iCurrentValue = 0;
    for(int i=0; i<m_iValidPosNum; ++i)
    {
        iCurrentValue = 100*(m_astValidPos[i].iAttackValue + m_astValidPos[i].iDefenceValue) + m_astValidPos[i].iMoveValue;
        if(iMaxValue <= iCurrentValue)
        {
            iMaxValue = iCurrentValue;
            m_iMovePosIndex = i;
        }
    }

    return T_SERVER_SUCESS;
}

//获取AI移动的路径
const UnitPath& CCombatUnitAI::GetMoveTargetPath()
{
    static UnitPath stMovePath;
    stMovePath.Clear();

    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(m_iBattlefiledObjID);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, id %d, uin %u\n", m_iBattlefiledObjID, m_uiActiveUin);
        return stMovePath;
    }

    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnitID);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get combat unit obj, unit id %d, uin %u\n", m_iActionUnitID, m_uiActiveUin);
        return stMovePath;
    }

    int iMotorDistance = pstActionUnitObj->GetMotorDistance();
    if(iMotorDistance >= 0)
    {
        if(!pstBattlefieldObj->IsPosCanWalk(pstActionUnitObj->GetMotorTargetPos(),m_iActionUnitID))
        {
            //机动模式目标点不能行走,取消机动模式
            pstActionUnitObj->SetMotorMode(TUNITPOSITION(-1,0), -1);
            return stMovePath;
        }

        //使用机动模式
        m_iMovePosIndex = 0;
        m_astValidPos[m_iMovePosIndex].iAttackValue = 1;    //设置不为0，在行动中再处理机动模式
        m_astValidPos[m_iMovePosIndex].stPos.iPosX = pstActionUnitObj->GetMotorTargetPos().iPosX;
        m_astValidPos[m_iMovePosIndex].stPos.iPosY = pstActionUnitObj->GetMotorTargetPos().iPosY;
    }

    const TUNITPOSITION& stSrcPos = pstActionUnitObj->GetUnitPosition();
    const TUNITPOSITION& stDestPos = m_astValidPos[m_iMovePosIndex].stPos;

    if(stSrcPos.iPosX==stDestPos.iPosX && stSrcPos.iPosY==stDestPos.iPosY)
    {
        return stMovePath;
    }

    //做A*寻路，找出路径
    std::vector<TUNITPOSITION> vPath;
    if(!CAStarPathUtility::FindAStarPath(*pstBattlefieldObj, pstBattlefieldObj->GetBattlePathManager(), stSrcPos, 
                                         stDestPos, pstActionUnitObj->GetUnitSize(), vPath))
    {
        if(iMotorDistance >= 0)
        {
            //机动模式并且走不到目标点,取消机动模式
            pstActionUnitObj->SetMotorMode(TUNITPOSITION(-1,0), -1);
        }

        //不能行走
        return stMovePath;
    }

    unsigned uMaxMoveDistance = pstBattlefieldObj->GetMaxMoveDistance(*pstActionUnitObj);
    for(unsigned i=0; i<vPath.size()&&i<uMaxMoveDistance; ++i)
    {
        UnitPosition* pstPos = stMovePath.add_stposes();
        pstPos->set_iposx(vPath[i].iPosX);
        pstPos->set_iposy(vPath[i].iPosY);
    }

    return stMovePath;
}

//当前执行的AI是否可以攻击
bool CCombatUnitAI::CanDoAttack()
{
    return (m_astValidPos[m_iMovePosIndex].iAttackValue > 0);
}

//当前使用的技能
int CCombatUnitAI::GetUseSkill()
{
    return m_iUseSkillID;
}

//当前攻击的目标位置
const TUNITPOSITION& CCombatUnitAI::GetTargetUnitPos()
{
    return m_astValidPos[m_iMovePosIndex].stTargetPos;
}

//获取当前所有可移动点的集合
int CCombatUnitAI::GetAllValidMovePos(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj)
{
    GetNearByValidMovePos(stBattlefieldObj, stActionObj.GetUnitPosition(), stBattlefieldObj.GetMaxMoveDistance(stActionObj));

    return T_SERVER_SUCESS;
}

//获取当前点周围的可移动点
void CCombatUnitAI::GetNearByValidMovePos(CBattlefieldObj& stBattlefieldObj, TUNITPOSITION& stPos, int iMoveStep)
{
    //先处理当前可移动点信息
    bool bIsValidPos = ProcessCurrentMovePos(stBattlefieldObj, stPos);
    if(!bIsValidPos)
    {
        //该位置不是有效的点，不检查它周围的
        return;
    }

    if(iMoveStep <= 0)
    {
        //已经不能再行走了，直接返回
        return;
    }

    //处理周围的4个可移动点信息
    TUNITPOSITION astPos[4];

    //左边
    astPos[0].iPosX = stPos.iPosX - 1;
    astPos[0].iPosY = stPos.iPosY;
    
    //右边
    astPos[1].iPosX = stPos.iPosX + 1;
    astPos[1].iPosY = stPos.iPosY;

    //上边
    astPos[2].iPosX = stPos.iPosX;
    astPos[2].iPosY = stPos.iPosY + 1;

    //下边
    astPos[3].iPosX = stPos.iPosX;
    astPos[3].iPosY = stPos.iPosY - 1;

    for(int i=0; i<4; ++i)
    {
        GetNearByValidMovePos(stBattlefieldObj, astPos[i], iMoveStep-1);
    }
     
    return; 
}

//判断当前点是否有效的可移动点
bool CCombatUnitAI::ProcessCurrentMovePos(CBattlefieldObj& stBattlefieldObj, TUNITPOSITION& stPos)
{
    CCombatUnitObj* pstCombatUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnitID);
    if(!pstCombatUnitObj)
    {
        return false;
    }

    //获取所有需要检查的点
    std::vector<TUNITPOSITION> vCheckPos;
    for(int x=0; x<=pstCombatUnitObj->GetUnitSize(); ++x)
    {
        for(int y=0; y<=pstCombatUnitObj->GetUnitSize(); ++y)
        {
            vCheckPos.push_back(TUNITPOSITION(stPos.iPosX+x, stPos.iPosY-y));
        }
    }

    //检查点的阻挡
    for(unsigned i=0; i<vCheckPos.size(); ++i)
    {
        if(!stBattlefieldObj.IsPosCanWalk(vCheckPos[i], pstCombatUnitObj->GetCombatUnitID()))
        {
            return false;
        }
    }

    //该位置是否已加入可移动列表
    for(int i=0; i<m_iValidPosNum; ++i)
    {
        if(m_astValidPos[i].stPos.iPosX==stPos.iPosX
           && m_astValidPos[i].stPos.iPosY==stPos.iPosY)
        {
            //已加入到可移动列表
            return true;
        }
    }

    //该位置加入可移动列表
    FightAIPosValue stPosValue;
    stPosValue.stPos.iPosX = stPos.iPosX;
    stPosValue.stPos.iPosY = stPos.iPosY;

    m_astValidPos[m_iValidPosNum++] = stPosValue;

    return true;
}

//计算所有可移动位置点的攻击价值
int CCombatUnitAI::ProcessAttackValue(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig)
{
    //从AI表中随机使用的技能
    GetAIUseSkill(stBattlefieldObj, stActionObj, stAIConfig);

    int iRet = T_SERVER_SUCESS;
    int iMaxAttackValue = 0;

    //计算所有点的攻击价值
    for(int i=0; i<m_iValidPosNum; ++i)
    {
        iRet = ProcessOnePosAttackValue(stBattlefieldObj, m_astValidPos[i], iMaxAttackValue);
        if(iRet)
        {
            LOGERROR("Failed to process one pos attack value, ret %d, uin %u\n", iRet, m_uiActiveUin);
            return iRet;
        }
    }
    
    //如果所有点最大攻击价值为0并且非普攻，则换成普攻重新计算一次
    if(iMaxAttackValue==0 && m_iUseSkillID!=0 && stActionObj.GetNormalSkillID()!=m_iUseSkillID)
    {
        m_iUseSkillID = stActionObj.GetNormalSkillID();

        //使用普攻重新计算所有点的行动价值
        for(int i=0; i<m_iValidPosNum; ++i)
        {
            iRet = ProcessOnePosAttackValue(stBattlefieldObj, m_astValidPos[i], iMaxAttackValue);
            if(iRet)
            {
                LOGERROR("Failed to process one pos attack value, ret %d, uin %u\n", iRet, m_uiActiveUin);
                return iRet;
            }
        }
    }

    //如果所有点最大攻击价值为0,返回所有点
    if(iMaxAttackValue == 0)
    {
        return T_SERVER_SUCESS;
    }

    //剔除攻击价值为0的点
    for(int i=0; i<m_iValidPosNum;)
    {
        if(m_astValidPos[i].iAttackValue == 0)
        {
            //将最后一个交换到该位置
            m_astValidPos[i] = m_astValidPos[m_iValidPosNum-1];
            --m_iValidPosNum;

            continue;
        }

        ++i;
    }

    return T_SERVER_SUCESS;
}

//计算所有可移动位置点的防御价值
int CCombatUnitAI::ProcessDefenceValue(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig)
{
    int iRet = T_SERVER_SUCESS;

    //遍历计算每个位置
    for(int i=0; i<m_iValidPosNum; ++i)
    {
        iRet = ProcessOnePosDefenceValue(stBattlefieldObj, stActionObj, stAIConfig, m_astValidPos[i]);
        if(iRet)
        {
            LOGERROR("Failed to process one pos defence value, ret %d, uin %u\n", iRet, m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//计算所有可移动位置点的移动价值
int CCombatUnitAI::ProcessMoveValue(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig)
{
    int iRet = T_SERVER_SUCESS;

    //遍历所有的位置
    for(int i=0; i<m_iValidPosNum; ++i)
    {
        iRet = ProcessOnePosMoveValue(stBattlefieldObj, stActionObj, stAIConfig, m_astValidPos[i]);
        if(iRet)
        {
            LOGERROR("Failed to process one pos move value, ret %d, uin %u\n", iRet, m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//计算某个可移动位置点的攻击价值
int CCombatUnitAI::ProcessOnePosAttackValue(CBattlefieldObj& stBattlefieldObj, FightAIPosValue& stPosValue, int& iMaxValue)
{
    if(m_iUseSkillID == 0)
    {
        stPosValue.iAttackValue = 0;
        return T_SERVER_SUCESS;
    }

    //获取技能的配置
    const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(m_iUseSkillID);
    if(!pstSkillConfig)
    {
        LOGERROR("Failed to get fight skill config, invalid skill id %d\n", m_iUseSkillID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //获取技能的命中分布配置
    const SSkillRangeConfig* pstTargetRangeConfig = SkillRangeCfgMgr().GetConfig(pstSkillConfig->iTargetRangeID);
    if(!pstTargetRangeConfig)
    {
        LOGERROR("Failed to get skill range config, invalid range id %d\n", pstSkillConfig->iTargetRangeID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //遍历所有可攻击的位置
    TUNITPOSITION stTmpPos;
    int iAttackValue = 0;
    for(int i=0; i<pstTargetRangeConfig->iRangeNum; ++i)
    {
        if(pstTargetRangeConfig->aiRangeInfo[i] == 0)
        {
            continue;
        }

        //读取Area表
        const SSkillAreaConfig* pstAreaConfig = SkillAreaCfgMgr().GetConfig(i+1+pstTargetRangeConfig->iRangeAreaParam);
        if(!pstAreaConfig)
        {
            LOGERROR("Failed to get skill area config, invalid area id %d\n", i+1+pstTargetRangeConfig->iRangeAreaParam);
            return T_ZONE_SYSTEM_INVALID_CFG;
        }

        //遍历Area中的所有位置
        for(int j=0; j<pstAreaConfig->iAreaPosNum; ++j)
        {
            stTmpPos.iPosX = stPosValue.stPos.iPosX + pstAreaConfig->astTargetPosInfo[j].iPosX;
            stTmpPos.iPosY = stPosValue.stPos.iPosY + pstAreaConfig->astTargetPosInfo[j].iPosY;

            //todo jasonxiong4 脚本重做
            //iAttackValue = CModuleHelper::GetStoryFramework()->DoCombatAIScript(m_uiActiveUin, stBattlefieldObj.GetCrossID(), m_iUseSkillID, m_iActionUnitID, stPosValue.stPos,
            //                                                                    stTmpPos, pstTargetRangeConfig->aiRangeInfo[i], pstDamageRangeConfig->aiRangeInfo[i]);
            if(stPosValue.iAttackValue < iAttackValue)
            {
                stPosValue.iAttackValue = iAttackValue;
                stPosValue.stTargetPos.iPosX = stTmpPos.iPosX;
                stPosValue.stTargetPos.iPosY = stTmpPos.iPosY;
            }
        }
    }

    if(stPosValue.iAttackValue > iMaxValue)
    {
        iMaxValue = stPosValue.iAttackValue;
    }

    return T_SERVER_SUCESS;
}

//计算某个可移动位置点的防御价值
int CCombatUnitAI::ProcessOnePosDefenceValue(CBattlefieldObj& stBattlefiledObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig, FightAIPosValue& stPosValue)
{
    //获取所有的敌人
    std::vector<int> vEnemyUnits;
    stBattlefiledObj.GetEnemyUnits(stActionObj.GetCombatUnitID(), vEnemyUnits);

    static int iValidEnemyNum = 0;
    static int aiDefenceValue[MAX_CAMP_FIGHT_UNIT_NUM];

    iValidEnemyNum = 0;

    for(unsigned i=0; i<vEnemyUnits.size(); ++i)
    {
        //获取敌人数据
        CCombatUnitObj* pstEnemyObj = CCombatUtility::GetCombatUnitObj(vEnemyUnits[i]);
        if(!pstEnemyObj)
        {
            continue;
        }

        //获取普攻
        const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(pstEnemyObj->GetNormalSkillID());
        if(!pstSkillConfig)
        {
            LOGERROR("Failed to get normal skill config, invalid skill id %d\n", pstEnemyObj->GetNormalSkillID());
            return T_ZONE_SYSTEM_INVALID_CFG;
        }

        //判断敌人是否能打到我
        std::vector<TUNITPOSITION> vCheckPos;
        for(int x=0; x<=stActionObj.GetUnitSize(); ++x)
        {
            for(int y=0; y<=stActionObj.GetUnitSize(); ++y)
            {
                vCheckPos.push_back(TUNITPOSITION(stPosValue.stPos.iPosX+x, stPosValue.stPos.iPosY-y));
            }
        }
    
        int iDistance = 0;
        for(unsigned j=0; j<vCheckPos.size(); ++j)
        {
            iDistance = CCombatUtility::GetAttackDistance(pstEnemyObj->GetUnitPosition(), vCheckPos[j], pstSkillConfig->iTargetRangeID);
            if(iDistance != 0)
            {
                break;
            }
        }

        if(iDistance == 0)
        {
            //该位置打不到
            continue;
        }

        //可以打到，计算伤害评估
        //基础效果
        int iBaseEffect = pstSkillConfig->iDamageBaseNum;
        if(!pstEnemyObj->HasBuffOfSameType(pstSkillConfig->iTargetBuff))
        {
            iBaseEffect += pstSkillConfig->iSkillBuffValue;
        }

        //计算命中因子

        //招式命中
        int iSkillHit = CCombatUtility::GetSkillHit(*pstEnemyObj, *pstSkillConfig, iDistance);
        //招式闪避
        int iSkillDodge = CCombatUtility::GetSkillDodge(*pstEnemyObj, stActionObj, *pstSkillConfig);
        if(iSkillHit+iSkillDodge < 0)
        {
            LOGERROR("Failed to process skill defence value, invalid skillhit and skilldodge, uin %u!\n", m_uiActiveUin);

            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //计算攻防因子
        //攻击强度
        int iAttackStrength = CCombatUtility::GetAttackStrength(*pstEnemyObj, *pstSkillConfig);
        //防御强度
        int iDefenceStrength = CCombatUtility::GetDefenceStrength(*pstEnemyObj, stActionObj, *pstSkillConfig);
        if(iAttackStrength+iDefenceStrength <= 0)
        {
            LOGERROR("Failed to process skill defence value, invalid attack strength and defence strength, uin %u\n", m_uiActiveUin);
            
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //计算基本效果
        int iAttackValue = 0;
        if(iSkillHit+iSkillDodge > 0)
        {
            iAttackValue = iBaseEffect*iSkillHit*iAttackStrength/((iSkillHit+iSkillDodge)*(iAttackStrength+iDefenceStrength));
        }

        //加上AI的伤害评估
        iAttackValue = iAttackValue*stAIConfig.aiAIAttribute[COMBAT_UNIT_AI_BEIJI]/100;
        if(iAttackValue > 0)
        {
            aiDefenceValue[iValidEnemyNum++] = iAttackValue;
        }
    }

    if(iValidEnemyNum == 0)
    {
        stPosValue.iDefenceValue = 0;

        return T_SERVER_SUCESS;
    }

    //计算平均值
    int iTotalValue = 0;
    for(int i=0; i<iValidEnemyNum; ++i)
    {
        iTotalValue += aiDefenceValue[i];
    }

    stPosValue.iDefenceValue = iTotalValue / iValidEnemyNum;

    return T_SERVER_SUCESS;
}

//计算某个可移动位置点的移动价值
int CCombatUnitAI::ProcessOnePosMoveValue(CBattlefieldObj& stBattlefiledObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig, FightAIPosValue& stPosValue)
{
    //获取所有的敌人
    std::vector<int> vEnemyUnits;
    stBattlefiledObj.GetEnemyUnits(stActionObj.GetCombatUnitID(), vEnemyUnits);

    //获取当前点与敌人的平均距离
    int iCurrentDistance = GetEnemyDisance(stActionObj.GetUnitPosition(), vEnemyUnits);

    //获取移动目标点与敌人的平均距离
    int iMoveDistance = GetEnemyDisance(stPosValue.stPos, vEnemyUnits);

    //计算移动价值
    stPosValue.iMoveValue = (iCurrentDistance - iMoveDistance)*stAIConfig.aiAIAttribute[COMBAT_UNIT_AI_ZHUIJI];

    //如果该位置有机关并且可见，扣除机关的价值负分
    CCombatTrapObj* pstTrapObj = stBattlefiledObj.GetTrapByPos(stPosValue.stPos);
    if(pstTrapObj && stBattlefiledObj.IsTrapVisible(stActionObj,*pstTrapObj))
    {
        //该位置有可见机关
        stPosValue.iMoveValue -= pstTrapObj->GetConfig()->iTrapValue;
    }

    return T_SERVER_SUCESS;
}

//获取当前使用的技能
void CCombatUnitAI::GetAIUseSkill(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig)
{
    std::vector<int> vValidSkills;
    stActionObj.GetValidFightSkills(vValidSkills);

    int iRandomRange = 0;
    for(unsigned i=0; i<vValidSkills.size(); ++i)
    {
        iRandomRange += stAIConfig.aiAIAttribute[COMBAT_UNIT_AI_SKILL1+i];
    }

    if(iRandomRange == 0)
    {
        return;
    }

    int iRandNum = CRandomCalculator::GetRandomNumberInRange(iRandomRange);
    for(unsigned i=0; i<vValidSkills.size(); ++i)
    {
        if(iRandNum<=stAIConfig.aiAIAttribute[COMBAT_UNIT_AI_SKILL1+i])
        {
            m_iUseSkillID = vValidSkills[i];
            break;
        }

        iRandNum -= stAIConfig.aiAIAttribute[COMBAT_UNIT_AI_SKILL1+i];
    }

    //判断技能是否能够使用
    const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(m_iUseSkillID);
    if(!pstSkillConfig)
    {
        LOGERROR("Failed to get fight skill, skill id %d\n", m_iUseSkillID);
        return;
    }

    //判断是否CD中
    if(stActionObj.GetSkillCDRound(m_iUseSkillID) != 0)
    {
        //不能使用，换成普攻
        m_iUseSkillID = vValidSkills[0];
        return;
    }

    return;
}

//计算技能的攻击价值
int CCombatUnitAI::ProcessSkillAttackValue(CCombatUnitObj& stActionObj, CCombatUnitObj& stTargetObj, const SFightUnitAIConfig& stAIConfig, 
                                           const SFightUnitSkillConfig& stSkillConfig, bool bIsToEnemy, const TUNITPOSITION& stPos)
{
    int iAttackValue = 0;

    if(bIsToEnemy)
    {
        //计算对敌伤害
        
        //基础效果
        int iBaseEffect = stSkillConfig.iDamageBaseNum;
        if(!stTargetObj.HasBuffOfSameType(stSkillConfig.iTargetBuff))
        {
            iBaseEffect += stSkillConfig.iSkillBuffValue;
        }

        //计算距离
        std::vector<TUNITPOSITION> vCheckPos;
        for(int x=0; x<=stTargetObj.GetUnitSize(); ++x)
        {
            for(int y=0; y<=stTargetObj.GetUnitSize(); ++y)
            {
                vCheckPos.push_back(TUNITPOSITION(stTargetObj.GetUnitPosition().iPosX+x, stTargetObj.GetUnitPosition().iPosY-y));
            }
        }
    
        int iDistance = 0;
        for(unsigned i=0; i<vCheckPos.size(); ++i)
        {
            iDistance = CCombatUtility::GetAttackDistance(stPos, vCheckPos[i], stSkillConfig.iTargetRangeID);
            if(iDistance != 0)
            {
                break;
            }
        }

        //加上距离对伤害效果的影响
        const SSkillRangeConfig* pstRangeConfig = SkillRangeCfgMgr().GetConfig(stSkillConfig.iDamageRangeID);
        if(!pstRangeConfig)
        {
            LOGERROR("Failed to get skill range config, skill id %d, range id %d, uin %u\n", stSkillConfig.iConfigID, stSkillConfig.iDamageRangeID, m_uiActiveUin);
            return T_ZONE_SYSTEM_INVALID_CFG;
        }

        int iRangeIndex = iDistance;
        if(iRangeIndex >= MAX_SKILL_RANGE_INFO_NUM)
        {
            //距离MAX_SKILL_RANGE_INFO_NUM特殊含义，这边最大到MAX_SKILL_RANGE_INFO_NUM-1
            iRangeIndex = MAX_SKILL_RANGE_INFO_NUM-1;
        }
    
        //距离转换成下标需要减1
        iRangeIndex = iRangeIndex - 1;

        iBaseEffect = iBaseEffect*pstRangeConfig->aiRangeInfo[iRangeIndex]/10000;

        //计算命中因子

        //招式命中
        int iSkillHit = CCombatUtility::GetSkillHit(stActionObj,stSkillConfig, iDistance);
        //招式闪避
        int iSkillDodge = CCombatUtility::GetSkillDodge(stActionObj, stTargetObj, stSkillConfig);
        if(iSkillHit+iSkillDodge <= 0)
        {
            LOGERROR("Failed to process skill attack value, invalid skillhit and skilldodge, uin %u!\n", m_uiActiveUin);

            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //计算攻防因子
        //攻击强度
        int iAttackStrength = CCombatUtility::GetAttackStrength(stActionObj, stSkillConfig);
        //防御强度
        int iDefenceStrength = CCombatUtility::GetDefenceStrength(stActionObj, stTargetObj, stSkillConfig);
        if(iAttackStrength+iDefenceStrength <= 0)
        {
            LOGERROR("Failed to process skill attack value, invalid attack strength and defence strength, uin %u\n", m_uiActiveUin);
            
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //计算基本效果
        iAttackValue = iBaseEffect*iSkillHit*iAttackStrength/((iSkillHit+iSkillDodge)*(iAttackStrength+iDefenceStrength));
        iAttackValue = (iAttackValue<=0) ? 1 : (iAttackValue+1);

        //目标是否可以被击杀
        if(stTargetObj.GetFightAttr(FIGHT_ATTR_HP) <= iAttackValue)
        {
            //目标能够被击杀
            iAttackValue = iAttackValue*stAIConfig.aiAIAttribute[COMBAT_UNIT_AI_JISHA]/100;
        }

        //是否自己的最后攻击目标
        if(stActionObj.GetLastAttackUnitID() == stTargetObj.GetCombatUnitID())
        {
            //是自己的最后攻击目标
            iAttackValue = iAttackValue * stAIConfig.aiAIAttribute[COMBAT_UNIT_AI_JIAOSHOU]/100;
        }
        
        CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(m_iBattlefiledObjID);
        if(!pstBattlefieldObj)
        {
            LOGERROR("Failed to get battlefield obj, invalid id %d, uin %u\n", m_iBattlefiledObjID, m_uiActiveUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //是否伙伴最后攻击目标
        if(pstBattlefieldObj->IsTeammateLastAttackUnit(stActionObj.GetCombatUnitID(), stTargetObj.GetCombatUnitID()))
        {
            //是伙伴的最后攻击目标
            iAttackValue = iAttackValue*stAIConfig.aiAIAttribute[COMBAT_UNIT_AI_HEJI]/100;
        }
    }
    else
    {
        //计算友方治疗
        int iTargetMaxHP = stTargetObj.GetFightAttr(FIGHT_ATTR_HPMAX);
        int iTargetHP = stTargetObj.GetFightAttr(FIGHT_ATTR_HP);

        iAttackValue = (-stSkillConfig.iDamageBaseNum)*(iTargetMaxHP-iTargetHP)/iTargetMaxHP;

        if(!stTargetObj.HasBuffOfSameType(stSkillConfig.iTargetBuff))
        {
            iAttackValue += stSkillConfig.iSkillBuffValue;
        }
    }

    return iAttackValue;
}

//获取位置到所有敌人位置的平均距离
int CCombatUnitAI::GetEnemyDisance(const TUNITPOSITION& stPos, std::vector<int>& vEnemyUnits)
{
    if(vEnemyUnits.size() == 0)
    {
        return 0;
    }

    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(m_iBattlefiledObjID);
    if(!pstBattlefieldObj)
    {
        return 0;
    }

    int iTotalDistance = 0;
    for(unsigned i=0; i<vEnemyUnits.size(); ++i)
    {
        CCombatUnitObj* pstEnemyObj = CCombatUtility::GetCombatUnitObj(vEnemyUnits[i]);
        if(!pstEnemyObj)
        {
            LOGERROR("Failed to get enemy obj, invalid id %d, uin %u\n", vEnemyUnits[i], m_uiActiveUin);
            return 0;
        }

        TUNITPOSITION& stEnemyPos = pstEnemyObj->GetUnitPosition();

        //iTotalDistance += ABS(stPos.iPosX,stEnemyPos.iPosX)+ABS(stPos.iPosY,stEnemyPos.iPosY); 
        iTotalDistance += GetWalkDistance(*pstBattlefieldObj, stPos, stEnemyPos);
    }

    return iTotalDistance/(int)vEnemyUnits.size();
}

//获取两点之间可行走的距离
int CCombatUnitAI::GetWalkDistance(CBattlefieldObj& stBattlefieldObj, const TUNITPOSITION& stCurPos, const TUNITPOSITION& stTargetPos)
{
    //获取原点的战斗单位大小
    int iSize = 0;

    CCombatUnitObj* pstUnitObj = stBattlefieldObj.GetCombatUnitByPos(stCurPos);
    if(pstUnitObj)
    {
        iSize = pstUnitObj->GetUnitSize();
    }

    //做A*寻路，找出路径
    std::vector<TUNITPOSITION> vPath;
    if(!CAStarPathUtility::FindAStarPath(stBattlefieldObj, stBattlefieldObj.GetBattlePathManager(), stCurPos, stTargetPos, iSize, vPath))
    {
        //不能行走
        return 0;
    }

    //减去包含的当前点
    return vPath.size();
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
