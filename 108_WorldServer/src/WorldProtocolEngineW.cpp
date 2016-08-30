#include <math.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "LogFile.hpp"
#include "AppLoopW.hpp"
#include "HandlerFactory.hpp"
#include "MsgStatistic.hpp"

#include "WorldProtocolEngineW.hpp"

using namespace ServerLib;

int CWorldProtocolEngineW::Initialize(bool bResume)
{
    m_stWorldMsg.Clear();

    return 0;
}

int CWorldProtocolEngineW::OnRecvCode(char* pszMsgBuffer, const int iMsgLength, EGameServerID enMsgPeer, int iInstance)
{
    ASSERT_AND_LOG_RTN_INT(pszMsgBuffer);

    int iRet = -1;
    timeval tvtmp = {0, 0};

    //解码消息
    char* pRealBuff = pszMsgBuffer + sizeof(unsigned short);
    int iBuffLen = iMsgLength - sizeof(unsigned short);

    if(!m_stWorldMsg.ParseFromArray(pRealBuff, iBuffLen))
    {
        LOGERROR("Fail to decode proto msg from buff!\n");
        return -1;
    }

    unsigned int uiMsgID = m_stWorldMsg.m_stmsghead().m_uimsgid();

    //根据MsgID获取消息处理器Handler
    IHandler* pHandler = CHandlerFactory::GetHandler(uiMsgID);
    if (!pHandler)
    {
        return ForwardMsg(pszMsgBuffer, iMsgLength);
    }

    LOGDEBUG("Recv msg from %d, id %d, length %d.\n",
        enMsgPeer, m_stWorldMsg.m_stmsghead().m_uimsgid(), iMsgLength);

    iRet = pHandler->OnClientMsg(&m_stWorldMsg);
    if(iRet < 0)
    {
        return -3;
    }

    MsgStatisticSingleton::Instance()->AddMsgStat(uiMsgID, iRet, 0, tvtmp, ESMT_FROM_SERVER);

    return 0;
}

int CWorldProtocolEngineW::ForwardMsg(char* pszMsgBuffer, const int iMsgLength)
{
    ASSERT_AND_LOG_RTN_INT(pszMsgBuffer);

    EGameServerID eServerID;
    switch (m_stWorldMsg.m_stmsghead().m_uimsgid())
    {
        //todo jasonxiong 后续考虑进行修改
        /*
        case MSGID_FETCHROLELIST_REQUEST:
        case MSGID_FETCHROLEALLINFOREQUEST:
        case MSGID_DODELETEITEMREQUEST:
        case MSGID_MODIFYROLEPOSREQUEST:
        case MSGID_THINGPROPERTYCHANGED_NOITFY:
        case MSGID_TITLE_OPERATE_REQUEST:
        case MSGID_DELETEPETREQUEST:
        case MSGID_QUERYUNLOCKBAGSTATREQUEST:
        {
            eServerID = GAME_SERVER_ROLEDB;
            break;
        }
        */
        
        case MSGID_ACCOUNT_CREATEROLE_RESPONSE:
        {
            eServerID = GAME_SERVER_ACCOUNT;
            break;
        }

        //todo jasonxiong 后续重新开发
        /*
        case MSGID_FETCHROLELIST_RESPONSE:
        case MSGID_FETCHROLEALLINFORESPONSE:
        case MSGID_DOSENDITEMRESPONSE:
        case MSGID_PAYRESPONSE:
        case MSGID_DODELETEITEMRESPONSE:
        case MSGID_MODIFYROLEPOSRESPONSE:
        case MSGID_FORBIDROLERESPONSE:
        case MSGID_FORBIDTALKINGRESPONSE:
        case MSGID_TITLE_OPERATE_RESPONSE:
        case MSGID_DELETEPETRESPONSE:
        case MSGID_QUERYUNLOCKBAGSTATRESPONSE:
        case MSGID_QUERYMAILRESPONSE:
        case MSGID_DELETEMAILRESPONSE:
        case MSGID_RECHARGE4LOTTERY_REQUEST:
        {
            eServerID = GAME_SERVER_CLUSTER;
            break;
        }
        // Zone <==> MailDB
        case MSGID_MAIL_MAILNUMBER_REQUEST:
        case MSGID_MAIL_FETCHLIST_REQUEST:
        case MSGID_MAIL_FETCHMAIL_REQUEST:
        case MSGID_MAIL_FETCHACCESSORY_REQ:
        case MSGID_MAIL_PICKACCESSORY_RESPONSE:
        case MSGID_MAIL_SENDMAIL_REQUEST:
        case MSGID_MAIL_DELETEMAIL_REQUEST:
        case MSGID_QUERYMAILREQUEST:
        case MSGID_DELETEMAILREQUEST:
        {
            eServerID = KYLIN_SERVER_MAILDB;
            break;
        }
        */

        default:
        {
            LOGERROR("Unknown msg id %d..........................\n", m_stWorldMsg.m_stmsghead().m_uimsgid());
            return -1;
        }
    }
    if (eServerID != GAME_SERVER_ZONE)
    {
        return m_pAppLoop->SendWorldMsg(iMsgLength, pszMsgBuffer, eServerID);
    }
    else
    {
        //todo jasonxiong 目前没有使用feedback字段的方式，后面统一再考虑
        //int* pFeedback = GetMsgFeedbackPointer(pszMsgBuffer);
        //int iFeedback = ntohl(*pFeedback);
        //return m_pAppLoop->SendWorldMsg(iMsgLength, pszMsgBuffer, eServerID, iFeedback);
        ;
    }

    return 0;
}

