
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "ZoneMsgHelper.hpp"
#include "ZoneObjectHelper.hpp"
#include "GameRole.hpp"
#include "ModuleShortcut.hpp"
#include "FightUnitObj.hpp"
#include "RepThingsUtility.hpp"
#include "ModuleHelper.hpp"
#include "ZoneOssLog.hpp"
#include "DropRewardUtility.hpp"

#include "RepThingsFramework.hpp"

using namespace ServerLib;

GameProtocolMsg CRepThingsFramework::m_stGameMsg;

CRepThingsFramework* CRepThingsFramework::Instance()
{
    static CRepThingsFramework* pInstance = NULL;
    if(!pInstance)
    {
        pInstance = new CRepThingsFramework();
    }

    return pInstance;
}

CRepThingsFramework::CRepThingsFramework()
{

}

CRepThingsFramework::~CRepThingsFramework()
{

}

//玩家对背包物品的操作
int CRepThingsFramework::OperaRepItem(unsigned int uin, const Zone_RepOpera_Request& rstRequest)
{
    //初始化成员变量
    m_uiUin = uin;
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_REPOPERA_RESPONSE);

    CGameRoleObj* pstRoleObj = GameTypeK32<CGameRoleObj>::GetByKey(m_uiUin);
    if(!pstRoleObj)
    {
        LOGERROR("Failed to get game role obj, uin %u\n", m_uiUin);
        return T_ZONE_GAMEROLE_NOT_EXIST;
    }

    //拉取相应的消息体
    Zone_RepOpera_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_repopera_response();

    ReqOperaType eType = rstRequest.etype();

    int iRet = T_SERVER_SUCESS;
    switch(eType)
    {
        case REQ_OPERA_TYPE_SPLIT:
            {
                iRet = SplitRepItem(*pstRoleObj, rstRequest.islotindex(), rstRequest.iitemnum(), *pstResp);
            }
            break;

        case REQ_OPERA_TYPE_EXCHANGE:
            {
                iRet = ExchangeRepItem(*pstRoleObj, rstRequest.islotindex(), rstRequest.iotherslotindex(), *pstResp);
            }
            break;

        case REQ_OPERA_TYPE_USE:
            {
                iRet = UseRepItem(*pstRoleObj, rstRequest.islotindex(), rstRequest.iitemnum(), rstRequest.ifightunitid(), *pstResp);
            }
            break;

        case REQ_OPERA_TYPE_DELETE:
            {
                iRet = DeleteRepItem(*pstRoleObj, rstRequest.islotindex(), rstRequest.iitemnum(), *pstResp);
            }
            break;

        case REQ_OPERA_TYPE_SORT:
            {
                iRet = SortRepItem(*pstRoleObj, *pstResp);
            }
            break;

        default:
            {
                iRet = T_ZONE_SYSTEM_PARA_ERR;
            }
            break;
    }

    if(iRet)
    {
        LOGERROR("Failed to opera rep item, opera type %d, ret %d, uin %u\n", rstRequest.etype(), iRet, m_uiUin);
        return iRet;
    }

    //填充并回包给客户端
    pstResp->set_etype(rstRequest.etype());
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    return T_SERVER_SUCESS;
}

//玩家拉取背包物品的详细信息
int CRepThingsFramework::GetRepItemInfo(unsigned int uin, const Zone_GetRepInfo_Request& rstRequest)
{
    //初始化成员变量
    m_uiUin = uin;
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_GETREPINFO_RESPONSE);

    CGameRoleObj* pstRoleObj = GameTypeK32<CGameRoleObj>::GetByKey(m_uiUin);
    if(!pstRoleObj)
    {
        LOGERROR("Failed to get game role obj, uin %u\n", m_uiUin);
        return T_ZONE_GAMEROLE_NOT_EXIST;
    }

    //拉取相应的消息体
    Zone_GetRepInfo_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_getrepinfo_response();

    //获取背包管理器
    pstRoleObj->GetRepThingsManager().UpdateRepItemToDB(*pstResp->mutable_stitems());

    //填充并回包给客户端
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    return T_SERVER_SUCESS;
}

