

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "ModuleShortcut.hpp"
#include "GameRole.hpp"
#include "ZoneObjectHelper.hpp"
#include "Random.hpp"
#include "CombatUnitObj.hpp"
#include "ZoneMsgHelper.hpp"
#include "UnitUtility.hpp"
#include "CombatUtility.hpp"
#include "CombatSkill.hpp"
#include "ZoneOssLog.hpp"
#include "DropRewardUtility.hpp"
#include "FightUnitUtility.hpp"

#include "BattlefieldObj.hpp"

using namespace ServerLib;
using namespace GameConfig;

GameProtocolMsg CBattlefieldObj::m_stRemoveBuff_Notify;

IMPLEMENT_DYN(CBattlefieldObj)

CBattlefieldObj::CBattlefieldObj()
{
    Initialize();
}

CBattlefieldObj::~CBattlefieldObj()
{
    return;
}

int CBattlefieldObj::Initialize()
{
    m_uiActiveUin = 0;
    m_uiPassiveUin = 0;

    //阵营信息
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        m_aiActiveCombatObjIndex[i] = -1;
        m_aiPassiveCombatObjIndex[i] = -1;
        m_astActivePos[i].iPosX = -1;
        m_astPassivePos[i].iPosX = -1;
    }

    //当前回合尚未出手的战斗单位信息
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM*2; ++i)
    {
        m_aiCanCombatUnit[i] = -1;
        m_aiActionDoneUnit[i] = -1;
    }

    //脚本杀死主动方单位信息
    m_iActiveScriptKilledNum = 0;
    memset(m_aiActiveScriptKilledConfigID, 0, sizeof(m_aiActiveScriptKilledConfigID));

    m_bActiveNeedSetForm = false;
    m_bPassiveNeedSetForm = false;

    m_iCanCombatUnitNum = 0;
    m_iActionDoneUnitNum = 0;
    m_iActionUnit = -1;
    m_iRoleCombatUnitStatus = ROLE_COMBAT_UNIT_STAT_INVALID;

    //战场机关
    m_iTrapNum = 0;
    for(int i=0; i<MAX_COMBAT_TRAP_NUM; ++i)
    {
        m_aiCombatTrapIndex[i] = -1;
    }

    //战场Obj的ID
    m_iBattlefieldObjID = -1;

	//关卡ID
	m_iCrossID = 0;

    //地图ID
    m_iMapID = 0;

    //地图对应的阻挡配置
    m_pstBattlePathManager = NULL;

    m_iCombatResult = COMBAT_RESULT_INVALID;

    m_iCrossID = 0;

    m_iCombatRoundNum = 0;

    //战斗战场的状态
    m_ucCombatStatus = 0;

    return T_SERVER_SUCESS;
}

//主动方的uin
void CBattlefieldObj::SetActiveUin(unsigned int uiUin)
{
    m_uiActiveUin = uiUin;
}

unsigned int CBattlefieldObj::GetActiveUin()
{
    return m_uiActiveUin;
}
    
//被动方的uin
void CBattlefieldObj::SetPassiveUin(unsigned int uiUin)
{
    m_uiPassiveUin = uiUin;
}

unsigned int CBattlefieldObj::GetPassiveUin()
{
    return m_uiPassiveUin;
}

//战场Obj的唯一ID
void CBattlefieldObj::SetBattlefieldObjID(int iObjID)
{
    m_iBattlefieldObjID = iObjID;
}

int CBattlefieldObj::GetBattlefieldObjID()
{
    return m_iBattlefieldObjID;
}

//初始化PVE战场
int CBattlefieldObj::InitPveBattlefield(CGameRoleObj& rstRoleObj, int iCrossID)
{
    //获取UIN
    m_uiActiveUin = rstRoleObj.GetUin();

    m_iCombatResult = COMBAT_RESULT_INVALID;

    //初始化PVE战场信息
    int iRet = InitBattlefieldInfo(iCrossID);
    if(iRet)
    {
        LOGERROR("Failed to init battlefield info, cross id %d\n", iCrossID);
        return iRet;
    }

    //初始化PVE怪物信息
    iRet = InitMonsterInfo(iCrossID);
    if(iRet)
    {
        LOGERROR("Failed to init pve monster info, cross id %d\n", iCrossID);
        return iRet;
    }

    //初始化主动方的战斗单位信息
    iRet = InitFightCampInfo(rstRoleObj);
    if(iRet)
    {
        LOGERROR("Failed to init active camp fight info, ret %d, uin %u\n", iRet, m_uiActiveUin);
        return iRet;
    }

    //设置战斗开始时间
    m_iStartTime = CTimeUtility::GetNowTime();

    m_ucCombatStatus = 0;

    return T_SERVER_SUCESS;
}

//初始化PVP战场
int CBattlefieldObj::InitPVPBattlefield(CGameRoleObj& stActiveRoleObj, CGameRoleObj& stPassiveRoleObj)
{
    //获取UIN
    m_uiActiveUin = stActiveRoleObj.GetUin();
    m_uiPassiveUin = stPassiveRoleObj.GetUin();

    m_iCombatResult = COMBAT_RESULT_INVALID;

    //初始化战场信息
    int iRet = InitBattlefieldInfo(PVP_COMBAT_CROSS_ID);
    if(iRet)
    {
        LOGERROR("Failed to init battlefield info, cross id %d\n", 100);
        return iRet;
    }

    //初始化主动方的战斗单位信息
    iRet = InitFightCampInfo(stActiveRoleObj);
    if(iRet)
    {
        LOGERROR("Failed to init active camp fight info, ret %d, uin %u\n", iRet, m_uiActiveUin);
        return iRet;
    }

    //初始化被动方的战斗单位信息
    iRet = InitFightCampInfo(stPassiveRoleObj, false);
    if(iRet)
    {
        LOGERROR("Failed to init passivce camp fight info, ret %d, uin %u\n", iRet, m_uiPassiveUin);
        return iRet;
    }

    //设置战斗开始时间
    m_iStartTime = CTimeUtility::GetNowTime();

    m_ucCombatStatus = 0;

    return T_SERVER_SUCESS;
}

//打包返回战场的信息
int CBattlefieldObj::PackBattlefiledInfo(Zone_Battlefield_Notify& stNotify)
{
    //返回战场ID
    stNotify.set_icrossid(m_iCrossID);

    //返回主动方战斗阵营信息
    CombatCamp* pstSelfCamp = stNotify.mutable_stcampactive();
    CombatCamp* pstOtherCamp = stNotify.mutable_stcamppassive();

    pstSelfCamp->set_uiuin(m_uiActiveUin);
    pstOtherCamp->set_uiuin(m_uiPassiveUin);

    //如果是玩家，设置玩家的名字和年龄
    CGameRoleObj* pstActiveRoleObj = CUnitUtility::GetRoleByUin(m_uiActiveUin);
    if(pstActiveRoleObj)
    {
        pstSelfCamp->set_strnickname(pstActiveRoleObj->GetNickName());
    }

    CGameRoleObj* pstPassiveRoleObj = CUnitUtility::GetRoleByUin(m_uiPassiveUin);
    if(pstPassiveRoleObj)
    {
        pstOtherCamp->set_strnickname(pstPassiveRoleObj->GetNickName());
    }

    int iRet = T_SERVER_SUCESS;
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_aiActiveCombatObjIndex[i] >= 0)
        {
            iRet = PackOneCombatUnitInfo(FIGHT_CAMP_ACTIVE, m_aiActiveCombatObjIndex[i], *pstSelfCamp->add_stunits());
            if(iRet)
            {
                LOGERROR("Failed to pack one combat unit info, obj index %d, uin %d\n", m_aiActiveCombatObjIndex[i], m_uiActiveUin);
                return iRet;
            }
        }

        if(m_aiPassiveCombatObjIndex[i] >= 0)
        {
            iRet = PackOneCombatUnitInfo(FIGHT_CAMP_PASSIVE, m_aiPassiveCombatObjIndex[i], *pstOtherCamp->add_stunits());
            if(iRet)
            {
                LOGERROR("Failed to pack one combat unit info, obj index %d, uin %u\n", m_aiPassiveCombatObjIndex[i], m_uiPassiveUin);
                return iRet;
            }
        }
    }

    //返回主动方有效位置信息
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_astActivePos[i].iPosX < 0)
        {
            break;
        }

        UnitPosition* pstActivePos = stNotify.add_stactiveposes();
        pstActivePos->set_iposx(m_astActivePos[i].iPosX);
        pstActivePos->set_iposy(m_astActivePos[i].iPosY);
    }

    //返回被动方有效位置信息
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_astPassivePos[i].iPosX < 0)
        {
            break;
        }

        UnitPosition* pstPassivePos = stNotify.add_stpassiveposes();
        pstPassivePos->set_iposx(m_astPassivePos[i].iPosX);
        pstPassivePos->set_iposy(m_astPassivePos[i].iPosY);
    }

    return T_SERVER_SUCESS;
}

//清理战场
void CBattlefieldObj::ClearBattlefield()
{
    //清理战斗单位
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_aiActiveCombatObjIndex[i] >= 0)
        {
            ClearOneCombatUnitObj(m_aiActiveCombatObjIndex[i]);
        }

        m_aiActiveCombatObjIndex[i] = -1;

        if(m_aiPassiveCombatObjIndex[i] >= 0)
        {
            ClearOneCombatUnitObj(m_aiPassiveCombatObjIndex[i]);
        }

        m_aiPassiveCombatObjIndex[i] = -1;
    }

    //清理机关单位
    for(int i=0; i<m_iTrapNum; ++i)
    {
        GameType<CCombatTrapObj>::Del(m_aiCombatTrapIndex[i]);
    }

    return;
}

//获取地图ID
int CBattlefieldObj::GetMapID()
{
    return m_iMapID;
}

//根据坐标位置获取战斗单位信息
CCombatUnitObj* CBattlefieldObj::GetCombatUnitByPos(const TUNITPOSITION& stPos)
{
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_aiActiveCombatObjIndex[i] != -1)
        {
            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiActiveCombatObjIndex[i]);
            if(pstUnitObj)
            {
                TUNITPOSITION& stUnitPos = pstUnitObj->GetUnitPosition();
                for(int x=0; x<=pstUnitObj->GetUnitSize(); ++x)
                {
                    for(int y=0; y<=pstUnitObj->GetUnitSize(); ++y)
                    {
                        if(stUnitPos.iPosX+x==stPos.iPosX && stUnitPos.iPosY-y==stPos.iPosY)
                        {
                            return pstUnitObj;
                        }
                    }
                }
            }
        }

        if(m_aiPassiveCombatObjIndex[i] != -1)
        {
            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiPassiveCombatObjIndex[i]);
            if(pstUnitObj)
            {
                TUNITPOSITION& stUnitPos = pstUnitObj->GetUnitPosition();
                for(int x=0; x<=pstUnitObj->GetUnitSize(); ++x)
                {
                    for(int y=0; y<=pstUnitObj->GetUnitSize(); ++y)
                    {
                        if(stUnitPos.iPosX+x==stPos.iPosX && stUnitPos.iPosY-y==stPos.iPosY)
                        {
                            return pstUnitObj;
                        }
                    }
                }         
            }
        }
    }

    return NULL;
}

//或否是伙伴的最后攻击对象
bool CBattlefieldObj::IsTeammateLastAttackUnit(int iCastUnitID, int iTargetUnitID)
{
    int iCastCamp = GetCombatUnitCamp(iCastUnitID);

    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(iCastCamp == FIGHT_CAMP_ACTIVE)
        {
            if(m_aiActiveCombatObjIndex[i] < 0 || m_aiActiveCombatObjIndex[i] == iCastUnitID)
            {
                continue;
            }

            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiActiveCombatObjIndex[i]);
            if(!pstUnitObj)
            {
                continue;
            }

            if(pstUnitObj->GetLastAttackUnitID() == iTargetUnitID)
            {
                return true;
            }

        }
        else
        {
            if(m_aiPassiveCombatObjIndex[i] < 0 || m_aiPassiveCombatObjIndex[i] == iCastUnitID)
            {
                continue;
            }

            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiPassiveCombatObjIndex[i]);
            if(!pstUnitObj)
            {
                continue;
            }

            if(pstUnitObj->GetLastAttackUnitID() == iTargetUnitID)
            {
                return true;
            }
        }
    }

    return false;
}

//获取敌人信息
void CBattlefieldObj::GetEnemyUnits(int iCastUnitID, std::vector<int>& vEnemyUnits)
{
    int iCampType = GetCombatUnitCamp(iCastUnitID);
    
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(iCampType==FIGHT_CAMP_ACTIVE && m_aiPassiveCombatObjIndex[i]>=0)
        {
            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiPassiveCombatObjIndex[i]);
            if(!pstUnitObj || pstUnitObj->IsCombatUnitDead())
            {
                continue;
            }

            vEnemyUnits.push_back(m_aiPassiveCombatObjIndex[i]);
        }
        else if(iCampType==FIGHT_CAMP_PASSIVE && m_aiActiveCombatObjIndex[i]>=0)
        {
            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiActiveCombatObjIndex[i]);
            if(!pstUnitObj || pstUnitObj->IsCombatUnitDead())
            {
                continue;
            }

            vEnemyUnits.push_back(m_aiActiveCombatObjIndex[i]);
        }
    }

    return;
}

