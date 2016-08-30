#include <string.h>

#include "ProtoDataUtility.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"
#include "ModuleHelper.hpp"
#include "ConfigHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "LogAdapter.hpp"
#include "StringUtility.hpp"
#include "WorldErrorNumDef.hpp"

#include "CreateRoleAccountHandler.hpp"

using namespace ServerLib;

CCreateRoleAccountHandler::~CCreateRoleAccountHandler()
{

}

int CCreateRoleAccountHandler::OnClientMsg(const void* pMsg)
{
    ASSERT_AND_LOG_RTN_INT(pMsg);

    m_pMsg = (GameProtocolMsg*)pMsg;

    switch (m_pMsg->m_stmsghead().m_uimsgid())
    {
        case MSGID_ACCOUNT_CREATEROLE_REQUEST:
        {
            OnRequestCreateRoleAccount();
            break;
        }
        case MSGID_WORLD_CREATEROLE_RESPONSE:
        {
            OnResponseCreateRoleAccount();
            break;
        }
        default:
            {
                break;
            }
    }

    return 0;
}


int CCreateRoleAccountHandler::OnRequestCreateRoleAccount()
{
    const CreateRole_Account_Request& rstRequest = m_pMsg->m_stmsgbody().m_staccountcreaterolerequest();
    unsigned int uiUin = rstRequest.uin();
    if (uiUin == 0)
    {
        LOGERROR("Wrong CreateRoleAccount Request, Uin %u\n", uiUin);
        return -1;
    }

    LOGDEBUG("Correct Uin CreateRoleAccount Request, Uin %u\n", uiUin);

    //转发创建角色帐号的请求给DB Server
    int iRet = SendCreateRoleRequestToDBSvr();
    if(iRet)
    {
        CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, m_pMsg->m_stmsghead().m_uisessionfd(), MSGID_ACCOUNT_CREATEROLE_RESPONSE, uiUin);
        m_stWorldMsg.mutable_m_stmsgbody()->mutable_m_staccountcreateroleresponse()->set_iresult(T_WORLD_CANNOT_CREATE_ROLE);
        CWorldMsgHelper::SendWorldMsgToAccount(m_stWorldMsg);
    }

    return iRet;
}

int CCreateRoleAccountHandler::OnResponseCreateRoleAccount()
{
    const World_CreateRole_Response& rstResponse = m_pMsg->m_stmsgbody().m_stworld_createrole_response();

    unsigned int uiUin = rstResponse.stroleid().uin();
    if (uiUin == 0)
    {
        LOGERROR("Wrong CreateRoleAccount Response, Uin %d\n", uiUin);
        return -1;
    }

    // 转换成MsgID_Account_CreateRole_Response发送给Account
    int iRet = SendCreateRoleResponseToAccount();

    LOGDEBUG("CreateRoleAccount Response To Account, Uin %d\n", uiUin);

    return iRet;
}

int CCreateRoleAccountHandler::SendCreateRoleRequestToDBSvr()
{
    //生成消息头
    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg,
                                     m_pMsg->m_stmsghead().m_uisessionfd(),
                                     MSGID_WORLD_CREATEROLE_REQUEST,
                                     m_pMsg->m_stmsghead().m_uin());

    //生成创建角色帐号的消息体
    World_CreateRole_Request* pstRequest = m_stWorldMsg.mutable_m_stmsgbody()->mutable_m_stworld_createrole_request();

    int iRet = InitBirthInfo(*pstRequest);
    if(iRet)
    {
        LOGERROR("Fail to init user init birth info, uin %u, ret 0x%0x\n", m_pMsg->m_stmsghead().m_uin(), iRet);
        return iRet;
    }

    iRet = CWorldMsgHelper::SendWorldMsgToRoleDB(m_stWorldMsg);

    return iRet;
}

int CCreateRoleAccountHandler::SendCreateRoleResponseToAccount()
{
    //初始化消息头
    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg,
                                     m_pMsg->m_stmsghead().m_uisessionfd(),
                                     MSGID_ACCOUNT_CREATEROLE_RESPONSE,
                                     m_pMsg->m_stmsghead().m_uin());

    const World_CreateRole_Response& rstResp = m_pMsg->m_stmsgbody().m_stworld_createrole_response();
    CreateRole_Account_Response* pstAccountResp = m_stWorldMsg.mutable_m_stmsgbody()->mutable_m_staccountcreateroleresponse();

    pstAccountResp->set_iresult(rstResp.iresult());
    pstAccountResp->mutable_stroleid()->CopyFrom(rstResp.stroleid());
    //todo jasonxiong2 单机版不需要名字
    //pstAccountResp->set_sznickname(rstResp.sznickname());

    //todo jasonxiong 后续创建完帐号后可能需要返回更多的东西，根据策划需要再添加

    int iRet = CWorldMsgHelper::SendWorldMsgToAccount(m_stWorldMsg);

    return iRet;
}

