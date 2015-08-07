
#ifndef __COMBAT_TRAP_OBJ_HPP__
#define __COMBAT_TRAP_OBJ_HPP__

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"
#include "ZoneObjectAllocator.hpp"

struct TCOMBATTRAPINFO
{
    int iConfigID;          //机关的配置ID
    int iTrapObjID;         //机关对象的ObjID

    TCOMBATTRAPINFO()
    {
        memset(this, 0, sizeof(*this));
    };
};

//战场上机关的对象TrapObj
class CCombatTrapObj : public CObj
{
public:
    CCombatTrapObj();
    virtual ~CCombatTrapObj();
    virtual int Initialize();

    DECLARE_DYN

public:

    //初始化机关
    int InitTrap(int iConfigID, int iCamp, const TUNITPOSITION& stPos, int iDirection);

    //获取机关当前的位置
    const TUNITPOSITION& GetPosition();

    //修改机关的血量
    void AddTrapHP(int iAddNum);
    int GetTrapHP();

    //判断机关是否已损坏
    bool IsTrapDead();

    //判断机关是否可被攻击
    bool IsTrapCanAttacked();

    //获取该机关的配置
    const GameConfig::STrapConfig* GetConfig();

    //获取机关的配置ID
    int GetTrapConfigID();

    //获取机关的阵营
    int GetTrapCamp();

///////////////////////////////////////////////////////////////////////
private:

    //机关的配置ID
    int m_iConfigID;

    //机关的配置
    const GameConfig::STrapConfig* m_pstConfig;

    //机关的当前血量
    int m_iTrapHp;

    //机关所属的阵营
    int m_iCamp;

    //机关的当前位置
    TUNITPOSITION m_stPos;

    //机关当前的朝向
    int m_iDirection;
};

#endif

