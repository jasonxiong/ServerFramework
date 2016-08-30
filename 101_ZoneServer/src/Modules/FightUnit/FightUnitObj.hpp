#ifndef __FIGHT_UNIT_OBJ_HPP__
#define __FIGHT_UNIT_OBJ_HPP__

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"
#include "ZoneObjectAllocator.hpp"
#include "RepThingsManager.hpp"

//玩家战斗单位对象

class CFightUnitObj : public CObj
{
public:
    CFightUnitObj();
    virtual ~CFightUnitObj();
    virtual int Initialize();

    void SetOwner(unsigned int uin);
    unsigned GetOwner();

    DECLARE_DYN

public:

    //获取战斗单位的ID
    int GetFightUnitID() { return m_iConfigID; };

    //装卸物品, bIsEquip = true 表示装物品，= false 表示卸物品
    int UnitEquipItem(int iSlot, RepItem& stItem, bool bIsEquip);

    //根据位置获取物品信息
    RepItem* GetItemInfo(int iSlot);

    //根据ID初始化战斗单位信息
    int InitFightUnitByID(int iFightUnitID);

    //初始化战斗对象的属性
    int InitFightAttr(int* aiAttribute, int iAttrMaxNum);

    //初始化战斗对象的物品
    void InitFightUnitItem(RepItem* pstItems, int iItemMaxNum);

    //获取普攻ID
    int GetNormalSkill();

    //战斗单位的AI信息
    int GetFightUnitAIID();

    //战斗单位属性
    int GetFightUnitAttr(int iAttr);
    int AddFightUnitAttr(int iAttr, int iAddAttrNum);
    int AddUnitAttrWithoutNotify(int iAttr, int iAddAttrNum);

	//等级
	inline int GetLevel(){ return m_iLevel;}

	//经验
	inline int GetExp(){return m_iLevelExp;}

public:
    
    //数据库操作函数
    void InitFightUnitFromDB(const OneFightUnitInfo& stUnitInfo);
    void UpdateFightUnitToDB(OneFightUnitInfo& stUnitInfo); 

///////////////////////////////////////////////////////////////////////
public:

    //玩家角色的uin
    unsigned int m_uiUin;

    //战斗单位配置ID
    int m_iConfigID;

    //玩家战斗单位的AI ID
    int m_iFightUnitAIID;

	//等级
	int m_iLevel;

	//经验
	int m_iLevelExp;

    //战斗单位的属性
    int m_aiUnitAttribute[MAX_FIGHT_UNIT_ATTRIBUTE_NUM];

    //战斗单位的普攻
    int m_iNormalSkill;

    //战斗单位物品信息
    int m_iSlotNum;
    RepItem m_astItems[MAX_UNIT_ITEM_SLOT];
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
