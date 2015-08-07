#include "GameConfigDefine.hpp"
#include "HandlerFactory.hpp"
#include "HandlerList.hpp"
#include "ModuleHelper.hpp"

int CHandlerList::RegisterAllHandler()
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DB拉取更新
    CHandlerFactory::RegisterHandler(MSGID_WORLD_FETCHROLE_RESPONSE, &m_stFetchRoleWorldHandler);
    CHandlerFactory::RegisterHandler(MSGID_WORLD_UPDATEROLE_RESPONSE,&m_stUpdateRoleInfoHandler);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 登录登出
    CHandlerFactory::RegisterHandler(MSGID_ZONE_LOGINSERVER_REQUEST, &m_stLoginHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    //CHandlerFactory::RegisterHandler(MSGID_LOGINSERVER_NOTIFY, &m_stLoginHandler);
    CHandlerFactory::RegisterHandler(MSGID_LOGOUTSERVER_REQUEST, &m_stLogoutHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    //CHandlerFactory::RegisterHandler(MSGID_LOGOUTSERVER_NOTIFY, &m_stLogoutHandler);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 踢人封号
    CHandlerFactory::RegisterHandler(MSGID_WORLD_KICKROLE_REQUEST, &m_stKickRoleWorldHandler);
    CHandlerFactory::RegisterHandler(MSGID_WORLD_KICKROLE_RESPONSE, &m_stKickRoleWorldHandler);
    //CHandlerFactory::RegisterHandler(MSGID_FORBIDROLEREQUEST, &m_stKickRoleWorldHandler);

    //GM命令执行
    CHandlerFactory::RegisterHandler(MSGID_ZONE_GAMEMASTER_REQUEST, &m_stGMCommandHandler, EKMT_CLIENT, EKMF_CHECKNONE);

    //物品相关操作
    CHandlerFactory::RegisterHandler(MSGID_ZONE_REPOPERA_REQUEST, &m_stRepThingsHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    CHandlerFactory::RegisterHandler(MSGID_ZONE_GETREPINFO_REQUEST, &m_stRepThingsHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    CHandlerFactory::RegisterHandler(MSGID_ZONE_WEAREQUIP_REQUEST, &m_stRepThingsHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    CHandlerFactory::RegisterHandler(MSGID_ZONE_SELLITEM_REQUEST, &m_stRepThingsHandler, EKMT_CLIENT, EKMF_CHECKNONE);

    //聊天相关
    CHandlerFactory::RegisterHandler(MSGID_ZONE_CHAT_REQUEST, &m_stChatHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    CHandlerFactory::RegisterHandler(MSGID_WORLD_CHAT_NOTIFY, &m_stChatHandler, EKMT_SERVER, EKMF_CHECKNONE);

    //战斗相关
    CHandlerFactory::RegisterHandler(MSGID_ZONE_DOCOMBAT_REQUEST, &m_stCombatHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    CHandlerFactory::RegisterHandler(MSGID_ZONE_COMBATMOVE_REUQEST, &m_stCombatHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    CHandlerFactory::RegisterHandler(MSGID_ZONE_COMBATACTION_REQUEST, &m_stCombatHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    CHandlerFactory::RegisterHandler(MSGID_ZONE_UPDATEPARTNERAI_REQUEST, &m_stCombatHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    CHandlerFactory::RegisterHandler(MSGID_ZONE_STARTPVPCOMBAT_REQUEST, &m_stCombatHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    CHandlerFactory::RegisterHandler(MSGID_ZONE_ACCEPTPVPCOMBAT_REQUEST, &m_stCombatHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    CHandlerFactory::RegisterHandler(MSGID_ZONE_SETCOMBATFORM_REQUEST, &m_stCombatHandler, EKMT_CLIENT, EKMF_CHECKNONE);

	//小伙伴
    //CHandlerFactory::RegisterHandler(MSGID_ZONE_FIGHTUNIT_OPERATION_REQUEST, &m_stFightUnitHandler, EKMT_CLIENT, EKMF_CHECKNONE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 时间同步
    //CHandlerFactory::RegisterHandler(MSGID_SYNCTIME_REQUEST, &m_stSyncTimeHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    //CHandlerFactory::RegisterHandler(MSGID_SYNCTIME_CLIENT_REQUEST, &m_stSyncTimeHandler, EKMT_CLIENT, EKMF_CHECKNONE);

    return 0;
}