//玩家从背包穿装备
int CRepThingsFramework::WearEquipment(unsigned int uin, const Zone_WearEquip_Request& rstRequest)
{
    //todo jasonxiong5 再来开发
    /*
    //初始化成员变量
    m_uiUin = uin;
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_WEAREQUIP_RESPONSE);

    CGameRoleObj* pstRoleObj = GameTypeK32<CGameRoleObj>::GetByKey(m_uiUin);
    if(!pstRoleObj)
    {
        LOGERROR("Failed to get game role obj, uin %u\n", m_uiUin);
        return T_ZONE_GAMEROLE_NOT_EXIST;
    }

    int iRet = T_SERVER_SUCESS;

    //背包穿装备相应的消息体
    Zone_WearEquip_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_wearequip_response();

    //穿装备
    if(rstRequest.biswear())
    {
        //获取背包管理器
        CRepThingsManager& stRepThingsManager = pstRoleObj->GetRepThingsManager();
    
        int iEquipItemID = stRepThingsManager.GetRepItemID(rstRequest.islotindex());
    
        const SEquipmentConfig* pstEquipConfig = EquipmentCfgMgr().GetConfig(iEquipItemID);
        if(!pstEquipConfig)
        {
            LOGERROR("Failed to get equipment config, equipment id %d\n", iEquipItemID);
            return T_ZONE_SYSTEM_INVALID_CFG;
        }
    
        //todo jasonxiong4 这边修改为按ID取
        //获取战斗单位的信息
        CFightUnitObj* pstFightUnitObj = pstRoleObj->GetFightUnitManager().GetFightUnitByID(rstRequest.iunitindex());
        if(!pstFightUnitObj)
        {
            LOGERROR("Failed to get fight unit obj, index %d, uin %u\n", rstRequest.iunitindex(), m_uiUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //先删除该位置的装备
        RepItem* stOldEquipment = pstFightUnitObj->GetItemInfo(pstEquipConfig->iEquipType);
        if(stOldEquipment->iItemID != 0)
        {
            //读取老装备的配置
            const SEquipmentConfig* pstOldEquipConfig = EquipmentCfgMgr().GetConfig(stOldEquipment->iItemID);
            if(!pstOldEquipConfig)
            {
                LOGERROR("Failed to get old equipment config, item id %d\n", stOldEquipment->iItemID);
                return T_ZONE_SYSTEM_INVALID_CFG;
            }

            iRet = pstFightUnitObj->UnitEquipItem(pstOldEquipConfig->iEquipType, *stOldEquipment, false);
            if(iRet)
            {
                LOGERROR("Failed to take off fight unit equipment, unit index %d, equip type %d, uin %u\n", rstRequest.iunitindex(), pstEquipConfig->iEquipType, m_uiUin);
                return iRet;
            }
        
            if(stOldEquipment->iItemID != 0)
            {
                //战斗单位身上脱下的装备放到背包中
                if(stRepThingsManager.GetEmptyItemSlotIndex() < 0)
                {
                    //背包无空位，不能卸下
                    return T_ZONE_USER_REP_FULL;
                }
        
                iRet = stRepThingsManager.AddRepItem(*stOldEquipment, -1, 0);
                if(iRet)
                {
                    return iRet;
                }
            }
        }
    
        //从背包中删除装备
        RepItem stWearEquip;
        iRet = stRepThingsManager.DeleteRepItem(rstRequest.islotindex(), stWearEquip, 0);
        if(iRet)
        {
            LOGERROR("Failed to delete rep equipment, slot %d, ret %d, uin %u\n", rstRequest.islotindex(), iRet, m_uiUin);
            return iRet;
        }
    
        //战斗单位身上穿上装备
        iRet = pstFightUnitObj->UnitEquipItem(pstEquipConfig->iEquipType, stWearEquip, true);
        if(iRet)
        {
            LOGERROR("Failed to wear equipment, unit index %d, equipment type %d, uin %u\n", rstRequest.iunitindex(), pstEquipConfig->iEquipType, m_uiUin);
            return iRet;
        }

        //封装返回
        OneSlotInfo* pstSlotInfo = pstResp->mutable_stslotinfo();
        pstSlotInfo->set_iitemid(stWearEquip.iItemID);
        pstSlotInfo->set_iitemnum(stWearEquip.iItemNum);
    }
    //脱装备
    else
    {
        CRepThingsManager& stRepTingsManager = pstRoleObj->GetRepThingsManager();

        //先判断背包中是否有空位
        if(stRepTingsManager.GetEmptyItemSlotIndex() < 0)
        {
            LOGERROR("Failed to take off equipment, rep full, uin %u\n", m_uiUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //获取战斗单位的信息
        CFightUnitManager& stFightUnitManager = pstRoleObj->GetFightUnitManager();

        //todo jasonxiong4 这边修改为按ID取
        CFightUnitObj* pstUnitObj = stFightUnitManager.GetFightUnitByID(rstRequest.iunitindex());
        if(!pstUnitObj)
        {
            LOGERROR("Failed to get fight unit by index %d, uin %u\n", rstRequest.iunitindex(), m_uiUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //获取该装备信息
        RepItem* stOldEquipInfo = pstUnitObj->GetItemInfo(rstRequest.iequipsubtype());
        const SEquipmentConfig* pstEquipConfig = EquipmentCfgMgr().GetConfig(stOldEquipInfo->iItemID);
        if(!pstEquipConfig)
        {
            LOGERROR("Failed to get equipment config, invalid id %d\n", stOldEquipInfo->iItemID);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //先从战斗单位身上脱下装备
        RepItem stEquipItem;
        iRet = pstUnitObj->UnitEquipItem(rstRequest.iequipsubtype(), stEquipItem, false);
        if(iRet)
        {
            LOGERROR("Failed to take off equipment, unit index %d, equip type %d, uin %u\n", rstRequest.iunitindex(), rstRequest.iequipsubtype(), m_uiUin);
            return iRet;
        }

        if(stEquipItem.iItemID==0 || stEquipItem.iItemNum==0)
        {
            LOGERROR("Failed to take off equipment, no equip, unit index %d, equip %d, uin %u\n", rstRequest.iunitindex(), rstRequest.iequipsubtype(), m_uiUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //将脱下的装备放进背包
        iRet = stRepTingsManager.AddRepItem(stEquipItem, rstRequest.islotindex(), 0);
        if(iRet)
        {
            LOGERROR("Failed to add new rep item, ret %d, uin %u\n", iRet, m_uiUin);
            return iRet;
        }
    }

    //填充并回包给客户端
    pstResp->set_biswear(rstRequest.biswear());
    pstResp->set_iunitindex(rstRequest.iunitindex());
    pstResp->set_iequipsubtype(rstRequest.iequipsubtype());
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();
    */

    return T_SERVER_SUCESS;
}

