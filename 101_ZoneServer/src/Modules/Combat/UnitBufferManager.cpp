
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "ZoneObjectHelper.hpp"
#include "UnitBufferObj.hpp"
#include "ModuleShortcut.hpp"
#include "CombatUnitObj.hpp"
#include "CombatUtility.hpp"
#include "ModuleHelper.hpp"

#include "UnitBufferManager.hpp"

using namespace ServerLib;

CUnitBufferManager::CUnitBufferManager()
{
    Initialize();
}

CUnitBufferManager::~CUnitBufferManager()
{

}

int CUnitBufferManager::Initialize()
{
    m_iUnitID = -1;

    for(int i=0; i<MAX_FIGHT_UNIT_BUFF_NUM; ++i)
    {
        m_aiUnitBuffObjIndex[i] = -1;
    }

    m_iUnitBuffObjNum = 0;

    return 0;
}

//拥有的战斗单位的UnitID
int CUnitBufferManager::GetOwnerUnitID()
{
    return m_iUnitID;
}

void CUnitBufferManager::SetOwnerUnitID(int iUnitID)
{
    m_iUnitID = iUnitID;
}

//增加一个Buff
int CUnitBufferManager::AddUnitBuff(unsigned int uin, int iCrossID, int iBuffID, int iCastUnitID, Zone_CombatAddBuff_Notify& stNotify)
{
    if(iBuffID == 0)
    {
        //没有新加BUFF
        return T_SERVER_SUCESS;
    }

    //读取Buff配置
    const SFightBuffConfig* pstBuffConfig = FightBuffCfgMgr().GetConfig(iBuffID);
    if(!pstBuffConfig)
    {
        LOGERROR("Failed to get fight buff config, invalid buff id %d\n", iBuffID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //设置通知的内容
    stNotify.set_icastunitid(iCastUnitID);

    //增加一个目标
    OneAddBuff* pstOneAddBuff = stNotify.add_staddbuffs();
    pstOneAddBuff->set_itargetunitid(m_iUnitID);

    //增加一个返回的BUFF信息
    AddBuffInfo* pstOneBuffNotify = pstOneAddBuff->add_stbuffinfo();

    //如果有叠加类型，处理叠加
    if(pstBuffConfig->iBuffType != BUFF_OVERLYING_ALL)
    {
        //有叠加类型，先判断Buff叠加
        bool bCanOverlying = ProcessOverlyingBuff(pstBuffConfig->iBuffType, pstBuffConfig->iBuffLevel, *(pstOneBuffNotify->mutable_stremovebuff()));
        if(!bCanOverlying)
        {
            //新的Buff不能叠加
            return T_SERVER_SUCESS;
        }
    }

    pstOneBuffNotify->set_ibuffid(iBuffID);

    if(pstBuffConfig->iEffectRound != 0)
    {
        //有回合数限制的BUFF，需要记录BUFF信息

        //创建新的BuffObj
        CUnitBufferObj* pstNewBuffObj = CreateNewBufferObj();
        if(!pstNewBuffObj)
        {
            LOGERROR("Failed to create new buffer obj!\n");
            return T_ZONE_SYSTEM_PARA_ERR;
        }
    
        //初始化新的Buff
        pstNewBuffObj->InitUnitBuff(iBuffID, iCastUnitID, pstBuffConfig->iEffectRound);
    }

    int iRet = T_SERVER_SUCESS;
    //是否即时生效的Buff
    if(pstBuffConfig->iTriggerType == BUFF_TRIGGER_IMMEDIATELY)
    {
        //即时生效的Buff，处理Buff效果
        iRet = DoOneBuffEffect(uin, iCrossID, *pstBuffConfig, *(pstOneBuffNotify->mutable_steffects()), iCastUnitID);
        if(iRet)
        {
            LOGERROR("Failed to process one buff effect, ret %d\n", iRet);
            return iRet;
        }

        //todo jasonxiong5 重新开发脚本系统
        //调用BUFF生效触发的脚本
        //CModuleHelper::GetStoryFramework()->DoBuffScript(uin, iCrossID, pstBuffConfig->iConfigID, m_iUnitID, iCastUnitID, iCastUnitID);
    }

    //判断是否有同步的Buff，如果有则添加
    if(pstBuffConfig->iAddBuffID != 0)
    {
        iRet = AddUnitBuff(uin, iCrossID, pstBuffConfig->iAddBuffID, iCastUnitID, stNotify);
        if(iRet)
        {
            LOGERROR("Failed to add new unit buff, ret %d\n", iRet);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//按类型生效Buff
int CUnitBufferManager::DoUnitBuffEffect(unsigned int uin, int iCrossID, int iTriggerType, int iTriggerUnitID, Zone_DoBuffEffect_Notify& stNotify, int* pDamageNum)
{
    stNotify.set_itriggerunitid(iTriggerUnitID);

    for(int i=0; i<m_iUnitBuffObjNum; ++i)
    {
        CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj)
        {
            LOGERROR("Failed to get unit buff obj, invalid index %d\n", m_aiUnitBuffObjIndex[i]);

            continue;
        }

        //判断是否还剩余回合数
        if(pstBuffObj->GetRemainRound() <= 0)
        {
            continue;
        }

        //读取Buff配置
        const SFightBuffConfig* pstBuffConfig = FightBuffCfgMgr().GetConfig(pstBuffObj->GetUnitBuffID());
        if(!pstBuffConfig)
        {
            LOGERROR("Failed to get fight buff config, invalid buff id %d\n", pstBuffObj->GetUnitBuffID());
            return T_ZONE_SYSTEM_INVALID_CFG;
        }

        //判断触发类型是否一致
        if(pstBuffConfig->iTriggerType != iTriggerType)
        {
            continue;
        }

        //生效单个Buff
        int iRet = DoOneBuffEffect(uin, iCrossID, *pstBuffConfig, *(stNotify.add_steffects()), pstBuffObj->GetCastUnitID(), iTriggerUnitID, pDamageNum);
        if(iRet)
        {
            LOGERROR("Failed to process one buff effect, ret %d, buff id %d\n", iRet, pstBuffObj->GetUnitBuffID());
            return iRet;
        }

        //todo jasonxiong5 重新开发脚本
        //调用BUFF生效触发的脚本
        //CModuleHelper::GetStoryFramework()->DoBuffScript(uin, iCrossID, pstBuffObj->GetUnitBuffID(), m_iUnitID, pstBuffObj->GetCastUnitID(), iTriggerUnitID);
    }

    return T_SERVER_SUCESS;
}

//减少所有BUFF的回合数
int CUnitBufferManager::DecreaseAllBuffRound(Zone_RemoveBuff_Notify& stNotify)
{
    int iRet = T_SERVER_SUCESS;

    stNotify.set_itargetunitid(m_iUnitID);

    //遍历所有的BUFF
    for(int i=0; i<m_iUnitBuffObjNum;)
    {
        CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj)
        {
            ++i;
            continue;
        }
        
        if(pstBuffObj->GetRemainRound() > 1)
        {
            //BUFF还有效，减少回合数
            pstBuffObj->SetRemainRound(pstBuffObj->GetRemainRound()-1);

            ++i;
            continue;
        }

        //BUFF失效，移除BUFF
        iRet = DelUnitBuffByIndex(m_aiUnitBuffObjIndex[i], *stNotify.add_steffects());
        if(iRet)
        {
            LOGERROR("Failed to delete unit buff, ret %d\n", iRet);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//获取所有BUFF的配置ID
void CUnitBufferManager::GetUnitBuffID(std::vector<int>& vBuffIDs)
{
    for(int i=0; i<m_iUnitBuffObjNum; ++i)
    {
        CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj)
        {
            continue;
        }

        vBuffIDs.push_back(pstBuffObj->GetUnitBuffID());
    }

    return;
}

//是否有BUFFID同类型的BUFF
bool CUnitBufferManager::HasBuffOfSameType(int iBuffID)
{
    //读取BUFF配置
    const SFightBuffConfig* pstNewBuffConfig = FightBuffCfgMgr().GetConfig(iBuffID);
    if(!pstNewBuffConfig)
    {
        return false;
    }

    for(int i=0; i<m_iUnitBuffObjNum; ++i)
    {
        if(m_aiUnitBuffObjIndex[i] < 0)
        {
            continue;
        }

        CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj)
        {
            continue;
        }

        //判断是否同类型的Buff
        const SFightBuffConfig* pstBuffConfig = FightBuffCfgMgr().GetConfig(pstBuffObj->GetUnitBuffID());
        if(!pstBuffConfig)
        {
            LOGERROR("Failed to get fight unit buff config, invalid buff id %d\n", pstBuffObj->GetUnitBuffID());
            continue;
        }

        if(pstBuffConfig->iBuffType == pstNewBuffConfig->iBuffType)
        {
            return true;
        }
    }

    return false;
}

//是否有相同ID的BUFF
bool CUnitBufferManager::HasBuffOfSameID(int iBuffID)
{
    if(iBuffID <= 0)
    {
        return false;
    }

    for(int i=0; i<m_iUnitBuffObjNum; ++i)
    {
        if(m_aiUnitBuffObjIndex[i] < 0)
        {
            continue;
        }

        CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj)
        {
            continue;
        }

        if(pstBuffObj->GetUnitBuffID() == iBuffID)
        {
            return true;
        }
    }

    return false;
}

//清理BuffManager中所有的BuffObj
void CUnitBufferManager::ClearBuffObj()
{
    for(int i=0; i<MAX_FIGHT_UNIT_BUFF_NUM; ++i)
    {
        if(m_aiUnitBuffObjIndex[i] < 0)
        {
            continue;
        }

        GameType<CUnitBufferObj>::Del(m_aiUnitBuffObjIndex[i]);

        m_aiUnitBuffObjIndex[i] = -1;
    }

    return;
}

//处理Buff生效的实际效果
int CUnitBufferManager::DoOneBuffEffect(unsigned int uin, int iCrossID, const SFightBuffConfig& stBuffConfig, BuffEffect& stEffectNotify, int iCastUnitID, int iTriggerUnitID, int* pDamageNum)
{
    int iRet = T_SERVER_SUCESS;

    //根据Buff生效的目标，执行相应的操作
    switch(stBuffConfig.aiProcessID[BUFF_PARAM_TARGET])
    {
        case BUFF_TARGET_SELF:
            {
                //中Buff的单位本身
                iRet = ProcessRealBuffEffect(uin, iCrossID, stBuffConfig, m_iUnitID, stEffectNotify);
            }
            break;

        case BUFF_TARGET_CAST:
            {
                //施法者,操作类型只能是加BUFF
                if(stBuffConfig.iProcessType!=BUFF_OPERA_ADDBUFF && stBuffConfig.iProcessType!=BUFF_OPERA_GUARD)
                {
                    iRet = T_ZONE_SYSTEM_INVALID_CFG;
                }
                else
                {
                    iRet = ProcessRealBuffEffect(uin, iCrossID, stBuffConfig, iCastUnitID, stEffectNotify, pDamageNum);
                }   
            }
            break;

        case BUFF_TARGET_TRIGGER:
            {
                //触发者,操作类型只能是加BUFF
                if(stBuffConfig.iProcessType != BUFF_OPERA_ADDBUFF && stBuffConfig.iProcessType!=BUFF_OPERA_GUARD)
                {
                    iRet = T_ZONE_SYSTEM_INVALID_CFG;
                }
                else
                {
                    iRet = ProcessRealBuffEffect(uin, iCrossID, stBuffConfig, iTriggerUnitID, stEffectNotify, pDamageNum);
                }   
            }
            break;

        default:
            {
                LOGERROR("Failed to do buff effect, invalid target type %d, buff id %d\n", stBuffConfig.aiProcessID[BUFF_PARAM_TARGET], stBuffConfig.iConfigID);
                return T_ZONE_SYSTEM_INVALID_CFG;
            }
            break;
    }

    if(iRet)
    {
        LOGERROR("Failed to process buff effect, ret %d, unit id %d, buff id %d\n", iRet, m_iUnitID, stBuffConfig.iConfigID);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

//处理可以叠加的Buff,返回true表示新Buff可以叠加上去
bool CUnitBufferManager::ProcessOverlyingBuff(int iBuffOverlyingType, int iBuffLevel, RemoveBuffEffect& stRemoveBuffNotify)
{
    for(int i=0; i<m_iUnitBuffObjNum; ++i)
    {
        if(m_aiUnitBuffObjIndex[i] < 0)
        {
            continue;
        }

        CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj)
        {
            continue;
        }

        //判断是否同类型的Buff
        const SFightBuffConfig* pstBuffConfig = FightBuffCfgMgr().GetConfig(pstBuffObj->GetUnitBuffID());
        if(!pstBuffConfig)
        {
            LOGERROR("Failed to get fight unit buff config, invalid buff id %d\n", pstBuffObj->GetUnitBuffID());
            continue;
        }

        if(pstBuffConfig->iBuffType != iBuffOverlyingType)
        {
            continue;
        }

        //判断新Buff强度
        if(iBuffLevel < pstBuffConfig->iBuffLevel)
        {
            //同类型的不能替换
            return false;
        }

        //新的能够替换老的，移除老的Buff
        DelUnitBuffByIndex(m_aiUnitBuffObjIndex[i], stRemoveBuffNotify);

        //返回可以替换
        return true;
    }

    return true;
}

//对目标生效一个BUFF的实际效果
int CUnitBufferManager::ProcessRealBuffEffect(unsigned int uin, int iCrossID, const SFightBuffConfig& stBuffConfig, int iTargetUnitID, BuffEffect& stEffectNotify, int* pDamageNum)
{
    //判断Buff的操作类型是否正确
    if(iTargetUnitID==m_iUnitID && (stBuffConfig.iProcessType==BUFF_OPERA_ADDBUFF || stBuffConfig.iProcessType==BUFF_OPERA_GUARD))
    {
        LOGERROR("Failed to process attr change buff on other units, buff id %d\n", stBuffConfig.iConfigID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //获取目标对象
    CCombatUnitObj* pstTargetUnitObj = CCombatUtility::GetCombatUnitObj(iTargetUnitID);
    if(!pstTargetUnitObj)
    {
        LOGERROR("Failed to get combat unit obj, invalid unit id %d\n", iTargetUnitID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //设置生效的BUFF ID和目标
    stEffectNotify.set_ibuffid(stBuffConfig.iConfigID);
    stEffectNotify.set_itargetunitid(iTargetUnitID);

    int iRet = T_SERVER_SUCESS;

    //根据操作类型，执行相应的操作
    switch(stBuffConfig.iProcessType)
    {
        case BUFF_OPERA_ADDATTR:
            {
                //加目标的属性
                int iEffect = 0;
                pstTargetUnitObj->AddFightAttr(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF], stBuffConfig.aiProcessID[BUFF_PARAM_VALUE], &iEffect);

                if(stBuffConfig.iEffectType == BUFF_EFFECT_ROUNDS)
                {
                    //非永久，需要记录生效的效果
                    //获取BuffObj
                    CUnitBufferObj* pstBuffObj = GetUnitBufferObjByID(stBuffConfig.iConfigID);
                    if(!pstBuffObj)
                    {
                        LOGERROR("Failed to do one buff effect, invalid buff id %d\n", stBuffConfig.iConfigID);
                        return T_ZONE_SYSTEM_PARA_ERR;
                    }

                    pstBuffObj->SetBuffEffect(pstBuffObj->GetBuffEffect()+iEffect);
                }

                //处理属性替换类型Buff的替换前的值
                ProcessReplaceAttrBuffValue(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF], iEffect);

                stEffectNotify.set_iattrtype(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF]);
                stEffectNotify.set_iattrchange(iEffect);
            }
            break;

        case BUFF_OPERA_ADDRATEATTR:
            {
                //加目标的百分比属性
                int iAddAttr = 0;
                if(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF] == FIGHT_ATTR_HP)
                {
                    //血量取最大血量
                    iAddAttr = pstTargetUnitObj->GetFightAttr(FIGHT_ATTR_HPMAX)*stBuffConfig.aiProcessID[BUFF_PARAM_VALUE]/100;
                }
                else
                {
                    //其他的属性都取当前值
                    iAddAttr = pstTargetUnitObj->GetFightAttr(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF])*stBuffConfig.aiProcessID[BUFF_PARAM_VALUE]/100;
                }

                int iEffect = 0;
                pstTargetUnitObj->AddFightAttr(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF], iAddAttr, &iEffect);

                if(stBuffConfig.iEffectType == BUFF_EFFECT_ROUNDS)
                {
                    //非永久，需要记录生效的效果

                    //获取BuffObj
                    CUnitBufferObj* pstBuffObj = GetUnitBufferObjByID(stBuffConfig.iConfigID);
                    if(!pstBuffObj)
                    {
                        LOGERROR("Failed to do one buff effect, invalid buff id %d\n", stBuffConfig.iConfigID);
                        return T_ZONE_SYSTEM_PARA_ERR;
                    }

                    pstBuffObj->SetBuffEffect(pstBuffObj->GetBuffEffect()+iEffect);
                }

                //处理属性替换类型Buff的替换前的值
                ProcessReplaceAttrBuffValue(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF], iEffect);

                stEffectNotify.set_iattrtype(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF]);
                stEffectNotify.set_iattrchange(iEffect);
            }
            break;

        case BUFF_OPERA_REPLACEATTR:
            {
                //替换目标属性
                int iOldAttr = pstTargetUnitObj->GetFightAttr(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF]);

                int iAddAttr = stBuffConfig.aiProcessID[BUFF_PARAM_VALUE] - iOldAttr;

                pstTargetUnitObj->AddFightAttr(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF], iAddAttr);

                if(stBuffConfig.iEffectType == BUFF_EFFECT_ROUNDS)
                {
                    //非永久，需要记录原来的属性值

                    //获取BuffObj
                    CUnitBufferObj* pstBuffObj = GetUnitBufferObjByID(stBuffConfig.iConfigID);
                    if(!pstBuffObj)
                    {
                        LOGERROR("Failed to do one buff effect, invalid buff id %d\n", stBuffConfig.iConfigID);
                        return T_ZONE_SYSTEM_PARA_ERR;
                    }

                    pstBuffObj->SetBuffEffect(iOldAttr);
                }

                stEffectNotify.set_iattrtype(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF]);
                stEffectNotify.set_iattrchange(iAddAttr);
            }
            break;

        case BUFF_OPERA_LOCKATTR:
            {
                //暂时没有，不做
                ;
            }
            break;

        case BUFF_OPERA_ADDBUFF:
            {
                //给目标增加一个Buff
                iRet = pstTargetUnitObj->AddUnitBuff(uin, iCrossID, stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF], m_iUnitID, *(stEffectNotify.mutable_staddbuff()));
            }
            break;

        case BUFF_OPERA_UNARM:
            {
                //缴械，让武器不生效
                CCombatUtility::ProcessWeaponEffect(*pstTargetUnitObj, false);
            }
            break;

        case BUFF_OPERA_FORBIDACTION:
            {
                //禁止行动
                pstTargetUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTACTION, true);
            }
            break;

        case BUFF_OPERA_FORBIDROUND:
            {
                //禁止进入行动回合
                pstTargetUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTROUND, true);
            }
            break;

        case BUFF_OPERA_FORBIDMOVE:
            {
                //禁止移动
                pstTargetUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTMOVE, true);
            }
            break;

        case BUFF_OPERA_IGNOREJOINTATK:
            {
                //无视合围
                pstTargetUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_IGNOREJOINT, true);
            }
            break;

        case BUFF_OPERA_UNBENDING:
            {
                //不屈
                pstTargetUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_UNBENDING, true);
            }
            break;

        case BUFF_OPERA_GUARD:
            {
                //守护类型的BUFF
                if(!pDamageNum)
                {
                    return T_ZONE_SYSTEM_PARA_ERR;
                }

                if(pstTargetUnitObj->IsCombatUnitDead())
                {
                    //目标已经死亡
                    return T_SERVER_SUCESS;
                }

                int iGuardDamage = (*pDamageNum) * stBuffConfig.aiProcessID[BUFF_PARAM_VALUE]/100;
                *pDamageNum -= iGuardDamage;

                int iRealGuardDamage = 0;
                pstTargetUnitObj->AddFightAttr(FIGHT_ATTR_HP, -iGuardDamage, &iRealGuardDamage);

                stEffectNotify.set_iattrtype(FIGHT_ATTR_HP);
                stEffectNotify.set_iattrchange(iRealGuardDamage);
            }
            break;

        default:
            {
                LOGERROR("Failed to do one buff effect, invalid opera type %d, buff id %d\n", stBuffConfig.iProcessType, stBuffConfig.iConfigID);
                return T_ZONE_SYSTEM_INVALID_CFG;
            }
            break;
    }

    if(iRet)
    {
        LOGERROR("Failed to do one buff effect, ret %d, buff id %d\n", iRet, stBuffConfig.iConfigID);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

//处理属性替换类型Buff的替换前的值
void CUnitBufferManager::ProcessReplaceAttrBuffValue(int iAttrType, int iAddNum)
{
   //遍历所有的BUFF，处理属性替换类型的值
    for(int i=0; i<m_iUnitBuffObjNum; ++i)
    {
        CUnitBufferObj* pstBuffObj= GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj)
        {
            continue;
        }

        const SFightBuffConfig* pstFightBuffConfig = FightBuffCfgMgr().GetConfig(pstBuffObj->GetUnitBuffID());
        if(!pstFightBuffConfig)
        {
            LOGERROR("Failed to get fight buff config, invalid buff id %d\n", pstBuffObj->GetUnitBuffID());
            return;
        }

        if(pstFightBuffConfig->iProcessType==BUFF_OPERA_REPLACEATTR && 
           pstFightBuffConfig->aiProcessID[BUFF_PARAM_ATTRBUFF]==iAttrType)
        {
            //对BUFF中记录的值进行同样的处理
            pstBuffObj->SetBuffEffect(pstBuffObj->GetBuffEffect()+iAddNum);
        }
    }

    return;
}

