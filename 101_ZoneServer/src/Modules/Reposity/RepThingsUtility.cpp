
#include "GameProtocol.hpp"
#include "GameRole.hpp"
#include "ZoneErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "RepThingsManager.hpp"
#include "ModuleShortcut.hpp"

#include "RepThingsUtility.hpp"
   
using namespace GameConfig;   

//获取物品的大类型
int CRepThingsUtility::GetItemType(int iItemID)
{
    if(iItemID == 0)
    {
        return REP_ITEM_TYPE_INVALID;
    }

    //ID为3000000以下的是道具
    if(iItemID<3000000)
    {
        return REP_ITEM_TYPE_PROP;
    }
    
    return REP_ITEM_TYPE_EQUIP;
}

//获取物品数量
int CRepThingsUtility::GetItemNumByID(CGameRoleObj& rstRoleObj, int iItemID)
{
    //获取背包物品管理器
    CRepThingsManager& rstThingsManager = rstRoleObj.GetRepThingsManager();

    return rstThingsManager.GetRepItemNumByID(iItemID);
}

//增删物品的接口,如果iAddNum小于0表示删除
int CRepThingsUtility::AddItemNumByID(CGameRoleObj& stRoleObj, int iItemID, int iAddNum, int iItemChannel)
{
    //获取背包物品管理器
    CRepThingsManager& rstThingsManager = stRoleObj.GetRepThingsManager();

    return rstThingsManager.AddItemNumByID(iItemID, iAddNum, iItemChannel);
}

//设置物品数量，如果为0则删除所有该ID的物品
void CRepThingsUtility::SetItemNumByID(CGameRoleObj& rstRoleObj, int iItemID, int iItemNum, int iItemChannel)
{
    int iRepItemNum = CRepThingsUtility::GetItemNumByID(rstRoleObj, iItemID);

    CRepThingsUtility::AddItemNumByID(rstRoleObj, iItemID, iItemNum-iRepItemNum, iItemChannel);

    return;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