//处理战场相关的BUFF
int CBattlefieldObj::DoBuffEffectByType(int iTriggerType, int iTargetUnitID, int iTriggerUnitID, int* pDamageNum)
{
    int iRet = T_SERVER_SUCESS;

    static GameProtocolMsg stDoBuffEffect_Notify;   //生效BUFF

    CZoneMsgHelper::GenerateMsgHead(stDoBuffEffect_Notify, MSGID_ZONE_DOBUFFEFFECT_NOTIFY);
    Zone_DoBuffEffect_Notify* pstNotify = stDoBuffEffect_Notify.mutable_m_stmsgbody()->mutable_m_stzone_dobuffeffect_notify();

    switch(iTriggerType)
    {
        case BUFF_TRIGGER_ROUNDBEGIN:
        case BUFF_TRIGGER_ROUNDEND:
            {
                //回合开始前和回合结束后，战场上所有单位都会触发

                //先处理自己的
                for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
                {
                    if(m_aiActiveCombatObjIndex[i] < 0)
                    {
                        continue;
                    }
            
                    CCombatUnitObj* pstCombatObj = CCombatUtility::GetCombatUnitObj(m_aiActiveCombatObjIndex[i]);
                    if(!pstCombatObj)
                    {
                        continue;
                    }
            
                    //处理BUFF
                    iRet = pstCombatObj->DoBuffEffectByType(m_uiActiveUin, m_iCrossID, iTriggerType, m_aiActiveCombatObjIndex[i], *pstNotify);
                    if(iRet)
                    {
                        return iRet;
                    }
                }
            
                //再处理对方的
                for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
                {
                    if(m_aiPassiveCombatObjIndex[i] < 0)
                    {
                        continue;
                    }
            
                    CCombatUnitObj* pstCombatObj = CCombatUtility::GetCombatUnitObj(m_aiPassiveCombatObjIndex[i]);
                    if(!pstCombatObj)
                    {
                        continue;
                    }
            
                    //处理BUFF
                    iRet = pstCombatObj->DoBuffEffectByType(m_uiActiveUin, m_iCrossID, iTriggerType, m_aiPassiveCombatObjIndex[i], *pstNotify);
                    if(iRet)
                    {
                        return iRet;
                    }
                }
            }
            break;

        case BUFF_TRIGGER_ACTIONROUNDBEGIN:
        case BUFF_TRIGGER_ACTIONROUNDEND:
        case BUFF_TRIGGER_UNDERDAMAGE:
        case BUFF_TRIGGER_DODGE:
        case BUFF_TRIGGER_UNDERATTACK:
        case BUFF_TRIGGER_MOVE:
            {
                //获取目标战斗单位Obj
                CCombatUnitObj* pstTargetObj = CCombatUtility::GetCombatUnitObj(iTargetUnitID);
                if(!pstTargetObj)
                {
                    LOGERROR("Failed to get combat unit obj, invalid target unit id %d, uin %u\n", iTargetUnitID, m_uiActiveUin);

                    return T_ZONE_SYSTEM_PARA_ERR;
                }

                iRet = pstTargetObj->DoBuffEffectByType(m_uiActiveUin, m_iCrossID, iTriggerType, iTriggerUnitID, *pstNotify);
                if(iRet)
                {
                    LOGERROR("Failed to do buff effect, trigger type %d, unit id %d, uin %u\n", iTriggerType, iTargetUnitID, m_uiActiveUin);
                    return iRet;
                }
            }
            break;

        case BUFF_TRIGGER_PROCESSDAMAGE:
            {
                //获取目标战斗单位的Obj
                CCombatUnitObj* pstTargetObj = CCombatUtility::GetCombatUnitObj(iTargetUnitID);
                if(!pstTargetObj)
                {
                    LOGERROR("Failed to get combat unit obj, invalid target unit id %d, uin %u\n", iTargetUnitID, m_uiActiveUin);

                    return T_ZONE_SYSTEM_PARA_ERR;
                }

                iRet = pstTargetObj->DoBuffEffectByType(m_uiActiveUin, m_iCrossID, iTriggerType, iTriggerUnitID, *pstNotify, pDamageNum);
                if(iRet)
                {
                    LOGERROR("Failed to do buff effect, trigger type %d, unit id %d, uin %u\n", iTriggerType, iTargetUnitID, m_uiActiveUin);
                    return iRet;
                }
            }
            break;

        default:
            {
                LOGERROR("Failed to do battlefiled buff effect, invalid trigger type %d\n", iTriggerType);
                return T_ZONE_SYSTEM_PARA_ERR;
            }
    }

    if(pstNotify->steffects_size() != 0)
    {
        SendNotifyToBattlefield(stDoBuffEffect_Notify);
    }

    return T_SERVER_SUCESS;
}

int CBattlefieldObj::DecreaseBuffRound()
{
    //先推送减少战斗单位BUFF回合数的消息
    static GameProtocolMsg stMsg;
    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_DECREASEUNITBUFF_NOTIFY);

    stMsg.mutable_m_stmsgbody()->mutable_m_stzone_decreaseunitbuff_notify()->set_icombatunitid(GetActionUnitID());
    SendNotifyToBattlefield(stMsg);

    CZoneMsgHelper::GenerateMsgHead(m_stRemoveBuff_Notify, MSGID_ZONE_REMOVEBUFF_NOTIFY);
    Zone_RemoveBuff_Notify* pstNotify = m_stRemoveBuff_Notify.mutable_m_stmsgbody()->mutable_m_stzone_removebuff_notify();

    //获取当前战斗单位
    CCombatUnitObj* pstActionObj = CCombatUtility::GetCombatUnitObj(GetActionUnitID());
    if(!pstActionObj)
    {
        LOGERROR("Failed to get combat unit obj, invalid action unit id %d\n", GetActionUnitID());
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = pstActionObj->DecreaseBuffRound(*pstNotify);
    if(iRet)
    {
        LOGERROR("Failed to decrease buff round, ret %d, uin %u\n", iRet, m_uiActiveUin);
        return iRet;
    }

    if(pstNotify->steffects_size() != 0)
    {
        SendNotifyToBattlefield(m_stRemoveBuff_Notify);
    }

    return T_SERVER_SUCESS;
}

//获取战斗胜负结果
bool CBattlefieldObj::GetIsActiveWin()
{
    return (m_iCombatResult == COMBAT_RESULT_ACTIVE_WIN);
}

//设置战斗结果,是否己方获胜
int CBattlefieldObj::SetCampActiveWin(int iIsActiveWin)
{
    if(iIsActiveWin)
    {
        //主动战斗方胜利
        m_iCombatResult = COMBAT_RESULT_ACTIVE_WIN;
    }
    else
    {
        //被动战斗方胜利
        m_iCombatResult = COMBAT_RESULT_PASSIVE_WIN;
    }

    return T_SERVER_SUCESS;
}

//获取关卡的ID
int CBattlefieldObj::GetCrossID()
{
    return m_iCrossID;
}

//设置当前行动单位的方向
int CBattlefieldObj::SetActionUnitDirection(int iDirection)
{
    if(iDirection <= COMBAT_DIRECTION_INVALID || iDirection > COMBAT_DIRECTION_SOUTH)
    {
        //非法的方向，直接返回
        return T_SERVER_SUCESS;
    }

    CCombatUnitObj* pstActionObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionObj)
    {
        LOGERROR("Failed to get action unit obj, unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    pstActionObj->SetUnitDirection(iDirection);

    return T_SERVER_SUCESS;
}

//获取阵营战斗单位的信息
int CBattlefieldObj::GetCombatUnitByCamp(int iCampType, int iUnitIndex)
{
    if(iUnitIndex < 0 || iUnitIndex >= MAX_CAMP_FIGHT_UNIT_NUM)
    {
        return -1;
    }

    CCombatUnitObj* pstUnitObj = NULL;
    if(iCampType == FIGHT_CAMP_ACTIVE)
    {
        //战斗主动发起方
        if(m_aiActiveCombatObjIndex[iUnitIndex] < 0)
        {
            return -1;
        }

        pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiActiveCombatObjIndex[iUnitIndex]);
       
    }
    else if(iCampType == FIGHT_CAMP_PASSIVE)
    {
        //战斗被动接受方
        if(m_aiPassiveCombatObjIndex[iUnitIndex] < 0)
        {
            return -1;
        }

        pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiPassiveCombatObjIndex[iUnitIndex]);
    }

    if(!pstUnitObj || pstUnitObj->IsCombatUnitDead())
    {
        return -1;
    }


    return pstUnitObj->GetCombatUnitID();
}

//战斗单位的回合数
int CBattlefieldObj::GetCombatRoundNum()
{
    return m_iCombatRoundNum;
}

void CBattlefieldObj::SetCombatRoundNum(int iRoundNum)
{
    m_iCombatRoundNum = iRoundNum;
}

//增加战场上的战斗单位
int CBattlefieldObj::AddCombatUnit(int iCampType, int iConfigID, int iPosX, int iPosY, int iDirection, int iUnitType, int& iUnitID)
{
    /*
    //返回增加的战斗单位unitID
    iUnitID = -1;

    //读取怪物配置
    const SMonsterConfig* pstMonsterConfig = MonsterCfgMgr().GetConfig(iConfigID);
    if(!pstMonsterConfig)
    {
        LOGERROR("Failed to get monster config, invalid id %d\n", iConfigID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int* piCombatUnitObjIndex = NULL;
    if(iCampType == FIGHT_CAMP_ACTIVE)
    {
        piCombatUnitObjIndex = m_aiActiveCombatObjIndex;
    }
    else if(iCampType == FIGHT_CAMP_PASSIVE)
    {
        piCombatUnitObjIndex = m_aiPassiveCombatObjIndex;
    }

    if(!piCombatUnitObjIndex)
    {
        LOGERROR("Failed to get combat unit index , invalid camp type %d, uin %u\n", iCampType, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //查找当前空闲的index
    int iIndex = -1;
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(piCombatUnitObjIndex[i] < 0)
        {
            iIndex = i;
            break;
        }
    }

    if(iIndex < 0)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //分配CCombatUnitObj
    CCombatUnitObj* pstCombatUnitObj = CreateCombatUnitObj(piCombatUnitObjIndex[iIndex]);
    if(!pstCombatUnitObj)
    {
        LOGERROR("Failed to create combat unit obj!\n");
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    TUNITPOSITION stPos(iPosX, iPosY);
    TUNITPOSITION stTmpPos;
    bool bFoundPos = false;
    for(int i=0; i<4; ++i)
    {
        //右
        stTmpPos.iPosX = stPos.iPosX + i;
        stTmpPos.iPosY = stPos.iPosY;
        if(IsPosCanWalk(stTmpPos))
        {
            //该位置没有人并且可行走
            bFoundPos = true;
            break;
        }
        
        //左
        stTmpPos.iPosX = stPos.iPosX - i;
        stTmpPos.iPosY = stPos.iPosY;
        if(IsPosCanWalk(stTmpPos))
        {
            //该位置没有人并且可行走
            bFoundPos = true;
            break;
        }

        //上
        stTmpPos.iPosX = stPos.iPosX;
        stTmpPos.iPosY = stPos.iPosY + i;
        if(IsPosCanWalk(stTmpPos))
        {
            //该位置没有人并且可行走
            bFoundPos = true;
            break;
        }

        //下
        stTmpPos.iPosX = stPos.iPosX;
        stTmpPos.iPosY = stPos.iPosY - i;
        if(IsPosCanWalk(stTmpPos))
        {
            //该位置没有人并且可行走
            bFoundPos = true;
            break;
        }
    }

    if(!bFoundPos)
    {
        //没找到合法的点
        LOGERROR("Failed to add combat unit, can not find valid pos, uin %u\n", m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    SFightPosInfo stTmpPosInfo;
    stTmpPosInfo.iPosX = stTmpPos.iPosX;
    stTmpPosInfo.iPosY = stTmpPos.iPosY;
    int iRet = pstCombatUnitObj->InitMonsterUnitObj(piCombatUnitObjIndex[iIndex], *pstMonsterConfig, stTmpPosInfo, iUnitType);
    if(iRet)
    {
        LOGERROR("Failed to init monster unit obj, monster id %d, ret %d, uin %u\n", iConfigID, iRet, m_uiActiveUin);
        return iRet;
    }

    iUnitID = piCombatUnitObjIndex[iIndex];

    pstCombatUnitObj->SetUnitDirection(iDirection);

    //添加到行动结束队列
    m_aiActionDoneUnit[m_iActionDoneUnitNum++] = iUnitID;

    LOGDEBUG("Success to init one monster info, monster id %d, uin %u\n", iConfigID, m_uiActiveUin);

    //推送给客户端的消息
    static GameProtocolMsg stAddUnitNotify;
    CZoneMsgHelper::GenerateMsgHead(stAddUnitNotify, MSGID_ZONE_ADDCOMBATUNIT_NOTIFY);
    
    Zone_AddCombatUnit_Notify* pstNotify = stAddUnitNotify.mutable_m_stmsgbody()->mutable_m_stzone_addcombatunit_notify();
    pstNotify->set_icamptype(iCampType);
    
    //封装单位的详细信息
    PackOneCombatUnitInfo(iCampType, piCombatUnitObjIndex[iIndex], *pstNotify->mutable_stunit());
    
    //推送消息给客户端
    SendNotifyToBattlefield(stAddUnitNotify);

    //释放战斗单位的登场技能
    std::vector<int> vSkillIDs;
    pstCombatUnitObj->GetSkillByType(SKILL_USE_ON_STAGE, vSkillIDs);

    for(unsigned i=0; i<vSkillIDs.size(); ++i)
    {
        iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, piCombatUnitObjIndex[iIndex], pstCombatUnitObj->GetUnitPosition(), vSkillIDs[i], SKILL_USE_ON_STAGE);
        if(iRet)
        {
            LOGERROR("Failed to cast combat unit skill, unit %d, skill %d, uin %u\n", piCombatUnitObjIndex[iIndex], vSkillIDs[i], m_uiActiveUin);
            return iRet;
        }
    } 
    */ 

    return T_SERVER_SUCESS;
}

