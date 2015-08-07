

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "ModuleShortcut.hpp"

#include "UnitBufferObj.hpp"

using namespace ServerLib;

IMPLEMENT_DYN(CUnitBufferObj)

CUnitBufferObj::CUnitBufferObj()
{
    Initialize();
}

CUnitBufferObj::~CUnitBufferObj()
{

}

int CUnitBufferObj::Initialize()
{
    //产生的Buff的ID
    m_iBuffID = 0;

    //Buff剩余的回合数
    m_iRemainRounds = 0;

    //Buff产生的实际效果
    m_iRealEffect = 0;

    //施加该Buff的战斗单位ID
    m_iCastUnitID = -1;

    return T_SERVER_SUCESS;
}

//初始化一个Buff
int CUnitBufferObj::InitUnitBuff(int iBuffID, int iCastUnitID, int iRemainRounds)
{
    m_iBuffID = iBuffID;
    m_iCastUnitID = iCastUnitID;
    m_iRemainRounds = iRemainRounds;

    m_iRealEffect = 0;

    return T_SERVER_SUCESS;
}
    
//BuffID
int CUnitBufferObj::GetUnitBuffID()
{
    return m_iBuffID;
}

void CUnitBufferObj::SetUnitBuffID(int iBuffID)
{
    m_iBuffID = iBuffID;
}

//Buff剩余回合数
int CUnitBufferObj::GetRemainRound()
{
    return m_iRemainRounds;
}

void CUnitBufferObj::SetRemainRound(int iRemainRound)
{
    m_iRemainRounds = iRemainRound;
}

//Buff产生的实际效果
int CUnitBufferObj::GetBuffEffect()
{
    return m_iRealEffect;
}

void CUnitBufferObj::SetBuffEffect(int iRealEffect)
{
    m_iRealEffect = iRealEffect;
}

//添加该Buff的战斗单位ID
int CUnitBufferObj::GetCastUnitID()
{
    return m_iCastUnitID;
}

void CUnitBufferObj::GetCastUnitID(int iCastUnitID)
{
    m_iCastUnitID = iCastUnitID;
}

    


