#ifndef __REP_THINGS_MANAGER_HPP__
#define __REP_THINGS_MANAGER_HPP__

#include <vector>

#include "GameConfigDefine.hpp"
#include "GameProtocol.hpp"

using namespace GameConfig;

struct RepItem
{
    int iItemID;
    int iItemNum;

    RepItem()
    {
        Reset();
    };

    void Reset()
    {
        iItemID = 0;
        iItemNum = 0;
    };
};

class CGameRoleObj;
class CRepThingsManager
{
public:
    CRepThingsManager();
    ~CRepThingsManager();

public:

    //初始化
    int Initialize();

    void SetOwner(unsigned int uin);
    CGameRoleObj* GetOwner();

    //背包增加物品
    int AddRepItem(int iItemID, int iItemNum, int iItemChannel = 0);

    //背包增加物品, iItemSlot表示放到指定的位置, -1表示不指定slot位置
    int AddRepItem(const RepItem& stItem, int iItemSlot = -1, int iItemChannel = 0);

    //背包删除物品
    int DeleteRepItem(int iItemSlot, int iItemNum, int iItemChannel = 0);

    //背包删除物品
    int DeleteRepItem(int iItemSlot, RepItem& stDeleteItem, int iItemChannel = 0);

    //背包删除物品
    int DeleteRepItemByID(int iItemID, int iItemChannel = 0);
    int DeleteRepItemByID(int iItemID, int iItemNum, int iItemChannel);

    //背包交换物品,如果是相同ID的则合并 2->1
    int ExchangeRepItem(int iItemSlot1, int iItemSlot2, bool bIsSortExchange=false);

    //获取背包中物品的数量
    int GetRepItemNum(int iItemSlot);

    int GetRepItemNumByID(int iItemID);

    //增加背包中物品数量，为负表示删除
    int AddItemNumByID(int iItemID, int iItemNum, int iItemChannel = 0);

    //获取背包中物品的ID
    int GetRepItemID(int iItemSlot);

    //获取背包空格子的SlotIndex,小于0 表示背包没有空格子
    int GetEmptyItemSlotIndex();

    //背包整理物品
    int SortRepItem();

    //开启背包格子
    int OpenRepBlock();

    //开启所有的背包格子
    int OpenAllRepBlock();

    //背包物品数据库操作函数
    void UpdateRepItemToDB(ITEMDBINFO& rstItemInfo);
    void InitRepItemFromDB(const ITEMDBINFO& stItemInfo);

private:

    //获取空格子的Slot，返回-1表示没有找到
    int GetEmptyItemSlot();

    //按装备子类型进行排序,第一个参数会更新起始的位置
    int SortEquipBySubType(int& iBeginIndex, int iMaxIndex, int iSubType);

    //按道具子类型进行排序，第一个参数会更新起始的位置
    int SortPropItemBySubType(int& iBeginIndex, int iMaxIndex, int iSubType);

    //发送背包物品变化的通知
    void SendRepChangeNotify(std::vector<int>& vItemSlot, int iItemChannel = 0);

private:

    //拥有该背包的角色的uin
    unsigned int m_uiUin;

    //当前背包开启的格子数目
    int m_iRepBlockNum;

    //背包格子中的物品信息
    RepItem m_astRepItem[MAX_REP_BLOCK_NUM]; 

    //玩家背包物品发生变化的通知
    static GameProtocolMsg m_stRepChangeNotify;
};

#endif