//玩家出售背包物品
int CRepThingsFramework::SellRepItem(unsigned int uin, const Zone_SellItem_Request& rstRequest)
{
    //初始化成员变量
    m_uiUin = uin;
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_SELLITEM_RESPONSE);

    CGameRoleObj* pstRoleObj = GameTypeK32<CGameRoleObj>::GetByKey(m_uiUin);
    if(!pstRoleObj)
    {
        LOGERROR("Failed to get game role obj, uin %u\n", m_uiUin);
        return T_ZONE_GAMEROLE_NOT_EXIST;
    }

    //获取背包管理器
    CRepThingsManager& rstRepThingsManager = pstRoleObj->GetRepThingsManager();

    int iRet = T_SERVER_SUCESS;

    //玩家出售背包物品的返回
    Zone_SellItem_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_sellitem_response();

    //扣除物品
    for(int i=0; i<rstRequest.stitems_size(); ++i)
    {
        const SellItemInfo& rstSellInfo = rstRequest.stitems(i);

        //删除物品
        iRet = rstRepThingsManager.DeleteRepItem(rstSellInfo.iitemslot(), rstSellInfo.isellnum(), 0);
        if(iRet)
        {
            LOGERROR("Failed to delete rep item, item slot %d, num %d, uin %u\n", rstSellInfo.iitemslot(), rstSellInfo.isellnum(), m_uiUin);
            return iRet;
        }
    }

    //填充并回包给客户端
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    return T_SERVER_SUCESS;
}