int CWorldProtocolEngineW::SendWorldMsg(const GameProtocolMsg& rstWorldMsg, EGameServerID enMsgPeer, int iInstance)
{
    int iBuffLen = 0;
    char* pBuff = NULL;
    unsigned short ushTotalLength = 0;
    if(enMsgPeer == GAME_SERVER_ROLEDB)
    {
        //2字节长度
        ushTotalLength = rstWorldMsg.ByteSize()+ sizeof(unsigned short) + sizeof(unsigned int);
        m_szCode[0] = ushTotalLength / 256;
        m_szCode[1] = ushTotalLength % 256;

        //如果是RoleDB加4字节uin
        *((unsigned*)(&m_szCode[sizeof(unsigned short)])) = rstWorldMsg.m_stmsghead().m_uin();

        //数据
        iBuffLen = sizeof(m_szCode) - sizeof(unsigned short) - sizeof(unsigned int);
        pBuff = &m_szCode[sizeof(unsigned short) + sizeof(unsigned int)];
    }
    else
    {
        //2字节长度
        ushTotalLength = rstWorldMsg.ByteSize()+ sizeof(unsigned short);
        m_szCode[0] = ushTotalLength / 256;
        m_szCode[1] = ushTotalLength % 256;

        //数据
        iBuffLen = sizeof(m_szCode) - sizeof(unsigned short);
        pBuff = &m_szCode[sizeof(unsigned short)];
    }

    bool bRet = rstWorldMsg.SerializeToArray(pBuff, iBuffLen);
    if(!bRet)
    {
        LOGERROR("fail to parse world proto msg to array!\n");
        return -1;
    }

    if(!m_pAppLoop)
    {
        return -1;
    }

    return m_pAppLoop->SendWorldMsg(ushTotalLength, m_szCode, enMsgPeer, iInstance);
}

void CWorldProtocolEngineW::SetService(CAppLoopW* pAppLoop)
{
    m_pAppLoop = pAppLoop;
}

