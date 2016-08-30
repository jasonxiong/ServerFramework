#ifndef __UNIT_BUFFER_MANAGER_HPP__
#define __UNIT_BUFFER_MANAGER_HPP__

#include <vector>

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"

using namespace GameConfig;

class CUnitBufferObj;
class CUnitBufferManager
{
public:
    CUnitBufferManager();
    ~CUnitBufferManager();

    int Initialize();

    //拥有的战斗单位的UnitID
    int GetOwnerUnitID();
    void SetOwnerUnitID(int iUnitID);

    //增加一个Buff
    int AddUnitBuff(unsigned int uin, int iCrossID, int iBuffID, int iCastUnitID, Zone_CombatAddBuff_Notify& stNotify);

    //按类型生效Buff
    int DoUnitBuffEffect(unsigned int uin, int iCrossID, int iTriggerType, int iTriggerUnitID, Zone_DoBuffEffect_Notify& stNotify, int* pDamageNum = NULL);

    //减少所有BUFF的回合数
    int DecreaseAllBuffRound(Zone_RemoveBuff_Notify& stNotify);

    //移除BUFF效果，根据ID
    int DelUnitBuffByID(int iBuffID, RemoveBuffEffect& stRemoveBuffNotify);

public:

    //获取所有BUFF的配置ID
    void GetUnitBuffID(std::vector<int>& vBuffIDs);

    //是否有BUFFID同类型的BUFF
    bool HasBuffOfSameType(int iBuffID);

    //是否有相同ID的BUFF
    bool HasBuffOfSameID(int iBuffID);

    //清理BuffManager中所有的BuffObj
    void ClearBuffObj();

private:

    //处理Buff生效的实际效果
    int DoOneBuffEffect(unsigned int uin, int iCrossID, const SFightBuffConfig& stBuffConfig, BuffEffect& stEffectNotify, int iCastUnitID = -1, int iTriggerUnitID = -1, int* pDamageNum = NULL);

    //处理可以叠加的Buff,返回true表示新Buff可以叠加上去
    bool ProcessOverlyingBuff(int iBuffOverlyingType, int iBuffLevel, RemoveBuffEffect& stRemoveBuffNotify);

    //对目标生效一个BUFF的实际效果
    int ProcessRealBuffEffect(unsigned int uin, int iCrossID, const SFightBuffConfig& stBuffConfig, int iTargetUnitID, BuffEffect& stEffectNotify, int* pDamageNum = NULL);

    //处理属性替换类型Buff的替换前的值
    void ProcessReplaceAttrBuffValue(int iAttrType, int iAddNum);

    //根据BuffObj的Index删除Buff
    int DelUnitBuffByIndex(int& iBuffObjIndex, RemoveBuffEffect& stRemoveBuffNotify);

    //移除BUFF的效果
    int RemoveBuffEffects(CUnitBufferObj& stBuffObj, const SFightBuffConfig& stFightBuffConfig, RemoveOneBuffEffect& stRemoveOneNotify);

    //根据ID查找BuffObj
    CUnitBufferObj* GetUnitBufferObjByID(int iBuffID);

    //创建一个新的BufferObj
    CUnitBufferObj* CreateNewBufferObj();

private:

    //BuffManager的拥有者UnitID
    int m_iUnitID;

    //所有附加的BuffObj的index
    int m_iUnitBuffObjNum;
    int m_aiUnitBuffObjIndex[MAX_FIGHT_UNIT_BUFF_NUM];
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
