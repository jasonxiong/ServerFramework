#ifndef __REP_THINGS_UTILITY_HPP__
#define __REP_THINGS_UTILITY_HPP__

//玩家背包物品操作工具类

#include "GameConfigDefine.hpp"

class CGameRoleObj;
class CRepThingsUtility
{
public:
   
    //获取物品的大类型
    static int GetItemType(int iItemID);

    //获取物品数量
    static int GetItemNumByID(CGameRoleObj& rstRoleObj, int iItemID);

    //增删物品的接口,如果iAddNum小于0表示删除
    static int AddItemNumByID(CGameRoleObj& stRoleObj, int iItemID, int iAddNum, int iItemChannel);

    //设置物品数量，如果为0则删除所有该ID的物品
    static void SetItemNumByID(CGameRoleObj& rstRoleObj, int iItemID, int iItemNum, int iItemChannel);
};

#endif
