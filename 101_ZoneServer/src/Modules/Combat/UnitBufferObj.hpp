
#ifndef __UNIT_BUFFER_OBJ_HPP__
#define __UNIT_BUFFER_OBJ_HPP__

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"
#include "ZoneObjectAllocator.hpp"

//战斗单位的Buff对象
class CUnitBufferObj : public CObj
{
public:
    CUnitBufferObj();
    virtual ~CUnitBufferObj();
    virtual int Initialize();

    DECLARE_DYN

public:

    //初始化一个Buff
    int InitUnitBuff(int iBuffID, int iCastUnitID, int iRemainRounds);
    
    //BuffID
    int GetUnitBuffID();
    void SetUnitBuffID(int iBuffID);

    //Buff剩余回合数
    int GetRemainRound();
    void SetRemainRound(int iRemainRound);

    //Buff产生的实际效果
    int GetBuffEffect();
    void SetBuffEffect(int iRealEffect);

    //添加该Buff的战斗单位ID
    int GetCastUnitID();
    void GetCastUnitID(int iCastUnitID);

///////////////////////////////////////////////////////////////////////
private:
    
    //产生的Buff的ID
    int m_iBuffID;

    //Buff剩余的回合数
    int m_iRemainRounds;

    //Buff产生的实际效果
    int m_iRealEffect;

    //施加该Buff的战斗单位ID
    int m_iCastUnitID;
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