//切换战斗单位的AI
int CBattlefieldObj::ChangeCombatUnitAI(int iCombatUnitID, int iNewAI)
{
    //获取战斗单位
    CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(iCombatUnitID);
    if(!pstUnitObj)
    {
        LOGERROR("Failed to get combat unit obj, invalid id %d, uin %u\n", iCombatUnitID, m_uiActiveUin);
        return T_ZONE_GAMEHERO_NOT_EXIST;
    } 

    int iRet = pstUnitObj->ChangeFightAI(iNewAI);
    if(iRet)
    {
        LOGERROR("Failed to change combat unit AI, unit id %d, new AI %d, uin %u\n", iCombatUnitID, iNewAI, m_uiActiveUin);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

//强制杀死战斗单位
int CBattlefieldObj::KillCombatUnit(int iCombatUnitID, bool bSendNotify)
{
    //获取战斗单位
    CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(iCombatUnitID);
    if(!pstUnitObj)
    {
        LOGERROR("Failed to get combat unit obj, invalid id %d, uin %u\n", iCombatUnitID, m_uiActiveUin);
        return T_ZONE_GAMEHERO_NOT_EXIST;
    } 

    int iRet = pstUnitObj->AddFightAttr(FIGHT_ATTR_HP, -pstUnitObj->GetFightAttr(FIGHT_ATTR_HP));
    if(iRet)
    {
        LOGERROR("Failed to kill combat unit, id %d, uin %u\n", iCombatUnitID, m_uiActiveUin);
        return iRet;
    }

    //杀死主动方单位，保存杀死的单位配置ID
    if(GetCombatUnitCamp(iCombatUnitID) == FIGHT_CAMP_ACTIVE)
    {
        m_aiActiveScriptKilledConfigID[m_iActiveScriptKilledNum++] = pstUnitObj->GetConfigID();
    }

    if(bSendNotify)
    {
        //推送强制杀死的消息
        static GameProtocolMsg stKillUnitNotify;
        CZoneMsgHelper::GenerateMsgHead(stKillUnitNotify, MSGID_ZONE_KILLCOMBATUNIT_NOTIFY);
    
        Zone_KillCombatUnit_Notify* pstNotify = stKillUnitNotify.mutable_m_stmsgbody()->mutable_m_stzone_killcombatunit_notify();
        pstNotify->set_iunitid(iCombatUnitID);
    
        SendNotifyToBattlefield(stKillUnitNotify);
    }

    //清理死亡的单位
    ClearDeadCombatUnit();

    return T_SERVER_SUCESS;
}

//获取战斗开始时间
int CBattlefieldObj::GetCombatStartTime()
{
    return m_iStartTime;
}

//处理行动回合开始前的技能
int CBattlefieldObj::DoRoundActionSkill()
{
    //获取当前行动单位
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get action unit obj, unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //获取战斗单位所有技能
    std::vector<int> vSkillIDs;
    pstActionUnitObj->GetSkillByType(SKILL_USE_ROUNDACTION, vSkillIDs);

    //释放技能
    int iRet = T_SERVER_SUCESS;
    for(unsigned i=0; i<vSkillIDs.size(); ++i)
    {
        iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, m_iActionUnit, pstActionUnitObj->GetUnitPosition(), vSkillIDs[i], SKILL_USE_ROUNDACTION);
        if(iRet)
        {
            LOGERROR("Failed to casst combat skill, unit %d, skill %d, uin %u\n", m_iActionUnit, vSkillIDs[i], m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//获取最大移动距离，考虑围困逻辑
int CBattlefieldObj::GetMaxMoveDistance(CCombatUnitObj& stUnitObj)
{
    if(stUnitObj.GetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTMOVE))
    {
        //不能移动
        return 0;
    }

    int iNearByEnemyNum = 0;
    if(!stUnitObj.GetCombatUnitStatus(COMBAT_UNIT_STATUS_IGNOREJOINT) && stUnitObj.GetUnitSize()==0)
    {
        //没有无视合围的状态并且不是大体积单位，计算敌人数目
        iNearByEnemyNum = GetNearByUnitNum(stUnitObj, true);
    }

    int iMoveDistance = (stUnitObj.GetFightAttr(FIGHT_ATTR_SPEED)/10)-iNearByEnemyNum;
    if(iMoveDistance <= 0)
    {
        iMoveDistance = 1;
    }

    return iMoveDistance;
}

//获取战斗单位周围的阵营单位数量
int CBattlefieldObj::GetNearByUnitNum(CCombatUnitObj& stUnitObj, bool bIsEnemy)
{
    int iTotalUnitNum = 0;
    TUNITPOSITION& stPos = stUnitObj.GetUnitPosition();

    //检查左边
    CCombatUnitObj* pstNearUnitObj = GetCombatUnitByPos(TUNITPOSITION(stPos.iPosX-1, stPos.iPosY));
    if(pstNearUnitObj && 
       ((bIsEnemy&&!IsUnitInSameCamp(stUnitObj,*pstNearUnitObj)) || (!bIsEnemy&&IsUnitInSameCamp(stUnitObj,*pstNearUnitObj))))
    {
        ++iTotalUnitNum;
    }

    //检查右边
    pstNearUnitObj = GetCombatUnitByPos(TUNITPOSITION(stPos.iPosX+1, stPos.iPosY));
    if(pstNearUnitObj && 
       ((bIsEnemy&&!IsUnitInSameCamp(stUnitObj,*pstNearUnitObj)) || (!bIsEnemy&&IsUnitInSameCamp(stUnitObj,*pstNearUnitObj))))
    {
        ++iTotalUnitNum;
    }

    //检查上边
    pstNearUnitObj = GetCombatUnitByPos(TUNITPOSITION(stPos.iPosX, stPos.iPosY+1));
    if(pstNearUnitObj && 
       ((bIsEnemy&&!IsUnitInSameCamp(stUnitObj,*pstNearUnitObj)) || (!bIsEnemy&&IsUnitInSameCamp(stUnitObj,*pstNearUnitObj))))
    {
        ++iTotalUnitNum;
    }

    //检查下边
    pstNearUnitObj = GetCombatUnitByPos(TUNITPOSITION(stPos.iPosX, stPos.iPosY-1));
    if(pstNearUnitObj && 
       ((bIsEnemy&&!IsUnitInSameCamp(stUnitObj,*pstNearUnitObj)) || (!bIsEnemy&&IsUnitInSameCamp(stUnitObj,*pstNearUnitObj))))
    {
        ++iTotalUnitNum;
    }

    return iTotalUnitNum;
}

//获取战斗单位斜角位置的阵营单位数量
int CBattlefieldObj::GetAngleNearByUnitNum(CCombatUnitObj& stUnitObj, bool bIsEnemy)
{
    int iTotalUnitNum = 0;
    TUNITPOSITION& stPos = stUnitObj.GetUnitPosition();

    //检查左下边
    CCombatUnitObj* pstNearUnitObj = GetCombatUnitByPos(TUNITPOSITION(stPos.iPosX-1, stPos.iPosY-1));
    if(pstNearUnitObj && 
       ((bIsEnemy&&!IsUnitInSameCamp(stUnitObj,*pstNearUnitObj)) || (!bIsEnemy&&IsUnitInSameCamp(stUnitObj,*pstNearUnitObj))))
    {
        ++iTotalUnitNum;
    }

    //检查左上边
    pstNearUnitObj = GetCombatUnitByPos(TUNITPOSITION(stPos.iPosX-1, stPos.iPosY+1));
    if(pstNearUnitObj && 
       ((bIsEnemy&&!IsUnitInSameCamp(stUnitObj,*pstNearUnitObj)) || (!bIsEnemy&&IsUnitInSameCamp(stUnitObj,*pstNearUnitObj))))
    {
        ++iTotalUnitNum;
    }

    //检查右下
    pstNearUnitObj = GetCombatUnitByPos(TUNITPOSITION(stPos.iPosX+1, stPos.iPosY-1));
    if(pstNearUnitObj && 
       ((bIsEnemy&&!IsUnitInSameCamp(stUnitObj,*pstNearUnitObj)) || (!bIsEnemy&&IsUnitInSameCamp(stUnitObj,*pstNearUnitObj))))
    {
        ++iTotalUnitNum;
    }

    //检查右上
    pstNearUnitObj = GetCombatUnitByPos(TUNITPOSITION(stPos.iPosX+1, stPos.iPosY+1));
    if(pstNearUnitObj && 
       ((bIsEnemy&&!IsUnitInSameCamp(stUnitObj,*pstNearUnitObj)) || (!bIsEnemy&&IsUnitInSameCamp(stUnitObj,*pstNearUnitObj))))
    {
        ++iTotalUnitNum;
    }

    return iTotalUnitNum;
}

//战场上某个位置是否可以行走
bool CBattlefieldObj::IsPosCanWalk(const TUNITPOSITION& stPos, int iUnitID)
{
    //该位置不能是阻挡或阻挡类型的机关
    if(!m_pstBattlePathManager->BattlefieldPosCanWalk(stPos) || GetBlockTrapByPos(stPos))
    {
        return false;
    }

    //该位置不能有人，如果是iUnitID的人则认为可走
    CCombatUnitObj* pstUnitObj = GetCombatUnitByPos(stPos);
    if(pstUnitObj && pstUnitObj->GetCombatUnitID()!=iUnitID)
    {
        return false;
    }

    return true;
}

//获取战斗地图阻挡配置
const CScenePathManager& CBattlefieldObj::GetBattlePathManager()
{
    return *m_pstBattlePathManager;
}

//战场上增加机关,返回TrapObj对象的ID
int CBattlefieldObj::AddCombatTrap(int iConfigID, int iCamp, const TUNITPOSITION& stPos, int iDirection)
{
    //是否超过支持的最大数量
    if(m_iTrapNum >= MAX_COMBAT_TRAP_NUM)
    {
        LOGERROR("Failed to add combat trap, number reach max %d, uin %u\n", MAX_COMBAT_TRAP_NUM, m_uiActiveUin);
        return -1;
    }

    //该位置是否阻挡或者有人
    if(!IsPosCanWalk(stPos))
    {
        LOGERROR("Failed to add combat trap, pos %d:%d, battlefield id %d, uin %u\n", stPos.iPosX, stPos.iPosY, m_iCrossID, m_uiActiveUin);
        return -1;
    }

    //该位置是否有机关
    if(GetTrapByPos(stPos))
    {
        //该位置已经有机关
        LOGERROR("Failed to add combat trap, pos %d:%d already have trap, uin %u\n", stPos.iPosX, stPos.iPosY, m_uiActiveUin);
        return -1;
    }

    //放置机关
    m_aiCombatTrapIndex[m_iTrapNum] = GameType<CCombatTrapObj>::Create();
    if(m_aiCombatTrapIndex[m_iTrapNum] < 0)
    {
        LOGERROR("Failed to create combat trap obj, uin %u\n", m_uiActiveUin);
        return -1;
    }

    CCombatTrapObj* pstTrapObj = GameType<CCombatTrapObj>::Get(m_aiCombatTrapIndex[m_iTrapNum]);
    if(!pstTrapObj)
    {
        LOGERROR("Failed to get combat trap obj, uin %u\n", m_uiActiveUin);
        return -1;
    }

    ++m_iTrapNum;

    //初始化机关
    int iRet = pstTrapObj->InitTrap(iConfigID, iCamp, stPos, iDirection);
    if(iRet)
    {
        LOGERROR("Failed to init combat trap, ret %d, trap id %d, uin %u\n", iRet, iConfigID, m_uiActiveUin);
        return -1;
    }

    //推送增加机关的通知
    static GameProtocolMsg stMsg;
    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_ADDCOMBATTRAP_NOTIFY);

    Zone_AddCombatTrap_Notify* pstNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_addcombattrap_notify();
    pstNotify->set_iconfigid(iConfigID);
    pstNotify->set_icamp(iCamp);
    pstNotify->set_itrapobjid(m_aiCombatTrapIndex[m_iTrapNum-1]);
    pstNotify->set_idirection(iDirection);
    pstNotify->mutable_stpos()->set_iposx(stPos.iPosX);
    pstNotify->mutable_stpos()->set_iposy(stPos.iPosY);

    SendNotifyToBattlefield(stMsg);

    return m_aiCombatTrapIndex[m_iTrapNum-1];
}

//触发战场上的机关
int CBattlefieldObj::TriggerCombatTrap(int iTriggerObjID, int iTrapObjID, int iTriggerType)
{
    //获取TrapObj
    CCombatTrapObj* pstTrapObj = GameType<CCombatTrapObj>::Get(iTrapObjID);
    if(!pstTrapObj)
    {
        LOGERROR("Failed to get combat trap obj, obj id %d, uin %u\n", iTrapObjID, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //判断触发类型
    if(pstTrapObj->GetConfig()->iTriggerType != iTriggerType)
    {
        return T_SERVER_SUCESS;
    }

    //如果是主动触发，则处理停止行动和移动等逻辑
    if(iTriggerType != TRAP_TRIGGER_PASSIVE)
    {
        CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(iTriggerObjID);
        if(!pstUnitObj)
        {
            LOGERROR("Failed to get trap trigger unit, obj id %d, uin %u\n", iTriggerObjID, m_uiActiveUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        switch(pstTrapObj->GetConfig()->iInterruptType)
        {
            case TRAP_INTERRUPT_MOVE:
                {
                    // 设置不能移动的状态
                    pstUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTMOVE, true);
                }
                break;

            case TRAP_INTERRUPT_ACTION:
                {
                    //设置不能行动状态
                    pstUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTACTION, true);
                }
                break;

            case TRAP_INTERRUPT_ALL:
                {
                    //设置不能移动并且不能行动
                    pstUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTMOVE, true);
                    pstUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTACTION, true);
                }
                break;

            default:
                {
                    //不打断任何行为
                    ;
                }
                break;
        }
    }

    //推送给客户端的消息
    static GameProtocolMsg stMsg;
    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_TRIGGERTRAP_NOTIFY);

    Zone_TriggerCombatTrap_Notify* pstNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_triggercombattrap_notify();
    pstNotify->set_itrapobjid(iTrapObjID);
    pstNotify->set_itriggerobjid(iTriggerObjID);
    pstNotify->set_itriggertype(iTriggerType);
    pstNotify->set_iinterrupttype(pstTrapObj->GetConfig()->iInterruptType);

    SendNotifyToBattlefield(stMsg);

    //todo jasonxiong5 重新开发脚本
    //执行脚本函数
    //CModuleHelper::GetStoryFramework()->DoTrapTriggerScript(m_uiActiveUin, iTriggerObjID, iTrapObjID, pstTrapObj->GetTrapConfigID(), iTriggerType);

    return T_SERVER_SUCESS;
}

//清除战场上的机关
void CBattlefieldObj::DeleteCombatTrap(int iTrapObjID)
{
    //先从战场信息中删除
    for(int i=0; i<m_iTrapNum; ++i)
    {
        if(m_aiCombatTrapIndex[i] == iTrapObjID)
        {
            m_aiCombatTrapIndex[i] = m_aiCombatTrapIndex[--m_iTrapNum];
            break;
        }
    }
    
    //删除对象
    GameType<CCombatTrapObj>::Del(iTrapObjID);

    //推送给客户端的移除通知
    static GameProtocolMsg stGameMsg;
    CZoneMsgHelper::GenerateMsgHead(stGameMsg, MSGID_ZONE_DELETECOMBATTRAP_NOTIFY);

    Zone_DeleteCombatTrap_Notify* pstNotify = stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_deletecombattrap_notify();
    pstNotify->set_itrapobjid(iTrapObjID);

    SendNotifyToBattlefield(stGameMsg);

    return;
}

//触发陷阱类型的机关
int CBattlefieldObj::TriggerPitfallTrap(int iTriggerType)
{
    //获取战斗单位
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get action unit obj, invalid unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //获取机关
    CCombatTrapObj* pstTrapObj = GetTrapByPos(pstActionUnitObj->GetUnitPosition());
    if(!pstTrapObj || pstTrapObj->GetConfig()->iTriggerType != iTriggerType)
    {
        return T_SERVER_SUCESS;
    }

    return TriggerCombatTrap(m_iActionUnit, GetTrapIndexByPos(pstActionUnitObj->GetUnitPosition()), iTriggerType);
}

//手动触发机关
int CBattlefieldObj::ManualTriggerTrap(const TUNITPOSITION& stPos)
{
    //获取行动单位对象
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get action unit obj, unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //获取机关对象
    CCombatTrapObj* pstTrapObj = GetTrapByPos(stPos);
    if(!pstTrapObj)
    {
        LOGERROR("Failed to get combat trap obj, pos %d:%d, uin %u\n", stPos.iPosX, stPos.iPosY, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //判断机关是否可见、是否死亡
    if(!IsTrapVisible(*pstActionUnitObj, *pstTrapObj) || pstTrapObj->IsTrapDead())
    {
        LOGERROR("Failed to trigger combat trap, unit id %d, trap pos %d:%d, uin %u\n", m_iActionUnit, stPos.iPosX, stPos.iPosY, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //判断触发距离是否合法
    int iDistance = CCombatUtility::GetAttackDistance(pstActionUnitObj->GetUnitPosition(), stPos, pstTrapObj->GetConfig()->iRangeID);
    if(iDistance == 0)
    {
        //触发距离不合法
        LOGERROR("Failed to trigger combat trap, invalid distance, uin %u\n", m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //触发机关
    int iRet = TriggerCombatTrap(m_iActionUnit, GetTrapIndexByPos(stPos), TRAP_TRIGGER_PASSIVE);
    if(iRet)
    {
        LOGERROR("Failed to trigger combat trap, type %d, uin %u\n", TRAP_TRIGGER_PASSIVE, m_uiActiveUin);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

//攻击战场机关
int CBattlefieldObj::AttackCombatTrap(const TUNITPOSITION& stPos)
{
    //推送的攻击消息
    static GameProtocolMsg stGameMsg;
    CZoneMsgHelper::GenerateMsgHead(stGameMsg, MSGID_ZONE_COMBATACTION_NOTIFY);

    Zone_CombatAction_Notify* pstNotify = stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_combataction_notify();
    pstNotify->set_iactionunitid(m_iActionUnit);
    pstNotify->mutable_sttargetpos()->set_iposx(stPos.iPosX);
    pstNotify->mutable_sttargetpos()->set_iposy(stPos.iPosY);
    pstNotify->set_etype(COMBAT_ACTION_CASTSKILL);
    pstNotify->set_iuseskilltype(SKILL_USE_ACTIVE);

    //获取行动单位对象
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get action unit obj, unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //获取机关对象
    CCombatTrapObj* pstTrapObj = GetTrapByPos(stPos);
    if(!pstTrapObj)
    {
        LOGERROR("Failed to get combat trap obj, pos %d:%d, uin %u\n", stPos.iPosX, stPos.iPosY, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //获取行动单位的普攻配置
    const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(pstActionUnitObj->GetNormalSkillID());
    if(!pstSkillConfig)
    {
        LOGERROR("Failed to get fight skill config, skill id %d\n", pstActionUnitObj->GetNormalSkillID());
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    pstNotify->set_icastskillid(pstActionUnitObj->GetNormalSkillID());

    //判断机关是否可见
    if(!IsTrapVisible(*pstActionUnitObj, *pstTrapObj) || pstTrapObj->IsTrapDead() || !pstTrapObj->IsTrapCanAttacked())
    {
        LOGERROR("Failed to trigger combat trap, unit id %d, trap pos %d:%d, uin %u\n", m_iActionUnit, stPos.iPosX, stPos.iPosY, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //判断触发距离是否合法
    int iDistance = CCombatUtility::GetAttackDistance(pstActionUnitObj->GetUnitPosition(), stPos, pstSkillConfig->iTargetRangeID);
    if(iDistance == 0)
    {
        //触发距离不合法
        LOGERROR("Failed to attack combat trap, invalid distance, uin %u\n", m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //扣除机关血量
    pstTrapObj->AddTrapHP(-1);
    
    pstNotify->set_inewtraphp(pstTrapObj->GetTrapHP());

    if(pstTrapObj->IsTrapDead())
    {
        //已经死亡，触发机关
        int iRet = TriggerCombatTrap(m_iActionUnit, GetTrapIndexByPos(stPos), TRAP_TRIGGER_DESTROY);
        if(iRet)
        {
            LOGERROR("Failed to trigger combat trap, type %d, uin %u\n", TRAP_TRIGGER_DESTROY, m_uiActiveUin);
            return iRet;
        }
    }

    //推送机关被攻击的消息
    SendNotifyToBattlefield(stGameMsg);

    return T_SERVER_SUCESS;
}

//判断机关是否可见
bool CBattlefieldObj::IsTrapVisible(CCombatUnitObj& stUnitObj, CCombatTrapObj& stTrapObj)
{
    //获取单位的阵营
    int iActionCamp = GetCombatUnitCamp(stUnitObj.GetCombatUnitID());

    //获取机关的阵营
    int iTrapCamp = stTrapObj.GetTrapCamp();
    
    switch(stTrapObj.GetConfig()->iVisiableType)
    {
        case TRAP_VISIBALE_ALL:
            {
                //所有人可见
                return true;
            }
            break;

        case TRAP_VISIBALE_SELF:
            {
                //己方阵营可见
                if(iActionCamp == iTrapCamp)
                {
                    return true;
                }
            }
            break;

        default:
            {
                ;
            }
            break;
    }

    return false;
}

bool CBattlefieldObj::IsTrapVisibleToActionUnit(CCombatTrapObj& stTrapObj)
{
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        return false;
    }

    return IsTrapVisible(*pstActionUnitObj, stTrapObj);
}

//获取战场机关详细信息
void CBattlefieldObj::GetCombatTrapInfo(std::vector<TCOMBATTRAPINFO>& vTrapInfo)
{
    for(int i=0; i<m_iTrapNum; ++i)
    {
        CCombatTrapObj* pstTrapObj = GameType<CCombatTrapObj>::Get(m_aiCombatTrapIndex[i]);
        if(!pstTrapObj)
        {
            continue;
        }

        TCOMBATTRAPINFO stTrapInfo;
        stTrapInfo.iConfigID = pstTrapObj->GetTrapConfigID();
        stTrapInfo.iTrapObjID = m_aiCombatTrapIndex[i];

        vTrapInfo.push_back(stTrapInfo);
    }

    return;
}

void CBattlefieldObj::AddCombatUnitAttr(CCombatUnitObj& stUnitObj, int iAttrType, int iAddValue)
{
    if(iAttrType<0 || iAttrType>=FIGHT_ATTR_MAX)
    {
        return;
    }

    int iRealAddValue = 0;
    stUnitObj.AddFightAttr(iAttrType, iAddValue, &iRealAddValue);

    if(iRealAddValue != 0)
    {
        //推送消息通知
        static GameProtocolMsg stMsg;
        CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_UNITATTRCHANGE_NOTIFY);
    
        Zone_UnitAttrChange_Notify* pstNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_unitattrchange_notify();
        pstNotify->set_iunitid(stUnitObj.GetCombatUnitID());
        pstNotify->set_iattrtype(iAttrType);
        pstNotify->set_iattrchange(iRealAddValue);
    
        SendNotifyToBattlefield(stMsg);
    }

    return;
}

//删除战斗单位身上的BUFF
void CBattlefieldObj::DelUnitBuff(int iUnitID, int iBuffID)
{
    CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(iUnitID);
    if(!pstUnitObj || !pstUnitObj->HasBuffOfSameID (iBuffID))
    {
        //战斗单位不存在或者身上没有BUFF
        return;
    }

    static GameProtocolMsg stMsg;
    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_REMOVEBUFF_NOTIFY);

    Zone_RemoveBuff_Notify* pstNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_removebuff_notify();
    pstNotify->set_itargetunitid(iUnitID);

    pstUnitObj->DelUnitBuff(iBuffID, *pstNotify->add_steffects());

    SendNotifyToBattlefield(stMsg);

    return;
}

//推送消息给战场上所有玩家
void CBattlefieldObj::SendNotifyToBattlefield(GameProtocolMsg& stNotify)
{
    CZoneMsgHelper::SendZoneMsgToRole(stNotify, CUnitUtility::GetRoleByUin(m_uiActiveUin));
    CZoneMsgHelper::SendZoneMsgToRole(stNotify, CUnitUtility::GetRoleByUin(m_uiPassiveUin));

    return;
}

//当前行动的玩家角色是否有效
bool CBattlefieldObj::IsValidActionRole(unsigned int uin)
{
    if((GetCombatUnitCamp(m_iActionUnit)==FIGHT_CAMP_ACTIVE && uin==m_uiActiveUin)
       || (GetCombatUnitCamp(m_iActionUnit)==FIGHT_CAMP_PASSIVE && uin==m_uiPassiveUin))
    {
        return true;
    }

    return false;
}

//是否需要设置阵型
bool CBattlefieldObj::IsNeedSetForm()
{
    return (m_bActiveNeedSetForm || m_bPassiveNeedSetForm);
}

//设置战斗阵型
int CBattlefieldObj::SetCombatForm(unsigned int uin, const CombatForm& stForm)
{
    //检查当前战场状态
    if(GetActionUnitStatus() != ROLE_COMBAT_UNIT_STAT_SETFORM)
    {
        LOGERROR("Failed to set combat form, invalid battle status, uin %u\n", uin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //检查unitid是否合法
    int* paiCombatObjIndex = (uin==m_uiActiveUin) ? m_aiActiveCombatObjIndex : m_aiPassiveCombatObjIndex;

    int iTotalUnitNum = 0;
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(paiCombatObjIndex[i] < 0)
        {
            break;
        }

        ++iTotalUnitNum;
    }

    if(stForm.stunitinfos_size() != iTotalUnitNum)
    {
        LOGERROR("Failed to set combat unit form, invalid set form number %d, uin %u\n", stForm.stunitinfos_size(), uin);
        return T_ZONE_SYSTEM_PARA_ERR;;
    }

    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(paiCombatObjIndex[i] < 0)
        {
            break;
        }

        if(paiCombatObjIndex[i] != stForm.stunitinfos(i).iunitid())
        {
            LOGERROR("Failed to set combat form, invalid unit id %d:%d, uin %u\n", paiCombatObjIndex[i], stForm.stunitinfos(i).iunitid(), uin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }
    }
    
    //设置所有战斗单位的位置
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(paiCombatObjIndex[i] < 0)
        {
            break;
        }

        CCombatUnitObj* pstCombatUnitObj = CCombatUtility::GetCombatUnitObj(paiCombatObjIndex[i]);
        if(!pstCombatUnitObj)
        {
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        pstCombatUnitObj->SetUnitPosition(TUNITPOSITION(stForm.stunitinfos(i).stpos().iposx(), stForm.stunitinfos(i).stpos().iposy()));
    }

    //设置标记
    if(uin == m_uiActiveUin)
    {
        m_bActiveNeedSetForm = false;
    }
    else
    {
        m_bPassiveNeedSetForm = false;
    }

    if(!IsNeedSetForm())
    {
        //阵型已经设置完成，设置战场状态
        SetActionUnitStatus(ROLE_COMBAT_UNIT_STAT_INVALID);
    }

    return T_SERVER_SUCESS;
}

//推送战斗阵型的通知
void CBattlefieldObj::SendCombatFormNotify()
{
    static GameProtocolMsg stMsg;
    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_SETCOMBATFORM_NOTIFY);

    Zone_SetCombatForm_Notify* pstNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_setcombatform_notify();

    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_aiActiveCombatObjIndex[i] >= 0)
        {
            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiActiveCombatObjIndex[i]);
            if(pstUnitObj && !pstUnitObj->IsCombatUnitDead())
            {
                OneCombatUnitInfo* pstFormInfo = pstNotify->mutable_stactiveform()->add_stunitinfos();
                pstFormInfo->set_iunitid(m_aiActiveCombatObjIndex[i]);
                pstFormInfo->mutable_stpos()->set_iposx(pstUnitObj->GetUnitPosition().iPosX);
                pstFormInfo->mutable_stpos()->set_iposy(pstUnitObj->GetUnitPosition().iPosY);
            }
        }

        if(m_aiPassiveCombatObjIndex[i] >= 0)
        {
            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiPassiveCombatObjIndex[i]);
            if(pstUnitObj && !pstUnitObj->IsCombatUnitDead())
            {
                OneCombatUnitInfo* pstFormInfo = pstNotify->mutable_stpassiveform()->add_stunitinfos();
                pstFormInfo->set_iunitid(m_aiPassiveCombatObjIndex[i]);
                pstFormInfo->mutable_stpos()->set_iposx(pstUnitObj->GetUnitPosition().iPosX);
                pstFormInfo->mutable_stpos()->set_iposy(pstUnitObj->GetUnitPosition().iPosY);
            }
        }
    }

    SendNotifyToBattlefield(stMsg);

    return;
}

//推送当前战场的出手信息
void CBattlefieldObj::SendActionUnitNotify()
{
    static GameProtocolMsg stMsg;
    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_ACTIONUNITINFO_NOTIFY);

    Zone_ActionUnitInfo_Notify* pstNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_actionunitinfo_notify();

    for(int i=0; i<m_iCanCombatUnitNum; ++i)
    {
        if(m_aiCanCombatUnit[i] < 0)
        {
            continue;
        }

        CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiCanCombatUnit[i]);
        if(!pstUnitObj || pstUnitObj->IsCombatUnitDead())
        {
            continue;
        }

        OneActionInfo* pstOneInfo = pstNotify->add_stinactioninfos();
        pstOneInfo->set_iunitid(m_aiCanCombatUnit[i]);
    }

    for(int i=0; i<m_iActionDoneUnitNum; ++i)
    {
        if(m_aiActionDoneUnit[i] < 0)
        {
            continue;
        }

        CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiActionDoneUnit[i]);
        if(!pstUnitObj || pstUnitObj->IsCombatUnitDead())
        {
            continue;
        }

        OneActionInfo* pstOneInfo = pstNotify->add_stactiondoneinfos();
        pstOneInfo->set_iunitid(m_aiActionDoneUnit[i]);
    }

    SendNotifyToBattlefield(stMsg);

    return;
}

//保存战场战斗单位详细信息
void CBattlefieldObj::SaveCombatUnitInfo()
{
    CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(m_uiActiveUin);
    if(!pstRoleObj)
    {
        return;
    }

    //先设置主动方阵型中除脚本杀死的单位外HP、MP为1
    std::vector<int> vExcludeConfigID;
    for(int i=0; i<m_iActiveScriptKilledNum; ++i)
    {
        vExcludeConfigID.push_back(m_aiActiveScriptKilledConfigID[i]);
    }

    //保存主动方存活单位的HP和MP
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_aiActiveCombatObjIndex[i] < 0)
        {
            continue;
        }

        CCombatUnitObj* pstCombatUnitObj = CCombatUtility::GetCombatUnitObj(m_aiActiveCombatObjIndex[i]);
        if(!pstCombatUnitObj)
        {
            continue;
        }

        CFightUnitUtility::SaveCombatUnitAttr(*pstRoleObj, pstCombatUnitObj->GetConfigID(), FIGHT_ATTR_HP, pstCombatUnitObj->GetFightAttr(FIGHT_ATTR_HP));
    }

    return;
}

//清理战场上死掉的单位
void CBattlefieldObj::ClearDeadCombatUnit()
{
   CCombatUnitObj* pstUnitObj = NULL;

    //先清理主动方的
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM;)
    {
        if(m_aiActiveCombatObjIndex[i] < 0)
        {
            break;
        }
        
        pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiActiveCombatObjIndex[i]);
        if(!pstUnitObj || !pstUnitObj->IsCombatUnitDead())
        {
            ++i;
            continue;
        }

        //单位已经死亡，清理
        ClearOneCombatUnitObj(m_aiActiveCombatObjIndex[i]);

        //从可行动队列中删除
        for(int j=0; j<m_iCanCombatUnitNum; ++j)
        {
            if(m_aiCanCombatUnit[j] == m_aiActiveCombatObjIndex[i])
            {
                m_aiCanCombatUnit[j] = m_aiCanCombatUnit[m_iCanCombatUnitNum-1];
                --m_iCanCombatUnitNum;

                break;
            }
        }

        //将后续的前移，保证顺序
        for(int j=i; j<(MAX_CAMP_FIGHT_UNIT_NUM-1); ++j)
        {
            m_aiActiveCombatObjIndex[j] = m_aiActiveCombatObjIndex[j+1];
        }

        m_aiActiveCombatObjIndex[MAX_CAMP_FIGHT_UNIT_NUM-1] = -1;
    }

    //再清理被动方的
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM;)
    {
        if(m_aiPassiveCombatObjIndex[i] < 0)
        {
            break;
        }
        
        pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiPassiveCombatObjIndex[i]);
        if(!pstUnitObj || !pstUnitObj->IsCombatUnitDead())
        {
            ++i;
            continue;
        }

        //单位已经死亡，清理
        ClearOneCombatUnitObj(m_aiPassiveCombatObjIndex[i]);

        //从可行动队列中删除
        for(int j=0; j<m_iCanCombatUnitNum; ++j)
        {
            if(m_aiCanCombatUnit[j] == m_aiPassiveCombatObjIndex[i])
            {
                m_aiCanCombatUnit[j] = m_aiCanCombatUnit[m_iCanCombatUnitNum-1];
                --m_iCanCombatUnitNum;

                break;
            }
        }

        //将后续的前移，保证顺序
        for(int j=i; j<(MAX_CAMP_FIGHT_UNIT_NUM-1); ++j)
        {
            m_aiPassiveCombatObjIndex[j] = m_aiPassiveCombatObjIndex[j+1];
        }

        m_aiPassiveCombatObjIndex[MAX_CAMP_FIGHT_UNIT_NUM-1] = -1;
    }

    return;
}

//减少行动单位技能CD回合
void CBattlefieldObj::DecreaseActionSkillCDRound()
{
    CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstUnitObj)
    {
        LOGERROR("Failed to decrease action skill cd round, uin %u\n", m_uiActiveUin);
        return;
    }

    static GameProtocolMsg stMsg;
    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_SKILLCDROUND_NOTIFY);

    Zone_SkillCDRound_Notify* pstNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_skillcdround_notify();

    pstUnitObj->DecreaseSkillCDRound(*pstNotify);
    SendNotifyToBattlefield(stMsg);

    return;
}

