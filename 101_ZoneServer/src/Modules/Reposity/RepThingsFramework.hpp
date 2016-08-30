#ifndef __REP_THINGS_FRAMEWORK_HPP__
#define __REP_THINGS_FRAMEWORK_HPP__

#include "GameProtocol.hpp"
#include "RepThingsManager.hpp"

class CGameRoleObj;
class CFightUnitObj;

class CRepThingsFramework
{
public:

    static CRepThingsFramework* Instance();

    ~CRepThingsFramework();

    //玩家对背包物品的操作
    int OperaRepItem(unsigned int uin, const Zone_RepOpera_Request& rstRequest);

    //玩家拉取背包物品的详细信息
    int GetRepItemInfo(unsigned int uin, const Zone_GetRepInfo_Request& rstRequest);

    //玩家从背包穿装备
    int WearEquipment(unsigned int uin, const Zone_WearEquip_Request& rstRequest);

    //玩家出售背包物品
    int SellRepItem(unsigned int uin, const Zone_SellItem_Request& rstRequest);

private:
    
    //拆分背包格子中的物品
    int SplitRepItem(CGameRoleObj& stRoleObj, int iSlotIndex, int iItemNum, Zone_RepOpera_Response& rstResponse);

    //交换和合并背包格子中的物品
    int ExchangeRepItem(CGameRoleObj& stRoleObj, int iSlotIndex, int iOtherSlotIndex, Zone_RepOpera_Response& rstResponse);

    //使用背包中的物品
    int UseRepItem(CGameRoleObj& stRoleObj, int iSlotIndex, int iItemNum, int iFightUnitID, Zone_RepOpera_Response& rstResponse);

    //删除背包中的物品
    int DeleteRepItem(CGameRoleObj& stRoleObj, int iSlotIndex, int iItemNum, Zone_RepOpera_Response& rstResponse);

    //整理物品背包
    int SortRepItem(CGameRoleObj& stRoleObj, Zone_RepOpera_Response& rstResponse);

    //发送处理成功的返回
    int SendSuccessResponse();

    CRepThingsFramework();

private:
    unsigned int m_uiUin;

    static GameProtocolMsg m_stGameMsg;
};

#endif


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