//根据BuffObj的Index删除Buff
int CUnitBufferManager::DelUnitBuffByIndex(int& iBuffObjIndex, RemoveBuffEffect& stRemoveBuffNotify)
{
    //获取BuffObj
    CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(iBuffObjIndex);
    if(!pstBuffObj)
    {
        LOGERROR("Failed to get buffer unit obj, invalid obj index %d\n", iBuffObjIndex);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //获取BUFF配置
    const SFightBuffConfig* pstFightBuffConfig = FightBuffCfgMgr().GetConfig(pstBuffObj->GetUnitBuffID());
    if(!pstFightBuffConfig)
    {
        LOGERROR("Failed to get fight buff config, invalid buff id %d\n", pstBuffObj->GetUnitBuffID());
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //设置返回
    RemoveOneBuffEffect* pstRemoveOneNotify = stRemoveBuffNotify.add_stremoveones();
    pstRemoveOneNotify->set_ibuffid(pstBuffObj->GetUnitBuffID());

    if(pstFightBuffConfig->iEffectType == BUFF_EFFECT_ROUNDS)
    {
        //非永久生效，需要移除BUFF效果
        int iRet = RemoveBuffEffects(*pstBuffObj, *pstFightBuffConfig, *pstRemoveOneNotify);
        if(iRet)
        {
            LOGERROR("Failed to remove buff effect, ret %d\n", iRet);
            return iRet;
        }
    }

    //删除BUFF
    GameType<CUnitBufferObj>::Del(iBuffObjIndex);
    iBuffObjIndex = m_aiUnitBuffObjIndex[--m_iUnitBuffObjNum];

    //如果有关联BUFF，则尝试删除关联BUFF
    if(pstFightBuffConfig->iDelBuffID != 0)
    {
        return DelUnitBuffByID(pstFightBuffConfig->iDelBuffID, stRemoveBuffNotify);
    }

    return T_SERVER_SUCESS;
}

//移除BUFF效果，根据ID
int CUnitBufferManager::DelUnitBuffByID(int iBuffID, RemoveBuffEffect& stRemoveBuffNotify)
{
    for(int i=0; i<m_iUnitBuffObjNum; ++i)
    {
        CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj || pstBuffObj->GetUnitBuffID()!=iBuffID)
        {
            continue;
        }

        //删除对应ID的BUFF
        return DelUnitBuffByIndex(m_aiUnitBuffObjIndex[i], stRemoveBuffNotify);
    }

    return T_SERVER_SUCESS;
}

//移除BUFF的效果
int CUnitBufferManager::RemoveBuffEffects(CUnitBufferObj& stBuffObj, const SFightBuffConfig& stFightBuffConfig, RemoveOneBuffEffect& stRemoveOneNotify)
{
    //判断BUFF的效果类型非永久，并且目标是自己
    if(stFightBuffConfig.iEffectType == BUFF_EFFECT_FOREVER
       || stFightBuffConfig.aiProcessID[BUFF_PARAM_TARGET]!=BUFF_TARGET_SELF)
    {
        //非永久类型，不移除
        return T_SERVER_SUCESS;
    }

    //获取战斗单位CombatUnitObj
    CCombatUnitObj* pstCombatObj = CCombatUtility::GetCombatUnitObj(m_iUnitID);
    if(!pstCombatObj)
    {
        LOGERROR("Failed to get combat unit obj, combat unit id %d\n", m_iUnitID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //根据BUFF类型，移除BUFF效果
    switch(stFightBuffConfig.iProcessType)
    {
        case BUFF_OPERA_ADDATTR:
        case BUFF_OPERA_ADDRATEATTR:
            {
                int iRealEffect = 0;
                pstCombatObj->AddFightAttr(stFightBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF], -stBuffObj.GetBuffEffect(), &iRealEffect);
                
                stRemoveOneNotify.set_iattrtype(stFightBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF]);
                stRemoveOneNotify.set_iattrchange(iRealEffect);
            }
            break;

        case BUFF_OPERA_REPLACEATTR:
            {
                int iNewAttr = stBuffObj.GetBuffEffect();
                if(iNewAttr < 0)
                {
                    iNewAttr = 0;
                }

                int iAddAttr = iNewAttr - pstCombatObj->GetFightAttr(stFightBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF]);
                pstCombatObj->AddFightAttr(stFightBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF], iAddAttr);

                stRemoveOneNotify.set_iattrtype(stFightBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF]);
                stRemoveOneNotify.set_iattrchange(iAddAttr);
            }
            break;

        case BUFF_OPERA_UNARM:
            {
                //扣除缴械的效果
                CCombatUtility::ProcessWeaponEffect(*pstCombatObj, true);
            }
            break;

        case BUFF_OPERA_FORBIDACTION:
            {
                pstCombatObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTACTION, false);
            }
            break;

        case BUFF_OPERA_FORBIDMOVE:
            {
                pstCombatObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTMOVE, false);
            }
            break;

        case BUFF_OPERA_FORBIDROUND:
            {
                pstCombatObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTROUND, false);
            }
            break;

        case BUFF_OPERA_UNBENDING:
            {
                pstCombatObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_UNBENDING, false);
            }
            break;

        case BUFF_OPERA_IGNOREJOINTATK:
            {
                pstCombatObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_IGNOREJOINT, false);
            }
            break;

        case BUFF_OPERA_GUARD:
            {
                ;
            }
            break;

        case BUFF_OPERA_ADDBUFF:
        case BUFF_OPERA_LOCKATTR:
        default:
            {
                LOGERROR("Failed to remove buff effect, invalid buff opera type %d, buff id %d\n", stFightBuffConfig.iProcessType, stBuffObj.GetUnitBuffID());
                return T_ZONE_SYSTEM_INVALID_CFG;
            }
            break;
    }

    return T_SERVER_SUCESS;
}

