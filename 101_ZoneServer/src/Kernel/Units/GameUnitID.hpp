
#ifndef __GAME_UNIT_ID_HPP__
#define __GAME_UNIT_ID_HPP__

// 场景单位ID类. 负责建立单位和所属场景之间的联系

#include "GameProtocol.hpp"
#include "ObjIdxList.hpp"
#include "ObjAllocator.hpp"

using namespace ServerLib;

class CGameUnitID : public CObj
{
public:
    CGameUnitID() {};
    virtual ~CGameUnitID() {};
    virtual int Initialize();
    virtual int Resume();

public:
    DECLARE_DYN

public:
    // 绑定对象Idx
    void BindObject(int iObjectIdx, unsigned char ucObjectType);

public:
    // 获取绑定对象的Idx
    int GetBindObjectIdx();

    // 获取绑定对象的类型
    int GetBindObjectType();

public:
    // 获取绑定对象的属性
    TUNITINFO* GetBindUnitInfo();

public:
    // 设置和获取同一场景内的单位链表
    TObjIdxListNode& GetListNode();

    // 设置和获取同一场景内相同类型单位的链表
    TObjIdxListNode& GetListNode2();

private:
    // 设置对象的属性
    void BindUnitInfo();

private: 
    int m_iBindObjectIdx;
    unsigned char m_ucBindObjectType;
    
    // 同一场景内, 所有单位的链表
    TObjIdxListNode m_stListNode;

    // 同一场景内, 相同类型的单位链表
    TObjIdxListNode m_stListNode2;

private:
    TUNITINFO* m_pstUnitInfo;
};

extern TObjIdxListNode* GetGameUnitIDListNode(int iIdx);
extern TObjIdxListNode* GetGameUnitIDListNode2(int iIdx);

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