//初始化怪物信息
int CBattlefieldObj::InitBattlefieldInfo(int iCrossID)
{
    m_iCrossID = iCrossID;

    //读取战斗关卡配置
    const SBattleCrossConfig* pstCrossConfig = BattleCrossCfgMgr().GetConfig(m_iCrossID);
    if(!pstCrossConfig)
    {
        LOGERROR("Failed to get fight cross config, invalid cross id %d\n", m_iCrossID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //判断是否需要设置阵型
    m_bActiveNeedSetForm = false;
    m_bPassiveNeedSetForm = false;
    if(pstCrossConfig->iNeedSetForm)
    {
        //需要设置阵型
        m_bActiveNeedSetForm = true;

        if(m_iCrossID == PVP_COMBAT_CROSS_ID)
        {
            //如果是PVP战斗
            m_bPassiveNeedSetForm = true;
        }
    }

    //设置地图ID
    m_iMapID = pstCrossConfig->iMapID;

    //设置地图阻挡配置
    m_pstBattlePathManager = BattlefieldPathCfgMgr().GetBattlefieldPathConfigManager(m_iMapID);
    if(!m_pstBattlePathManager)
    {
        LOGERROR("Failed to get battlefiled path config, map id %d\n", m_iMapID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //获取双方合法的站位位置
    GetBattlefieldValidPos(*pstCrossConfig);

    return T_SERVER_SUCESS;
}

//初始化怪物信息
int CBattlefieldObj::InitMonsterInfo(int iCrossID)
{
    //读取战斗关卡配置
    const SBattleCrossConfig* pstCrossConfig = BattleCrossCfgMgr().GetConfig(iCrossID);
    if(!pstCrossConfig)
    {
        LOGERROR("Failed to get pve fight cross config, invalid cross id %d\n", iCrossID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //读取关卡怪物组配置
    const SCrossMonsterConfig* pstMonsterGroupConfig = CrossMonterCfgMgr().GetConfig(pstCrossConfig->iMonsterGroup);
    if(!pstMonsterGroupConfig)
    {
        LOGERROR("Failed to get monster group config, invalid id %d\n", pstCrossConfig->iMonsterGroup);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = T_SERVER_SUCESS;

    //初始化怪物战斗信息
    for(int i=0; i<pstMonsterGroupConfig->iMonsterNum; ++i)
    {
        const OneCrossMonster& stOneConfig = pstMonsterGroupConfig->astMonsterInfo[i];
        if(stOneConfig.iMonsterID==0 || m_astPassivePos[i].iPosX<0)
        {
            //有效怪物的配置是连续的，碰到ID为0认为结束了
            break;
        }

        iRet = InitOneMonsterInfo(i, stOneConfig, m_astPassivePos[i]);
        if(iRet)
        {
            LOGERROR("Failed to init one monster info, monster id %d, ret %d, active uin %u\n", stOneConfig.iMonsterID, iRet, m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//初始化单只怪物信息
int CBattlefieldObj::InitOneMonsterInfo(int iUnitIndex, const OneCrossMonster& stOneConfig, const SFightPosInfo& stPosInfo)    
{
    //分配CCombatUnitObj
    CCombatUnitObj* pstCombatUnitObj = CreateCombatUnitObj(m_aiPassiveCombatObjIndex[iUnitIndex]);
    if(!pstCombatUnitObj)
    {
        LOGERROR("Failed to create combat unit obj!\n");
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = pstCombatUnitObj->InitMonsterUnitObj(m_aiPassiveCombatObjIndex[iUnitIndex], stOneConfig, stPosInfo, COMBAT_UNIT_TYPE_MONSTER);
    if(iRet)
    {
        LOGERROR("Failed to init monster unit obj, monster id %d, ret %d, uin %u\n", stOneConfig.iMonsterID, iRet, m_uiActiveUin);
        return iRet;
    }

    //释放战斗单位的登场技能
    std::vector<int> vSkillIDs;
    pstCombatUnitObj->GetSkillByType(SKILL_USE_ON_STAGE, vSkillIDs);

    for(unsigned i=0; i<vSkillIDs.size(); ++i)
    {
        iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, m_aiPassiveCombatObjIndex[iUnitIndex], pstCombatUnitObj->GetUnitPosition(), vSkillIDs[i], SKILL_USE_ON_STAGE);
        if(iRet)
        {
            LOGERROR("Failed to cast combat unit skill, unit %d, skill %d, uin %u\n", m_aiPassiveCombatObjIndex[iUnitIndex], vSkillIDs[i], m_uiActiveUin);
            return iRet;
        }
    }

    LOGDEBUG("Success to init one monster info, monster id %d, uin %u\n", stOneConfig.iMonsterID, m_uiActiveUin);

    return T_SERVER_SUCESS;
}

//初始化单个阵营信息
int CBattlefieldObj::InitFightCampInfo(CGameRoleObj& rstRoleObj, bool bIsActive)   
{
    //获取角色战斗单位管理器
    CFightUnitManager& rstFightUnitManager = rstRoleObj.GetFightUnitManager();

    //获取阵型中战斗单位的信息
    std::vector<int> vFormUnitInfo;
    rstFightUnitManager.GetFormFightUnitInfo(vFormUnitInfo);

    int iOnFightNum = (vFormUnitInfo.size()>MAX_FIGHTUNIT_ON_FORM) ? MAX_FIGHTUNIT_ON_FORM : vFormUnitInfo.size();

    const SFightPosInfo* pstCampPosConfig = NULL;
    if(bIsActive)
    {
        pstCampPosConfig = m_astActivePos;
    }
    else
    {
        pstCampPosConfig = m_astPassivePos;
    }

    int iRet = T_SERVER_SUCESS;
    for(int i=0; i<iOnFightNum; ++i)
    {
        if(pstCampPosConfig->iPosX < 0)
        {
            break;
        }

        iRet = InitOneFightUnitInfo(i, rstRoleObj, vFormUnitInfo[i], pstCampPosConfig[i], bIsActive);
        if(iRet)
        {
            LOGERROR("Failed to init one active fight unit info, index %d, ret %d, uin %u\n", vFormUnitInfo[i], iRet, rstRoleObj.GetUin());
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//初始化单个战斗单位信息
int CBattlefieldObj::InitOneFightUnitInfo(int iUnitIndex, CGameRoleObj& rstRoleObj, int iFightUnitID, const SFightPosInfo& stPosInfo, bool bIsActive)
{
    int* paiCombatObjIndex = NULL;
    if(bIsActive)
    {
        paiCombatObjIndex = m_aiActiveCombatObjIndex;
    }
    else
    {
        paiCombatObjIndex = m_aiPassiveCombatObjIndex;
    }

    //创建战场战斗单位对象
    CCombatUnitObj* pstCombatUnitObj = CreateCombatUnitObj(paiCombatObjIndex[iUnitIndex]);
    if(!pstCombatUnitObj)
    {
        LOGERROR("Failed to create fight unit obj!\n");
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //初始化战场战斗单位对象
    int iRet = pstCombatUnitObj->InitRoleUnitObj(paiCombatObjIndex[iUnitIndex], rstRoleObj, iFightUnitID, stPosInfo);
    if(iRet)
    {
        LOGERROR("Failed to init combat unit obj, unit id %d, ret %d, uin %u\n", iFightUnitID, iRet, rstRoleObj.GetUin());
        return iRet;
    }

    //释放战斗单位的登场技能
    std::vector<int> vSkillIDs;
    pstCombatUnitObj->GetSkillByType(SKILL_USE_ON_STAGE, vSkillIDs);

    for(unsigned i=0; i<vSkillIDs.size(); ++i)
    {
        iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, paiCombatObjIndex[iUnitIndex], pstCombatUnitObj->GetUnitPosition(), vSkillIDs[i], SKILL_USE_ON_STAGE);
        if(iRet)
        {
            LOGERROR("Failed to cast combat unit skill, unit %d, skill %d, uin %u\n", paiCombatObjIndex[iUnitIndex], vSkillIDs[i], rstRoleObj.GetUin());
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//打包返回单个战斗对象信息
int CBattlefieldObj::PackOneCombatUnitInfo(int iCamp, int iUnitObjIndex, CombatUnit& stUnitInfo)
{
    if(iUnitObjIndex < 0)
    {
        return T_SERVER_SUCESS;
    }

    CCombatUnitObj* pstCombatUnitObj = GameType<CCombatUnitObj>::Get(iUnitObjIndex);
    if(!pstCombatUnitObj)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    pstCombatUnitObj->PackCombatUnitInfo(iCamp, stUnitInfo);

    return T_SERVER_SUCESS;
}

//初始化回合出手单位信息
void CBattlefieldObj::InitRoundActionUnits()
{
    m_iCanCombatUnitNum = 0;
    m_iActionDoneUnitNum = 0;

    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_aiActiveCombatObjIndex[i] == -1)
        {
            break;
        }

        AddOneUnitToRoundActionList(m_aiActiveCombatObjIndex[i]);
    }

    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_aiPassiveCombatObjIndex[i] == -1)
        {
            break;
        }

        AddOneUnitToRoundActionList(m_aiPassiveCombatObjIndex[i]);
    }

    return;
}

//添加一个战斗单位到回合出手列表
void CBattlefieldObj::AddOneUnitToRoundActionList(int iUnitObjIndex)
{
    CCombatUnitObj* pstCombatUnitObj = GameType<CCombatUnitObj>::Get(iUnitObjIndex);
    if(!pstCombatUnitObj || pstCombatUnitObj->IsCombatUnitDead())
    {
        //找不到或者单位已经死亡，不添加
        return;
    }

    m_aiCanCombatUnit[m_iCanCombatUnitNum++] = iUnitObjIndex;

    return;
}

//获取战斗单位的阵营
int CBattlefieldObj::GetCombatUnitCamp(int iCombatUnitiD)
{
    //先判断是否是主动出手方
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_aiActiveCombatObjIndex[i] == iCombatUnitiD)
        {
            return FIGHT_CAMP_ACTIVE;
        }

        if(m_aiPassiveCombatObjIndex[i] == iCombatUnitiD)
        {
            return FIGHT_CAMP_PASSIVE;
        }
    }

    return FIGHT_CAMP_INVALID;
}

//战斗单位是否同一阵营
bool CBattlefieldObj::IsUnitInSameCamp(CCombatUnitObj& stUnitObj, CCombatUnitObj& stOtherUnitObj)
{
    return (GetCombatUnitCamp(stUnitObj.GetCombatUnitID())==GetCombatUnitCamp(stOtherUnitObj.GetCombatUnitID()));
}

//设置伙伴的战斗AI
int CBattlefieldObj::SetPartnerAI(int iPartnerAIID)
{
    //判断当前战斗单位是否是伙伴
    if(GetActionUnitType() != COMBAT_UNIT_TYPE_PARTNER)
    {
        LOGERROR("Failed to set partner AI, action unit is not partner, uin %u\n", m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    CCombatUnitObj* pstPartnerUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstPartnerUnitObj)
    {
        LOGERROR("Failed to get partner unit obj, uin %u\n", m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    return pstPartnerUnitObj->SetFightAI(iPartnerAIID);
}

//判断当前是否有未出手单位
bool CBattlefieldObj::HasActionCombatUnit()
{
    for(int i=0; i<m_iCanCombatUnitNum; ++i)
    {
        if(m_aiCanCombatUnit[i] >= 0)
        {
            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiCanCombatUnit[i]);
            if(pstUnitObj && !pstUnitObj->IsCombatUnitDead())
            {
                return true;
            }
        }
    }

    return false;
}

//todo jasonxiong5 这边要用新的先攻重新开发
//设置当前出手的战斗单位
void CBattlefieldObj::SetActionCombatUnit()
{
    if(m_iActionUnit >= 0)
    {
        LOGERROR("Failed to set action combat unit, current action unit %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return;
    }

    //遍历当前行动数组，找出当前出手的战斗单位
    int iActionIndex = -1;
    for(int i=0; i<m_iCanCombatUnitNum; ++i)
    {
        if(m_aiCanCombatUnit[i] < 0)
        {
            //已经行动过
            continue;
        }

        CCombatUnitObj* pstUnitObj = GameType<CCombatUnitObj>::Get(m_aiCanCombatUnit[i]);
        if(!pstUnitObj)
        {
            LOGERROR("Failed to get combat unit obj, uin %u\n", m_uiActiveUin);
            return;
        }

        if(pstUnitObj->IsCombatUnitDead())
        {
            m_aiCanCombatUnit[i] = -1;

            continue;
        }

        if(iActionIndex < 0)
        {
            iActionIndex = i;

            continue;
        }

        CCombatUnitObj* pstActionUnitObj = GameType<CCombatUnitObj>::Get(m_aiCanCombatUnit[iActionIndex]);
        if(!pstActionUnitObj)
        {
            LOGERROR("Failed to get combat unit obj, uin %u\n", m_uiActiveUin);
            return;
        }

        if(pstActionUnitObj->IsCombatUnitDead())
        {
            //单位死亡，不能行动
            m_aiCanCombatUnit[i] = -1;

            continue;
        }
    }

    if(iActionIndex >= 0)
    {
        m_iActionUnit = m_aiCanCombatUnit[iActionIndex];

        m_aiCanCombatUnit[iActionIndex] = -1;
    }

    return;
}

//清除当前出手的战斗单位
void CBattlefieldObj::ClearActionCombatUnit()
{
    //设置当前行动单位到已出手中
    m_aiActionDoneUnit[m_iActionDoneUnitNum++] = m_iActionUnit;
    m_iActionUnit = -1;

    return;
}

//获取当前出手的战斗单位的类型
int CBattlefieldObj::GetActionUnitType()
{
    CCombatUnitObj* pstActionUnitObj = GameType<CCombatUnitObj>::Get(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get action unit obj, index %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return COMBAT_UNIT_TYPE_INVALID;
    }

    return pstActionUnitObj->GetCombatUnitType();
}

//获取当前出手单位的ID
int CBattlefieldObj::GetActionUnitID()
{
    return m_iActionUnit;
}

//主角战斗单位当前的状态
int CBattlefieldObj::GetActionUnitStatus()
{
    return m_iRoleCombatUnitStatus;
}

void CBattlefieldObj::SetActionUnitStatus(int iStatus)
{
    m_iRoleCombatUnitStatus = iStatus;
}

//判断是否能够结束战斗
bool CBattlefieldObj::CanEndCombat()
{
    return (m_iCombatResult != COMBAT_RESULT_INVALID);
}

//推送客户端主角操作的指令
void CBattlefieldObj::NotifyRoleUnitDirective(int iDirective)
{
    static GameProtocolMsg stUnitDirectiveNotify;

    CZoneMsgHelper::GenerateMsgHead(stUnitDirectiveNotify, MSGID_ZONE_UNITDIRECTIVE_NOTIFY);

    Zone_UnitDirective_Notify* pstNotify = stUnitDirectiveNotify.mutable_m_stmsgbody()->mutable_m_stzone_unitdirective_notify();
    pstNotify->set_idirective((UnitDirectiveType)iDirective);
    pstNotify->set_icombatunitid(m_iActionUnit);

    //推送通知
    SendNotifyToBattlefield(stUnitDirectiveNotify);

    return;
}

//开始战斗单位的移动
int CBattlefieldObj::DoCombatUnitMove(const UnitPath& stPath)
{
    if(stPath.stposes_size() == 0)
    {
        //战斗单位不移动
        return T_SERVER_SUCESS;
    }

    //获取当前行动的战斗单位Obj
    CCombatUnitObj* pstActionUnitObj = GameType<CCombatUnitObj>::Get(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get action unit obj, index %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //判断是否可以移动
    if(pstActionUnitObj->GetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTMOVE))
    {
        //不能移动
        pstActionUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTMOVE, false);

        return T_SERVER_SUCESS;
    }

    //获取战斗单位最大移动距离
    int iMaxWalkPosNum = GetMaxMoveDistance(*pstActionUnitObj);

    //检查步长
    if(stPath.stposes_size()>iMaxWalkPosNum || stPath.stposes_size()>MAX_POSITION_NUMBER_IN_PATH)
    {
        LOGERROR("Failed to do combat unit move, pos num reach max %d, unit index %d, uin %u\n", iMaxWalkPosNum, m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //封装新的路径
    TUNITPATH stUnitPath;
    for(int i=0; i<stPath.stposes_size(); ++i)
    {
        stUnitPath.astPos[i].iPosX = stPath.stposes(i).iposx();
        stUnitPath.astPos[i].iPosY = stPath.stposes(i).iposy();

        ++stUnitPath.iPosNum;
    }

    //检查路径是否可连续行走
    if(!m_pstBattlePathManager->CanContinueWalk(pstActionUnitObj->GetUnitPosition(), stUnitPath))
    {
        LOGERROR("Failed to do combat unit move, path can not walk, uin %u\n", m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //检查路径上的点是否可行走
    for(int i=0; i<stUnitPath.iPosNum; ++i)
    {
        if(!IsPosCanWalk(stUnitPath.astPos[i], m_iActionUnit))
        {
            LOGERROR("Failed to do combat unit move, path can not walk, uin %u\n", m_uiActiveUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }
    }

    //处理路径上的机关
    int iTargetIndex = stUnitPath.iPosNum-1;
    for(int i=0; i<stUnitPath.iPosNum; ++i)
    {
        CCombatTrapObj* pstTrapObj = GetTrapByPos(stUnitPath.astPos[i]);
        if(!pstTrapObj)
        {
            //没有机关
            continue;
        }

        //尝试触发停止触发类型的机关
        if(pstTrapObj->GetConfig()->iTriggerType == TRAP_TRIGGER_MOVEON)
        {
            TriggerCombatTrap(m_iActionUnit, pstTrapObj->GetObjectID(), TRAP_TRIGGER_MOVEON);
        }

        //判断是否打断行动
        if(pstTrapObj->GetConfig()->iInterruptType == TRAP_INTERRUPT_MOVE)
        {
            //打断移动
            iTargetIndex = i;

            break;
        }
    }

    return DoCombatUnitMove(iTargetIndex+1, stUnitPath.astPos);
}

//开始战斗单位的移动,AI移动只有目标点
int CBattlefieldObj::DoCombatUnitMove(int iMoveDistance, const TUNITPOSITION* pstMovePath)
{
    //获取当前行动的战斗单位Obj
    CCombatUnitObj* pstActionUnitObj = GameType<CCombatUnitObj>::Get(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get action unit obj, index %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    if(pstActionUnitObj->GetUnitPosition().iPosX == pstMovePath[iMoveDistance-1].iPosX
       && pstActionUnitObj->GetUnitPosition().iPosY == pstMovePath[iMoveDistance-1].iPosY)
    {
        //目标位置就是当前位置
        return T_SERVER_SUCESS;
    }

    //设置最新的方向
    CCombatUtility::UpdateUnitDirection(*pstActionUnitObj, pstMovePath[iMoveDistance-1]);

    //设置最新的位置
    pstActionUnitObj->SetUnitPosition(pstMovePath[iMoveDistance-1]);

    //推送移动的通知
    static GameProtocolMsg stCombatUnitMoveNotify;

    CZoneMsgHelper::GenerateMsgHead(stCombatUnitMoveNotify, MSGID_ZONE_COMBATMOVE_NOTIFY);

    Zone_CombatMove_Notify* pstNotify = stCombatUnitMoveNotify.mutable_m_stmsgbody()->mutable_m_stzone_combatmove_notify();
    pstNotify->set_iunitid(m_iActionUnit);
    pstNotify->set_idirection(pstActionUnitObj->GetUnitDirection());
    pstNotify->set_imovetype(COMBAT_MOVE_NORMAL);

    //封装路径返回给客户端
    for(int i=0; i<iMoveDistance; ++i)
    {
        UnitPosition* pstPos = pstNotify->mutable_stpath()->add_stposes();
        pstPos->set_iposx(pstMovePath[i].iPosX);
        pstPos->set_iposy(pstMovePath[i].iPosY);
    }

    SendNotifyToBattlefield(stCombatUnitMoveNotify);

    //生效移动触发的BUFF,移动N格触发N次
    for(int i=0; i<iMoveDistance; ++i)
    {
        int iRet = DoBuffEffectByType(BUFF_TRIGGER_MOVE, m_iActionUnit, m_iActionUnit);
        if(iRet)
        {
            LOGERROR("Failed to trigger move buff, ret %d, unit id %d, uin %u\n", iRet, m_iActionUnit, m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//战斗单位使用技能
int CBattlefieldObj::DoCombatCastSkill(int iSkillID, const TUNITPOSITION& stTargetPos, int iSkillUseType)
{
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get combat unit obj, unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //尝试处理机动模式
    if(pstActionUnitObj->GetMotorDistance() >= 0)
    {
        if(pstActionUnitObj->GetMotorLeftLen() <= pstActionUnitObj->GetMotorDistance())
        {
            //已满足取消机动模式条件，取消机动模式
            pstActionUnitObj->SetMotorMode(TUNITPOSITION(-1,0), -1);
        }

        return T_SERVER_SUCESS;
    }

    if(pstActionUnitObj->GetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTACTION))
    {
        //攻击方当前不能行动
        pstActionUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTACTION, false);

        return T_SERVER_SUCESS;
    }

    CCombatUnitObj* pstTargetUnitObj = GetCombatUnitByPos(stTargetPos);
    int iRet = T_SERVER_SUCESS;
    if(!pstTargetUnitObj)
    {
        //目标位置不是战斗单位对空位放技能
        iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, m_iActionUnit, stTargetPos, iSkillID, iSkillUseType);
        if(iRet)
        {
            LOGERROR("Failed to cast combat skill, ret %d, target pos %d:%d, skill %d, uin %u\n", iRet, stTargetPos.iPosX, stTargetPos.iPosY, iSkillID, m_uiActiveUin);
            return iRet;
        }
    }
    else
    {
        int iTargetUnitID = pstTargetUnitObj->GetCombatUnitID();

        //目标位置有战斗单位
        int iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, m_iActionUnit, pstTargetUnitObj->GetUnitPosition(), iSkillID, iSkillUseType);
        if(iRet)
        {
            LOGERROR("Failed to cast combat skill, ret %d, target %d, skill %d, uin %u\n", iRet, iTargetUnitID, iSkillID, m_uiActiveUin);
            return iRet;
        }
    
        //如果目标已经死亡，不进行下面的处理
        if(IsCombatUnitDead(iTargetUnitID))
        {
            return T_SERVER_SUCESS;
        }
    
        //技能的主要目标是否产生闪避
        int iChiefTargetDodge = CCombatSkill::m_iChiefTargetDodge;
    
        //处理攻击单位技能的连击
        iRet = DoComboAttack(*pstTargetUnitObj, iSkillID);
    
        //技能攻击处理完成,不是同一个阵营并且是普攻,则处理队友的夹击
        if(!IsUnitInSameCamp(*pstActionUnitObj,*pstTargetUnitObj) && pstActionUnitObj->GetNormalSkillID()==iSkillID)
        {
            iRet = DoPincerAttack(*pstTargetUnitObj);
            if(iRet)
            {
                LOGERROR("Failed to process pincer attack, unit id %d, target %d, uin %u\n", m_iActionUnit, iTargetUnitID, m_uiActiveUin);
                return iRet;
            }
        }
    
        //如果主要目标产生闪避，并且不是对自己,则尝试触发反击
        if(iChiefTargetDodge==SKILL_HIT_CHIEFDODGE && pstTargetUnitObj->GetCombatUnitID()!=m_iActionUnit)
        {
            iRet = DoCounterAttack(*pstTargetUnitObj, *pstActionUnitObj);
            if(iRet)
            {
                LOGERROR("Failed to do counterattack, unit id %d, uin %u\n", iTargetUnitID, m_uiActiveUin);
                return iRet;
            }
        }
    
        //处理攻击者技能的伙伴合击
        iRet = DoJointAttack(*pstTargetUnitObj, iSkillID);
        if(iRet)
        {
            LOGERROR("Failed to process joint attack, unit id %d, skill id %d, uin %u\n", m_iActionUnit, iSkillID, m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//战斗单位使用道具,增加BUFF或者放置机关
int CBattlefieldObj::DoCombatUseItem(int iItemID, const TUNITPOSITION& stTargetPos)
{
    //todo jasonxiong5 道具使用这个要重新开发逻辑
    /*
    //推送开始使用道具的消息
    static GameProtocolMsg stActionBeginMsg;
    CZoneMsgHelper::GenerateMsgHead(stActionBeginMsg, MSGID_ZONE_BEGINCOMBATACTION_NOTIFY);

    SendNotifyToBattlefield(stActionBeginMsg);

    //读取道具配置
    const SPropItemConfig* pstPropConfig = PropItemCfgMgr().Get(iItemID);
    if(!pstPropConfig)
    {
        LOGERROR("Failed to get prop item config, invalid prop id %d\n", iItemID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //获取使用道具战斗单位Obj
    CCombatUnitObj* pstCastUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstCastUnitObj)
    {
        LOGERROR("Failed to get cast fight unit obj, invalid unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //判断是否能够使用
    int iDistance = CCombatUtility::GetAttackDistance(pstCastUnitObj->GetUnitPosition(), stTargetPos, pstPropConfig->iRangeID);
    if(iDistance == 0)
    {
        LOGERROR("Failed to use item on target, invalid pos, item id %d, uin %u\n", iItemID, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //扣除玩家道具
    CGameRoleObj* pstRoleObj = NULL;
    int iCastUnitCamp = GetCombatUnitCamp(m_iActionUnit);
    switch(iCastUnitCamp)
    {
        case FIGHT_CAMP_ACTIVE:
            {
                pstRoleObj = CUnitUtility::GetRoleByUin(m_uiActiveUin);
            }
            break;

        case FIGHT_CAMP_PASSIVE:
            {
                pstRoleObj = CUnitUtility::GetRoleByUin(m_uiPassiveUin);
            }
            break;

        default:
            {
                ;
            }
            break;
    }
    
    if(!pstRoleObj)
    {
        LOGERROR("Failed to get game role obj, active %u, passive %u\n", m_uiActiveUin, m_uiPassiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = pstRoleObj->GetRepThingsManager().DeleteRepItemByID(iItemID, ITEM_DELETE_BATTLE);
    if(iRet)
    {
        LOGERROR("Failed to delete rep item, item id %d, uin %u\n", iItemID, pstRoleObj->GetUin());
        return iRet;
    }

    switch(pstPropConfig->iPropType)
    {
        case PROP_ITEM_YAOPIN:
            {
                //药品

                //判断放置的单位是否有需求的技能
                if(pstPropConfig->iTrapNeedSkill!=0 && !pstCastUnitObj->HasFightSkill(pstPropConfig->iTrapNeedSkill))
                {
                    //没有需求的技能
                    LOGERROR("Failed to use prop item, no need skill, skill id %d, item id %d, uin %u\n", pstPropConfig->iTrapNeedSkill, iItemID, m_uiActiveUin);
                    return T_ZONE_SYSTEM_PARA_ERR;
                }

                static GameProtocolMsg stCombatAddBuffNotify;
                CZoneMsgHelper::GenerateMsgHead(stCombatAddBuffNotify, MSGID_ZONE_COMBATADDBUFF_NOTIFY);
            
                Zone_CombatAddBuff_Notify* pstNotify = stCombatAddBuffNotify.mutable_m_stmsgbody()->mutable_m_stzone_combataddbuff_notify();
            
                CCombatUnitObj* pstTargetUnitObj = GetCombatUnitByPos(stTargetPos);
                if(!pstTargetUnitObj)
                {
                    LOGERROR("Failed to use prop item, item id %d, target pos %d:%d, uin %u\n", iItemID, stTargetPos.iPosX, stTargetPos.iPosY, m_uiActiveUin);
                    return T_ZONE_SYSTEM_PARA_ERR;
                }

                //生效道具效果
                iRet = pstTargetUnitObj->AddUnitBuff(m_uiActiveUin, m_iCrossID, pstPropConfig->iBuffID, m_iActionUnit, *pstNotify);
                if(iRet)
                {
                    LOGERROR("Failed to add combat unit buff, ret %d, item id %d, buff id %d, uin %u\n", iRet, iItemID, pstPropConfig->iBuffID, pstRoleObj->GetUin());
                    return iRet;
                }
            
                SendNotifyToBattlefield(stCombatAddBuffNotify);
            }
            break;

        case PROP_ITEM_JIGUAN:
            {
                //判断放置的单位是否有需求的技能
                if(pstPropConfig->iTrapNeedSkill!=0 && !pstCastUnitObj->HasFightSkill(pstPropConfig->iTrapNeedSkill))
                {
                    //没有需求的技能
                    LOGERROR("Failed to add combat trap, no need skill, skill id %d, item id %d, uin %u\n", pstPropConfig->iTrapNeedSkill, iItemID, m_uiActiveUin);
                    return T_ZONE_SYSTEM_PARA_ERR;
                }

                //在战场上放置机关
                int iTrapObjID = AddCombatTrap(pstPropConfig->iAddTrapID, GetCombatUnitCamp(m_iActionUnit), stTargetPos, pstPropConfig->iTrapDirection);
                if(iTrapObjID < 0)
                {
                    LOGERROR("Failed to add combat trap, trap obj id %d, item id %d, uin %u\n", iTrapObjID, iItemID, m_uiActiveUin);
                    return T_ZONE_SYSTEM_PARA_ERR;
                }
            }
            break;

        default:
            {
                LOGERROR("Failed to use combat prop item, prop id %d, uin %u\n", iItemID, m_uiActiveUin);
                return T_ZONE_SYSTEM_INVALID_CFG;
            }
            break;
    }

    //推送ActionNotify消息
    static GameProtocolMsg stActionNotifyMsg;
    CZoneMsgHelper::GenerateMsgHead(stActionNotifyMsg, MSGID_ZONE_COMBATACTION_NOTIFY);

    Zone_CombatAction_Notify* pstActionNotify = stActionNotifyMsg.mutable_m_stmsgbody()->mutable_m_stzone_combataction_notify();
    pstActionNotify->set_iactionunitid(m_iActionUnit);

    pstActionNotify->mutable_sttargetpos()->set_iposx(stTargetPos.iPosX);
    pstActionNotify->mutable_sttargetpos()->set_iposy(stTargetPos.iPosY);

    pstActionNotify->set_etype(COMBAT_ACTION_USEITEM);
    pstActionNotify->set_iitemid(iItemID);

    SendNotifyToBattlefield(stActionNotifyMsg);

    //处理使用物品脚本接口
    CModuleHelper::GetStoryFramework()->DoUseItemScript(m_uiActiveUin, m_iCrossID, iItemID, m_iActionUnit, stTargetPos.iPosX, stTargetPos.iPosY);
    */


    return T_SERVER_SUCESS;
}

//处理技能的连击
int CBattlefieldObj::DoComboAttack(CCombatUnitObj& stTargetUnitObj, int iSkillID)
{
    const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(iSkillID);
    if(!pstSkillConfig)
    {
        LOGERROR("Failed to get fight skill config, invalid id %d\n", iSkillID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get action unit obj, unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iComboAttr = pstActionUnitObj->GetFightAttr(FIGHT_ATTR_LIANJI);
    int iRet = T_SERVER_SUCESS;
    for(int i=0; i<pstSkillConfig->iComboNum; ++i)
    {
        //判断是否会出现连击
        if(CRandomCalculator::GetRandomNumberInRange(100) >= iComboAttr)
        {
            //没有随机出连击
            break;
        }

        //处理技能的连击
        iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, m_iActionUnit, stTargetUnitObj.GetUnitPosition(), iSkillID, SKILL_USE_LIANJI);
        if(iRet)
        {
            LOGERROR("Failed to cast combat skill, unit id %d, target id %d, skill id %d, uin %u\n", m_iActionUnit, stTargetUnitObj.GetCombatUnitID(), iSkillID, m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//处理技能的夹击
int CBattlefieldObj::DoPincerAttack(CCombatUnitObj& stTargetUnitObj)
{
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get combat unit obj, unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    TUNITPOSITION& stActionPos = pstActionUnitObj->GetUnitPosition();
    TUNITPOSITION& stTargetPos = stTargetUnitObj.GetUnitPosition();

    //获取镜像位置的坐标
    TUNITPOSITION stMirrorPos;
    stMirrorPos.iPosX = stTargetPos.iPosX*2-stActionPos.iPosX;
    stMirrorPos.iPosY = stTargetPos.iPosY*2-stActionPos.iPosY;

    //获取镜像位置的单位
    CCombatUnitObj* pstMirrorUnitObj = GetCombatUnitByPos(stMirrorPos);
    if(!pstMirrorUnitObj || !IsUnitInSameCamp(*pstActionUnitObj,*pstMirrorUnitObj))
    {
        //镜像位置没有单位或不是同阵营
        return T_SERVER_SUCESS;
    }

    //处理普攻攻击
    int iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, pstMirrorUnitObj->GetCombatUnitID(), stTargetUnitObj.GetUnitPosition(), pstMirrorUnitObj->GetNormalSkillID(), SKILL_USE_JIAJI);
    if(iRet)
    {
        LOGERROR("Failed to process pincer attack, unit id %d, skill %d, uin %u\n", pstMirrorUnitObj->GetCombatUnitID(), pstMirrorUnitObj->GetNormalSkillID(), m_uiActiveUin);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

//处理技能的反击
int CBattlefieldObj::DoCounterAttack(CCombatUnitObj& stCounterAtkUnitObj, CCombatUnitObj& stTargetObj)
{
    //判断是否触发反击
    if(CRandomCalculator::GetRandomNumberInRange(100) >= stCounterAtkUnitObj.GetFightAttr(FIGHT_ATTR_FANJI))
    {
        //没有触发反击
        return T_SERVER_SUCESS;
    }

    //触发反击技能
    int iSkillID = stCounterAtkUnitObj.GetCounterAtkSkill();
    int iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, stCounterAtkUnitObj.GetCombatUnitID(), stTargetObj.GetUnitPosition(), iSkillID, SKILL_USE_FANJI);
    if(iRet)
    {
        LOGERROR("Failed to process counteratk skill, uint id %d, skill id %d, uin %u\n", stCounterAtkUnitObj.GetCombatUnitID(), iSkillID, m_uiActiveUin);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

//处理伙伴技能的合击
int CBattlefieldObj::DoJointAttack(CCombatUnitObj& stTargetUnitObj, int iSkillID)
{
    //读取攻击技能的配置
    const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(iSkillID);
    if(!pstSkillConfig)
    {
        LOGERROR("Failed to get fight skill config, invalid skill id %d\n", iSkillID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //该技能是否有合击
    if(pstSkillConfig->iJointAtkID == 0)
    {
        return T_SERVER_SUCESS;
    }

    //读取合击技能的配置
    const SJointAttackConfig* pstJointAtkConfig = JointAtkCfgMgr().GetConfig(pstSkillConfig->iJointAtkID);
    if(!pstJointAtkConfig)
    {
        LOGERROR("Failed to get joint attack config, invalid joint atk id %d\n", pstSkillConfig->iJointAtkID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get unit obj, unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //遍历攻击单位周边的九宫格
    int iRet = T_SERVER_SUCESS;
    TUNITPOSITION& stActionPos = pstActionUnitObj->GetUnitPosition();
    for(int iDeltaX = -1; iDeltaX <= 1; ++iDeltaX)
    {
        for(int iDeltaY = -1; iDeltaY <= 1; ++iDeltaY)
        {
            TUNITPOSITION stPos(stActionPos.iPosX+iDeltaX, stActionPos.iPosY+iDeltaY);
            CCombatUnitObj* pstJointUnitObj = GetCombatUnitByPos(stPos);
            if(!pstJointUnitObj || !IsUnitInSameCamp(*pstActionUnitObj, *pstJointUnitObj))
            {
                //没有单位或者不是攻方的队友
                continue;
            }

            //判断单位技能是否满足合击要求
            for(int i=0; i<pstJointAtkConfig->iNeedSkillNum; ++i)
            {
                if(pstJointAtkConfig->aiNeedSkills[i] == 0)
                {
                    continue;
                }

                if(!pstJointUnitObj->HasFightSkill(pstJointAtkConfig->aiNeedSkills[i]))
                {
                    //没有这个合击技能
                    continue;
                }

                //找到，合击伙伴使用合击技能
                iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, pstJointUnitObj->GetCombatUnitID(), stTargetUnitObj.GetUnitPosition(), pstJointAtkConfig->aiNeedSkills[i], SKILL_USE_HEJI);
                if(iRet)
                {
                    LOGERROR("Failed to process joint attack skill, unit id %d, skill id %d, uin %u\n", pstJointUnitObj->GetCombatUnitID(), pstJointAtkConfig->aiNeedSkills[i], m_uiActiveUin);
                }
            }
        }
    }

    return T_SERVER_SUCESS;
}

//判断战斗单位是否死亡
bool CBattlefieldObj::IsCombatUnitDead(int iUnitID)
{
    CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(iUnitID);
    if(!pstUnitObj)
    {
        return true;
    }

    return pstUnitObj->IsCombatUnitDead();
}

//获取阻挡类型的机关
CCombatTrapObj* CBattlefieldObj::GetBlockTrapByPos(const TUNITPOSITION& stPos)
{
    CCombatTrapObj* pstTrapObj = GetTrapByPos(stPos);
    if(!pstTrapObj || pstTrapObj->GetConfig()->iTrapType!=COMBAT_TRAP_ORGAN)
    {
        return NULL;
    }

    return pstTrapObj;
}

//根据机关的位置获取机关
CCombatTrapObj* CBattlefieldObj::GetTrapByPos(const TUNITPOSITION& stPos)
{
    for(int i=0; i<m_iTrapNum; ++i)
    {
        CCombatTrapObj* pstTrapObj = GameType<CCombatTrapObj>::Get(m_aiCombatTrapIndex[i]);
        if(!pstTrapObj)
        {
            continue;
        }

        if(pstTrapObj->GetPosition().iPosX == stPos.iPosX && 
           pstTrapObj->GetPosition().iPosY == stPos.iPosY)
        {
            return pstTrapObj;
        }
    }

    return NULL;
}

int CBattlefieldObj::GetTrapIndexByPos(const TUNITPOSITION& stPos)
{
    for(int i=0; i<m_iTrapNum; ++i)
    {
        CCombatTrapObj* pstTrapObj = GameType<CCombatTrapObj>::Get(m_aiCombatTrapIndex[i]);
        if(!pstTrapObj)
        {
            continue;
        }

        if(pstTrapObj->GetPosition().iPosX == stPos.iPosX && 
           pstTrapObj->GetPosition().iPosY == stPos.iPosY)
        {
            return m_aiCombatTrapIndex[i];
        }
    }

    return -1;
}

//战场状态，供脚本使用
bool CBattlefieldObj::GetCombatStatus(int iStatusType)
{
    return ((m_ucCombatStatus>>iStatusType) & 0x01);
}

void CBattlefieldObj::SetCombatStatus(int iStatusType, bool bIsSet)
{
    if(bIsSet)
    {
        m_ucCombatStatus = (m_ucCombatStatus | (0x01<<iStatusType));
    }
    else
    {
        m_ucCombatStatus = (m_ucCombatStatus & ~(0x01<<iStatusType));
    }

    return;
}

//获取地图上有效站位位置
void CBattlefieldObj::GetBattlefieldValidPos(const SBattleCrossConfig& stCrossConfig)
{
    //初始化主动方站位位置
    int iValidPosNum = 0;
    for(int i=0; i<stCrossConfig.iSelfUnitNum; ++i)
    {
        if(stCrossConfig.astFightPosInfo[i].iPosX < 0)
        {
            continue;
        }

        m_astActivePos[iValidPosNum].iPosX = stCrossConfig.astFightPosInfo[i].iPosX;
        m_astActivePos[iValidPosNum].iPosY = stCrossConfig.astFightPosInfo[i].iPosY;
        m_astActivePos[iValidPosNum].iDirection = stCrossConfig.astFightPosInfo[i].iDirection;
        ++iValidPosNum;
    }

    //初始化被动方站位位置
    iValidPosNum = 0;
    for(int i=0; i<stCrossConfig.iEnemyUnitNum; ++i)
    {
        if(stCrossConfig.astEnemyFightPosInfo[i].iPosX < 0)
        {
            continue;
        }

        m_astPassivePos[iValidPosNum].iPosX = stCrossConfig.astEnemyFightPosInfo[i].iPosX;
        m_astPassivePos[iValidPosNum].iPosY = stCrossConfig.astEnemyFightPosInfo[i].iPosY;
        m_astPassivePos[iValidPosNum].iDirection = stCrossConfig.astEnemyFightPosInfo[i].iDirection;
        ++iValidPosNum;
    }

    return;
}

//创建CCombatUnitObj,参数返回ObjIndex
CCombatUnitObj* CBattlefieldObj::CreateCombatUnitObj(int& iCombatUnitObjIndex)
{
    iCombatUnitObjIndex = GameType<CCombatUnitObj>::Create();
    if(iCombatUnitObjIndex < 0)
    {
        LOGERROR("Failed to create combat unit obj!\n");
        return NULL;
    }

    CCombatUnitObj* pstCombatUnitObj = GameType<CCombatUnitObj>::Get(iCombatUnitObjIndex);
    if(!pstCombatUnitObj)
    {
        LOGERROR("Failed to get combat unit obj!\n");
        return NULL;
    }

    return pstCombatUnitObj;
}

//删除一个战场单位
void CBattlefieldObj::ClearOneCombatUnitObj(int iCombatUnitID)
{
    CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(iCombatUnitID);
    if(!pstUnitObj)
    {
        return;
    }

    //清理BUFF
    pstUnitObj->ClearUnitBuff();

    GameType<CCombatUnitObj>::Del(iCombatUnitID);

    return;
}