//拆分背包格子中的物品, iItemNum是需要拆分到新物品格子里的数目
int CRepThingsFramework::SplitRepItem(CGameRoleObj& stRoleObj, int iSlotIndex, int iItemNum, Zone_RepOpera_Response& rstResponse)
{
    //判断参数
    if(iItemNum == 0)
    {
        LOGERROR("Failed to split rep item, invalid num %d, uin %u\n", iItemNum, m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //获取背包管理器
    CRepThingsManager& stRepThingsManager = stRoleObj.GetRepThingsManager();

    int iRealItemNum = stRepThingsManager.GetRepItemNum(iSlotIndex);
    //判断拆分物品的数量是否够
    if(iRealItemNum <= iItemNum)
    {
        LOGERROR("Failed to spilt rep item, num need:real %d:%d, slot %d, uin %u\n", iItemNum, iRealItemNum, iSlotIndex, m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //判断拆分出来的物品是否能放进去
    int iEmptyRepSlot = stRepThingsManager.GetEmptyItemSlotIndex();
    if(iEmptyRepSlot < 0)
    {
        LOGERROR("Failed to split rep item, rep full, uin %u\n", m_uiUin);
        return T_ZONE_USER_REP_FULL;
    }

    //先减原来格子的物品
    int iRet = stRepThingsManager.DeleteRepItem(iSlotIndex, iItemNum);
    if(iRet)
    {
        LOGERROR("Failed to delete rep item, slot %d, num %d, uin %u\n", iSlotIndex, iItemNum, m_uiUin);
        return iRet;
    }

    int iItemID = stRepThingsManager.GetRepItemID(iSlotIndex);

    //增加新格子里的物品
    RepItem stAddItem;
    stAddItem.iItemID = iItemID;
    stAddItem.iItemNum = iItemNum;

    iRet = stRepThingsManager.AddRepItem(stAddItem, iEmptyRepSlot);
    if(iRet)
    {
        LOGERROR("Failed to add rep item, item id %d, num %d, uin %u\n", iItemID, iItemNum, m_uiUin);
        return iRet;
    }

    LOGDEBUG("Success to split rep item, slot %d, num %d, uin %u\n", iSlotIndex, iItemNum, m_uiUin);

    return T_SERVER_SUCESS;
}

//交换和合并背包格子中的物品
int CRepThingsFramework::ExchangeRepItem(CGameRoleObj& stRoleObj, int iSlotIndex, int iOtherSlotIndex, Zone_RepOpera_Response& rstResponse)
{
    //获取背包管理器
    CRepThingsManager& stRepThingsManager = stRoleObj.GetRepThingsManager();

    //交换和合并背包格子
    int iRet = stRepThingsManager.ExchangeRepItem(iSlotIndex, iOtherSlotIndex);
    if(iRet)
    {
        LOGERROR("Failed to exchange rep item, slot %d, other slot %d, uin %u\n", iSlotIndex, iOtherSlotIndex, m_uiUin);
        return iRet;
    }

    LOGDEBUG("Success to exchange rep item, slot %d, other slot %d, uin %u\n", iSlotIndex, iOtherSlotIndex, m_uiUin);

    return T_SERVER_SUCESS;
}

//使用背包中的物品
int CRepThingsFramework::UseRepItem(CGameRoleObj& stRoleObj, int iSlotIndex, int iItemNum, int iFightUnitID, Zone_RepOpera_Response& rstResponse)
{
    //获取背包管理器
    CRepThingsManager& stRepThingsManager = stRoleObj.GetRepThingsManager();

    //获取使用物品的ID
    int iItemID = stRepThingsManager.GetRepItemID(iSlotIndex);

    //检查物品数量是否够
    if(iItemNum > stRepThingsManager.GetRepItemNum(iSlotIndex))
    {
        LOGERROR("Failed to use rep item, item id %d, uin %u\n", iItemID, m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //先扣除物品
    int iRet = stRepThingsManager.DeleteRepItem(iSlotIndex, iItemNum, 0);
    if(iRet)
    {
        LOGERROR("Failed to delete rep item, slot %d, num %d, ret %d, uin %u\n", iSlotIndex, iItemNum, iRet, m_uiUin);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

//删除背包中的物品
int CRepThingsFramework::DeleteRepItem(CGameRoleObj& stRoleObj, int iSlotIndex, int iItemNum, Zone_RepOpera_Response& rstResponse)
{
    //获取背包管理器
    CRepThingsManager& stRepThingsManager = stRoleObj.GetRepThingsManager();

    int iSlotItemNum = stRepThingsManager.GetRepItemNum(iSlotIndex);
    if(iSlotItemNum < iItemNum)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = stRepThingsManager.DeleteRepItem(iSlotIndex, iItemNum, 0);
    if(iRet)
    {
        LOGERROR("Failed to delete rep item, slot %d, uin %u\n", iSlotIndex, m_uiUin);
        return iRet;
    }

    LOGDEBUG("Success to delete rep item, slot %d, uin %u\n", iSlotIndex, m_uiUin);

    return T_SERVER_SUCESS;
}

//整理物品背包
int CRepThingsFramework::SortRepItem(CGameRoleObj& stRoleObj, Zone_RepOpera_Response& rstResponse)
{
    //获取背包管理器
    CRepThingsManager& stRepThingsManager = stRoleObj.GetRepThingsManager();

    int iRet = stRepThingsManager.SortRepItem();
    if(iRet)
    {
        LOGERROR("Failed to sort rep item, ret %d, uin %u\n", iRet, m_uiUin);
        return iRet;
    }

    LOGDEBUG("Success to sort rep item, uin %u\n", m_uiUin);

    return T_SERVER_SUCESS;
}

int CRepThingsFramework::SendSuccessResponse()
{
    CGameRoleObj* pstRoleObj = GameTypeK32<CGameRoleObj>::GetByKey(m_uiUin);
    if(!pstRoleObj)
    {
        LOGERROR("Failed to get game role obj, uin %u\n", m_uiUin);
        return -1;
    }

    CZoneMsgHelper::SendZoneMsgToRole(m_stGameMsg, pstRoleObj);

    return 0;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