int CWorldProtocolEngineW::RegisterAllHandler()
{
    // RoleDB
    CHandlerFactory::RegisterHandler(MSGID_WORLD_FETCHROLE_RESPONSE, &m_stFetchRoleWorldHandler);
    CHandlerFactory::RegisterHandler(MSGID_WORLD_FETCHROLE_REQUEST, &m_stFetchRoleWorldHandler);

    CHandlerFactory::RegisterHandler(MSGID_WORLD_UPDATEROLE_RESPONSE, &m_stUpdateRoleWorldHandler);
    CHandlerFactory::RegisterHandler(MSGID_WORLD_UPDATEROLE_REQUEST, &m_stUpdateRoleWorldHandler);

    //CHandlerFactory::RegisterHandler(MSGID_GAMEMASTERDB_GRANT_REQUEST, &m_stGameMasterHandler);
    //CHandlerFactory::RegisterHandler(MSGID_GAMEMASTERDB_GRANT_RESPONSE, &m_stGameMasterHandler);

    // Account
    CHandlerFactory::RegisterHandler(MSGID_ACCOUNT_LISTROLE_REQUEST, &m_stListRoleAccountHandler);
    CHandlerFactory::RegisterHandler(MSGID_ACCOUNT_LISTROLE_RESPONSE, &m_stListRoleAccountHandler);

    CHandlerFactory::RegisterHandler(MSGID_ACCOUNT_CREATEROLE_REQUEST, &m_stCreateRoleAccountHandler);
    CHandlerFactory::RegisterHandler(MSGID_WORLD_CREATEROLE_RESPONSE, &m_stCreateRoleAccountHandler);

    CHandlerFactory::RegisterHandler(MSGID_ACCOUNT_DELETEROLE_REQUEST, &m_stDeleteRoleAccountHandler);
    CHandlerFactory::RegisterHandler(MSGID_ACCOUNT_DELETEROLE_RESPONSE, &m_stDeleteRoleAccountHandler);

    // NameDB
    CHandlerFactory::RegisterHandler(MSGID_ADDNEWNAME_REQUEST, &m_stWorldNameHandler);
    CHandlerFactory::RegisterHandler(MSGID_ADDNEWNAME_RESPONSE, &m_stWorldNameHandler);
    CHandlerFactory::RegisterHandler(MSGID_DELETENAME_REQUEST, &m_stWorldNameHandler);

    // LoginOut
    CHandlerFactory::RegisterHandler(MSGID_ZONE_LOGOUT_NOTIFY, &m_stLogoutHandler);
    CHandlerFactory::RegisterHandler(MSGID_WORLD_KICKROLE_REQUEST, &m_stKickRoleWorldHandler);
    CHandlerFactory::RegisterHandler(MSGID_WORLD_KICKROLE_RESPONSE, &m_stKickRoleWorldHandler);

    //CHandlerFactory::RegisterHandler(MSGID_KICKROLE_CLUSTER_REQUEST, &m_stKickRoleWorldHandler);
    //CHandlerFactory::RegisterHandler(MSGID_KICKROLE_CLUSTER_RESPONSE, &m_stKickRoleWorldHandler);

    // Chat
    CHandlerFactory::RegisterHandler(MSGID_WORLD_CHAT_NOTIFY, &m_stChatHandler);

    //拉取Zone列表
    CHandlerFactory::RegisterHandler(MSGID_ACCOUNT_LISTZONE_REQUEST, &m_stListZoneHandler);

    // Mail
    //CHandlerFactory::RegisterHandler(MSGID_MAIL_MAILNUMBER_NOTIFY, &m_stMailMsgHandler);
    //CHandlerFactory::RegisterHandler(MSGID_MAIL_FETCHLIST_RESPONSE, &m_stMailMsgHandler);
    //CHandlerFactory::RegisterHandler(MSGID_MAIL_FETCHMAIL_RESPONSE, &m_stMailMsgHandler);
    //CHandlerFactory::RegisterHandler(MSGID_MAIL_FETCHACCESSORY_RES, &m_stMailMsgHandler);
    //CHandlerFactory::RegisterHandler(MSGID_MAIL_SENDMAIL_RESPONSE, &m_stMailMsgHandler);
   // CHandlerFactory::RegisterHandler(MSGID_MAIL_LISTCHANGE_NOTIFY, &m_stMailMsgHandler);

    // Friend
    /*
    CHandlerFactory::RegisterHandler(MSGID_FRIEND_FETCHGAMEFRIEND_REQUEST, &m_stFriendMsgHandler);
    CHandlerFactory::RegisterHandler(MSGID_FRIEND_FETCHGAMEFRIEND_RESPONSE, &m_stFriendMsgHandler);
    CHandlerFactory::RegisterHandler(MSGID_FRIEND_ADD_REQUEST, &m_stFriendMsgHandler);
    CHandlerFactory::RegisterHandler(MSGID_FRIEND_ADD_RESPONSE, &m_stFriendMsgHandler);
    CHandlerFactory::RegisterHandler(MSGID_FRIEND_ADD_NOTIFY, &m_stFriendMsgHandler);
    CHandlerFactory::RegisterHandler(MSGID_FRIEND_ROLEINFO_REQUEST, &m_stFriendMsgHandler);
    CHandlerFactory::RegisterHandler(MSGID_FRIEND_ROLEINFO_RESPONSE, &m_stFriendMsgHandler);
    CHandlerFactory::RegisterHandler(MSGID_FRIEND_UPDATEGAMEFRIEND_REQUEST, &m_stFriendMsgHandler);
    CHandlerFactory::RegisterHandler(MSGID_FRIEND_LEVELCHANGE_NOTIFY, &m_stFriendMsgHandler);
    CHandlerFactory::RegisterHandler(MSGID_FRIEND_LOGINOUT_NOTIFY, &m_stFriendMsgHandler);
    CHandlerFactory::RegisterHandler(MSGID_SOCIAL_QUERY_REQUEST, &m_stFriendMsgHandler);
    CHandlerFactory::RegisterHandler(MSGID_SOCIAL_QUERY_RESPONSE, &m_stFriendMsgHandler);
    CHandlerFactory::RegisterHandler(MSGID_SOCIAL_CHECK_REQUEST, &m_stFriendMsgHandler);
    CHandlerFactory::RegisterHandler(MSGID_SOCIAL_CHECK_RESPONSE, &m_stFriendMsgHandler);
    CHandlerFactory::RegisterHandler(MSGID_FRIEND_SEARCH_REQUEST, &m_stFriendMsgHandler);
    CHandlerFactory::RegisterHandler(MSGID_FRIEND_CONTACT_REQUEST, &m_stFriendMsgHandler);
    CHandlerFactory::RegisterHandler(MSGID_CLOSERFRIENDUPDATE_NOTIFY, &m_stFriendMsgHandler);
    */

    // Online
    CHandlerFactory::RegisterHandler(MSGID_ZONE_ONLINEROLENUM_REQUEST, &m_OnlineStatHandler);
    //CHandlerFactory::RegisterHandler(MSGID_WORLD_ONLINESTAT_RESPONSE, &m_OnlineStatHandler);

    // Msg
    //CHandlerFactory::RegisterHandler(MSGID_PROMPTMSG_NOTIFY, &m_stBroadcastMsgHandler);
    //CHandlerFactory::RegisterHandler(MSGID_GAMEMASTER_REQUEST, &m_stBroadcastMsgHandler);
    //CHandlerFactory::RegisterHandler(MSGID_BOSSSTATUSCHANGED_NOTIFY, &m_stBroadcastMsgHandler);
    //CHandlerFactory::RegisterHandler(MSGID_PINSTANCEDRAWLUCKY_NOTIFY, &m_stBroadcastMsgHandler);

    // Charge
    //CHandlerFactory::RegisterHandler(MSGID_CHARGE_BALANCE_REQUEST, &m_stChargeHandler);
    //CHandlerFactory::RegisterHandler(MSGID_CHARGE_BALANCE_RESPONSE, &m_stChargeHandler);
    //CHandlerFactory::RegisterHandler(MSGID_CHARGE_DEDUCT_REQUEST, &m_stChargeHandler);
    //CHandlerFactory::RegisterHandler(MSGID_CHARGE_DEDUCT_RESPONSE, &m_stChargeHandler);

    // Activity
    //CHandlerFactory::RegisterHandler(MSGID_ROLETODAYACTIVITY_REQUEST, &m_stTodayActivityHandler);
    //CHandlerFactory::RegisterHandler(MSGID_ACTIVITYRANDOMROLE_REQUEST, &m_stTodayActivityHandler);

    return 0;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
