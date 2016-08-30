#include <assert.h>
#include <string.h>

#include "ProtoDataUtility.hpp"
#include "GameProtocol.hpp"
#include "ModuleHelper.hpp"
#include "ZoneObjectHelper.hpp"
#include "HandlerFactory.hpp"
#include "ZoneErrorNumDef.hpp"
#include "UpdateRoleInfoHandler.hpp"
#include "ZoneMsgHelper.hpp"
#include "UnitUtility.hpp"
#include "LoginHandler.hpp"
#include "HandlerHelper.hpp"
#include "GameConfigDefine.hpp"
#include "LogoutHandler.hpp"

#include "FetchRoleWorldHandler.hpp"

using namespace ServerLib;

CFetchRoleWorldHandler::~CFetchRoleWorldHandler(void)
{
}

GameProtocolMsg CFetchRoleWorldHandler::m_stZoneMsg;
int CFetchRoleWorldHandler::OnClientMsg()
{
    int iRet = -1;
    switch (m_pRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_WORLD_FETCHROLE_RESPONSE:
        {
            iRet = OnFetchRole();
            break;
        }

    default:
        {
            break;
        }
    }

    return iRet;
}

int CFetchRoleWorldHandler::OnFetchRole()
{
    const World_FetchRole_Response& rstFetchResp = m_pRequestMsg->m_stmsgbody().m_stworld_fetchrole_response();
    unsigned int uiUin = rstFetchResp.stroleid().uin();

    // 获取会话
    CSessionManager* pSessionManager = CModuleHelper::GetSessionManager();
    m_pSession = pSessionManager->FindSessionByRoleID(rstFetchResp.stroleid());
    if(!m_pSession)
    {
        TRACESVR("Cannot Find Session: Uin = %u\n", uiUin);
        return -2;
    }

    // Session 已经被使用,login时检查
    if (rstFetchResp.bislogin() && m_pSession->GetBindingRole() != NULL)
    {
        TRACESVR("Session Already Binding Role: Uin = %u, Session = %d\n",
            uiUin, m_pSession->GetID());
        CHandlerHelper::SetErrorCode(T_ZONE_SESSION_EXISTS_ERR);
        CLoginHandler::LoginFailed(m_pSession->GetNetHead());
        return -3;
    }

    // 获取角色信息失败
    if (rstFetchResp.iresult() != 0)
    {
        TRACESVR("FetchRole Failed: Uin = %u, ResultID = %d\n", uiUin, rstFetchResp.iresult());
        CHandlerHelper::SetErrorCode(T_ZONE_SESSION_EXISTS_ERR);
        CLoginHandler::LoginFailed(m_pSession->GetNetHead());
        pSessionManager->DeleteSession(m_pSession->GetID());
        return -4;
    }

    // 检查uin是否一致
    if (uiUin != m_pSession->GetRoleID().uin())
    {
        CHandlerHelper::SetErrorCode(T_ZONE_SESSION_EXISTS_ERR);
        CLoginHandler::LoginFailed(m_pSession->GetNetHead());
        pSessionManager->DeleteSession(m_pSession->GetID());

        // 清除World缓存
        CLogoutHandler::NotifyLogoutToWorld(rstFetchResp.stroleid());

        TRACESVR("Invalid Session: Uin = %u\n", uiUin);

        return -5;
    }

    TRACESVR("FetchRole OK: Uin = %u\n", uiUin);

    // 登录角色
    int iRet = LoginRole();
    if (iRet < 0)
    {
        CHandlerHelper::SetErrorCode(T_ZONE_SESSION_EXISTS_ERR);
        CLoginHandler::LoginFailed(m_pSession->GetNetHead());
        pSessionManager->DeleteSession(m_pSession->GetID());

        // 清除World缓存
        CLogoutHandler::NotifyLogoutToWorld(rstFetchResp.stroleid());

        TRACESVR("LoginRole Failed: iRet = %d\n", iRet);

        return -6;
    }

    TRACESVR("LoginRole OK: Uin = %u\n", uiUin);

    // 查询邮件封数
    //todo jasonxiong 后面再统一开发，暂时删除掉功能
    QueryMailNumber();

    // 拉取游戏好友缓存到角色数据中
    //todo jasonxiong 后面再统一开发，暂时删除掉功能
    FetchGameFriend();

    return 0;
}

int CFetchRoleWorldHandler::LoginRole()
{
    const World_FetchRole_Response& rstFetchResp = m_pRequestMsg->m_stmsgbody().m_stworld_fetchrole_response();
    unsigned int uiUin = rstFetchResp.stroleid().uin();

    // 创建角色对象
    m_pRoleObj = (CGameRoleObj*)CUnitUtility::CreateUnit(EUT_ROLE, uiUin);
    if(!m_pRoleObj)
    {
        TRACESVR("Create RoleObj Failed.\n");
        return -1;
    }

    // 初始化角色数据系统
    int iRet = m_pRoleObj->InitRole(rstFetchResp.stroleid());
    if (iRet < 0)
    {
        TRACESVR("Init Role Failed: iRet = %d\n", iRet);

        CUnitUtility::DeleteUnit(&m_pRoleObj->GetUnitInfo());

        return -2;
    }

    // 将会话和角色绑定
    m_pSession->SetBindingRole(m_pRoleObj);
    m_pRoleObj->SetSessionID(m_pSession->GetID());

    // 初始化角色数据
    iRet = InitRoleData();
    if (iRet < 0)
    {
        CUnitUtility::DeleteUnit(&m_pRoleObj->GetUnitInfo());
        TRACESVR("Init RoleData Failed: iRet = %d\n", iRet);

        return -3;
    }

    // 通知登录成功
    CLoginHandler::LoginOK(uiUin);

    // 登陆后初始化
    InitRoleAfterLogin();

    return 0;
}