int CCreateRoleAccountHandler::InitBirthInfo(World_CreateRole_Request& rstRequest)
{
    const CreateRole_Account_Request& stAccountRequest = m_pMsg->m_stmsgbody().m_staccountcreaterolerequest();

    rstRequest.set_uin(stAccountRequest.uin());
    rstRequest.set_world(stAccountRequest.worldid());

    //todo jasonxiong2 先不要名字
    //rstRequest.set_sznickname(stAccountRequest.sznickname());

    CConfigManager* pstConfigManager = CModuleHelper::GetConfigManager();

    //todo jasonxiong2 这个暂时只考虑一条配置
    const SRoleBirthConfig* pstBirthConfig = pstConfigManager->GetBirthConfigManager().GetConfig(1);
    if(!pstBirthConfig)
    {
        LOGERROR("Failed to get role birth config, invalid id %u\n", 1);
        return T_WORLD_SYSYTEM_INVALID_CFG;
    }

    //初始化玩家基础属性
    unsigned int uTimeNow = time(NULL);

    BASEDBINFO stBaseInfo;

    //todo jasonxiong2 单机版不需要名字
    //stBaseInfo.set_sznickname(stAccountRequest.sznickname());
    stBaseInfo.set_icreatetime(uTimeNow);

    //初始化资源
    for(int i=RESOURCE_TYPE_INVALID; i<RESOURCE_TYPE_MAX; ++i)
    {
        stBaseInfo.add_iresources(0);
    }

    stBaseInfo.set_iresources(RESOURCE_TYPE_COIN, pstBirthConfig->iInitCoin);
    stBaseInfo.set_iresources(RESOURCE_TYPE_CASH, pstBirthConfig->iInitGold);
    stBaseInfo.set_iresources(RESOURCE_TYPE_ENERGY, pstBirthConfig->iInitEnergy);

    //初始化玩家的任务信息
    QUESTDBINFO stQuestInfo;

    //初始化玩家的背包信息
    ITEMDBINFO stItemInfo;
    stItemInfo.set_m_iopenedslotnum(pstBirthConfig->iInitBagNum);

    //初始化玩家的战斗单位信息
    int iRet = InitFightUnitInfo(*pstBirthConfig, rstRequest);
    if(iRet)
    {
        LOGERROR("Failed to init fight unit info, uin %u, ret %d\n", rstRequest.uin(), iRet);
        return iRet;
    }

    //初始化玩家的好友信息
    FRIENDDBINFO stFriendInfo;

    //初始化保留字段1
    RESERVED1DBINFO stReserved1;

    //初始化保留字段2
    RESERVED2DBINFO stReserved2;

    //1.将玩家基础信息编码到请求中
    if(!EncodeProtoData(stBaseInfo, *rstRequest.mutable_stbirthdata()->mutable_strbaseinfo()))
    {
        LOGERROR("Failed to encode base proto data, uin %u!\n", rstRequest.uin());
        return -1;
    }

    LOGDEBUG("Base proto Info compress rate %d:%zu, uin %u\n", stBaseInfo.ByteSize(), rstRequest.stbirthdata().strbaseinfo().size(), rstRequest.uin());

    //2.将玩家任务信息编码到请求中
    if(!EncodeProtoData(stQuestInfo, *rstRequest.mutable_stbirthdata()->mutable_strquestinfo()))
    {
        LOGERROR("Failed to encode quest proto data, uin %u!\n", rstRequest.uin());
        return -2;
    }

    LOGDEBUG("quest proto Info compress rate %d:%zu, uin %u\n", stQuestInfo.ByteSize(), rstRequest.stbirthdata().strquestinfo().size(), rstRequest.uin());

    //3.将玩家物品信息编码到请求中
    if(!EncodeProtoData(stItemInfo, *rstRequest.mutable_stbirthdata()->mutable_striteminfo()))
    {
        LOGERROR("Failed to encode item proto data, uin %u!\n", rstRequest.uin());
        return -2;
    }

    LOGDEBUG("item proto Info compress rate %d:%zu, uin %u\n", stItemInfo.ByteSize(), rstRequest.stbirthdata().striteminfo().size(), rstRequest.uin());

    //4.将玩家好友信息信息编码到请求中
    if(!EncodeProtoData(stFriendInfo, *rstRequest.mutable_stbirthdata()->mutable_strfriendinfo()))
    {
        LOGERROR("Failed to encode friend proto data, uin %u!\n", rstRequest.uin());
        return -3;
    }

    LOGDEBUG("friend proto Info compress rate %d:%zu, uin %u\n", stFriendInfo.ByteSize(), rstRequest.stbirthdata().strfriendinfo().size(), rstRequest.uin());

    //5.将玩家Reserved1字段编码到请求中
    if(!EncodeProtoData(stReserved1, *rstRequest.mutable_stbirthdata()->mutable_strreserved1()))
    {
        LOGERROR("Failed to encode reserved1 proto data, uin %u!\n", rstRequest.uin());
        return -3;
    }

    LOGDEBUG("reserved1 proto Info compress rate %d:%zu, uin %u\n", stReserved1.ByteSize(), rstRequest.stbirthdata().strreserved1().size(), rstRequest.uin());

    //6.将玩家Reserved2字段编码到请求中
    if(!EncodeProtoData(stReserved2, *rstRequest.mutable_stbirthdata()->mutable_strreserved2()))
    {
        LOGERROR("Failed to encode reserved2 proto data, uin %u!\n", rstRequest.uin());
        return -3;
    }

    LOGDEBUG("reserved2 proto Info compress rate %d:%zu, uin %u\n", stReserved2.ByteSize(), rstRequest.stbirthdata().strreserved2().size(), rstRequest.uin());

    return 0;
}