//根据ID查找BuffObj
CUnitBufferObj* CUnitBufferManager::GetUnitBufferObjByID(int iBuffID)
{
    for(int i=0; i<m_iUnitBuffObjNum; ++i)
    {
        CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj)
        {
            continue;
        }

        if(pstBuffObj->GetUnitBuffID() == iBuffID)
        {
            return pstBuffObj;
        }
    }

    return NULL;
}

//创建一个新的BufferObj
CUnitBufferObj* CUnitBufferManager::CreateNewBufferObj()
{
    if(m_iUnitBuffObjNum >= MAX_FIGHT_UNIT_BUFF_NUM)
    {
        //超过支持的Buff上限
        return NULL;
    }

    //创建并初始化新的BuffObj
    m_aiUnitBuffObjIndex[m_iUnitBuffObjNum] = GameType<CUnitBufferObj>::Create();
    if(m_aiUnitBuffObjIndex[m_iUnitBuffObjNum] < 0)
    {
        LOGERROR("Failed to create CUnitBufferObj!\n");
        return NULL;
    }

    CUnitBufferObj* pstNewBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[m_iUnitBuffObjNum]);
    if(!pstNewBuffObj)
    {
        LOGERROR("Failed to get new buffer obj!\n");
        return NULL;
    }

    ++m_iUnitBuffObjNum;

    return pstNewBuffObj;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