// 初始化角色数据
int CFetchRoleWorldHandler::InitRoleData()
{
    const World_FetchRole_Response& rstFetchResp = m_pRequestMsg->m_stmsgbody().m_stworld_fetchrole_response();
    unsigned int uiUin = rstFetchResp.stroleid().uin();

    CGameSessionObj *pSession = m_pRoleObj->GetSession();
    if (pSession == NULL)
    {
        TRACESVR("No session: %u\n", uiUin);
        return -20;
    }

    int iRet = T_SERVER_SUCESS;

    //1.初始化玩家的基本信息
    BASEDBINFO stBaseInfo;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().strbaseinfo(), stBaseInfo))
    {
        TRACESVR("Failed to decode base proto data, uin %u\n", uiUin);
        return -21;
    }
    m_pRoleObj->InitBaseInfoFromDB(stBaseInfo);

    //2.初始化玩家的任务信息
    QUESTDBINFO stQuestInfo;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().strquestinfo(), stQuestInfo))
    {
        TRACESVR("Failed to decode quest proto data, uin %u\n", uiUin);
        return -22;
    }
    //m_pRoleObj->InitStoryFromDB(stQuestInfo);
    
    //3.初始化玩家的物品信息
    ITEMDBINFO stItemInfo;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().striteminfo(), stItemInfo))
    {
        TRACESVR("Failed to decode item proto data, uin %u\n", uiUin);
        return -23;
    }
    m_pRoleObj->InitRepThingsFromDB(stItemInfo);

    //4.初始化玩家的其他战斗信息
    FIGHTDBINFO stFightInfo;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().strfightinfo(), stFightInfo))
    {
        TRACESVR("Failed to decode fight proto data, uin %u\n", uiUin);
        return -25;
    }

    iRet = m_pRoleObj->InitFightUnitFromDB(stFightInfo);
    if(iRet)
    {
        TRACESVR("Failed to init fight unit data, ret %d, uin %u\n", iRet, uiUin);

        m_pRoleObj->GetFightUnitManager().ClearFightUnitObj();

        return iRet;
    }

    //5.初始化玩家的好友信息
    FRIENDDBINFO stFriendInfo;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().strfriendinfo(), stFriendInfo))
    {
        TRACESVR("Failed to decode friend proto data, uin %u\n", uiUin);
        return -26;
    }

    //6.初始化玩家的Reserved1字段
    RESERVED1DBINFO stReserved1Info;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().strreserved1(), stReserved1Info))
    {
        TRACESVR("Failed to decode reserved1 proto data, uin %u\n", uiUin);
        return -27;
    }

    //7.初始化玩家的Reserved2字段
    RESERVED2DBINFO stReserved2Info;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().strreserved2(), stReserved2Info))
    {
        TRACESVR("Failed to decode reserved2 proto data, uin %u\n", uiUin);
        return -28;
    }
     
    //todo jasonxiong 初始化玩家的其他信息

    // GM标志
    //todo jasonxiong 先注释掉所有GM相关的功能，后续需要时再进行开发
    /*
    m_pRoleObj->GetRoleInfo().m_stBaseProfile.m_cGMType = rstDBRoleInfo.fGM;
    CWhiteListConfig& rWhiteListConfig = WhiteListCfgMgr();
    bool bGM = rWhiteListConfig.IsInGMList(m_pRoleObj->GetRoleID().m_uiUin);
    if (bGM)
    {
        CUnitUtility::SetUnitStatus(&(m_pRoleObj->GetRoleInfo().m_stUnitInfo), EUS_ISGM);
    }
    else
    {
        CUnitUtility::ClearUnitStatus(&(m_pRoleObj->GetRoleInfo().m_stUnitInfo), EUS_ISGM);
    } 
    */ 

    // 各种时间
    m_pRoleObj->SetLoginCount(stBaseInfo.ilogincount()+1);

    m_pRoleObj->SetOnline();      

    return 0;
}

// 登录后初始化
int CFetchRoleWorldHandler::InitRoleAfterLogin()
{
    return 0;
}

int CFetchRoleWorldHandler::QueryMailNumber()
{
    //todo jasonxiong 后面再统一开发
    /*
    CMailMsgHandler* pHandler =
        dynamic_cast<CMailMsgHandler*>(CHandlerFactory::GetHandler(MSGID_MAIL_MAILNUMBER_NOTIFY));
    ASSERT_AND_LOG_RTN_INT(pHandler);

    return pHandler->SendMailNumberRequestToWorld(m_pRoleObj->GetRoleID());
    */

    return 0;
}

int CFetchRoleWorldHandler::FetchGameFriend()
{
    //todo jasonxiong 后面再统一开发
    /*
    CFriendMsgHandler* pHandler =
        dynamic_cast<CFriendMsgHandler*>(CHandlerFactory::GetHandler(MSGID_FRIEND_FETCHGAMEFRIEND_REQUEST));
    ASSERT_AND_LOG_RTN_INT(pHandler);

    return pHandler->SendFetchGameFriendRequestToWorld(m_pRoleObj->GetRoleID());
    */

    return 0;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