int CCreateRoleAccountHandler::InitFightUnitInfo(const SRoleBirthConfig& stConfig, World_CreateRole_Request& rstRequest)
{
    static FIGHTDBINFO stFightInfo;
    stFightInfo.Clear();

    int iRet = T_SERVER_SUCESS;
    for(int i=0; i<stConfig.iUnitNum; ++i)
    {
        if(stConfig.aiUnitID[i] == 0)
        {
            continue;
        }

        //增加一个单位
        iRet = InitOneFightUnitInfo(stConfig.aiUnitID[i], *stFightInfo.mutable_stunitrep()->add_stunits());
        if(iRet)
        {
            LOGERROR("Failed to init one fight unit info, unit id %d, uin %u\n", stConfig.aiUnitID[i], rstRequest.uin());
            return -1;
        }

        //增加到阵型中
        stFightInfo.mutable_stforms()->add_ifightunitids(stConfig.aiUnitID[i]);
    }

    //将玩家战斗信息编码到请求中
    if(!EncodeProtoData(stFightInfo, *rstRequest.mutable_stbirthdata()->mutable_strfightinfo()))
    {
        LOGERROR("Failed to encode fight proto data, uin %u!\n", rstRequest.uin());
        return -3;
    }

    LOGDEBUG("fight proto Info compress rate %d:%zu, uin %u\n", stFightInfo.ByteSize(), rstRequest.stbirthdata().strfightinfo().size(), rstRequest.uin());

    return T_SERVER_SUCESS;
}

//初始化一个战斗单位
int CCreateRoleAccountHandler::InitOneFightUnitInfo(int iConfigID, OneFightUnitInfo& stOneFightUnit)
{
    const SFightUnitConfig* pstUnitConfig = CModuleHelper::GetConfigManager()->GetFightUnitConfigManager().GetConfig(iConfigID);
    if(!pstUnitConfig)
    {
        LOGERROR("Failed to get fight unit config, invalid id %d\n", iConfigID);
        return -1;
    }

    stOneFightUnit.set_iunitid(pstUnitConfig->iConfigID);
    stOneFightUnit.set_iunitaiid(pstUnitConfig->aiUnitAIID[0]);
    stOneFightUnit.set_inormalskillid(pstUnitConfig->iNormalSkill);
    
    //初始化属性
    for(int i=0; i<FIGHT_ATTR_MAX; ++i)
    {
        stOneFightUnit.add_iattributes(pstUnitConfig->aiAttribute[i]);
    }

    //初始化物品槽信息
    for(int i=0; i<pstUnitConfig->iInitSlot; ++i)
    {
        stOneFightUnit.mutable_stiteminfo()->add_stitems();
    }

    return T_SERVER_SUCESS;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
