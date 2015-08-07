#include <string.h>
#include "RoleDBLogManager.hpp"
#include "StringUtility.hpp"
#include "RoleDBApp.hpp"
#include "AppDef.hpp"

#include "RoleDBUpdateRoleHandler.hpp"

using namespace ServerLib;

//SQL语句字符串缓冲区
char CRoleDBUpdateRoleHandler::m_szQueryString[GameConfig::ROLE_TABLE_SPLIT_FACTOR][51200];

CRoleDBUpdateRoleHandler::CRoleDBUpdateRoleHandler(DBClientWrapper* pDatabase)
{ 
    m_pDatabase = pDatabase;
}

void CRoleDBUpdateRoleHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg,
        SHandleResult* pstHandleResult)
{
    if (!pstRequestMsg || !pstHandleResult)
	{
		return;
	}

    // update role请求消息
    m_pstRequestMsg = pstRequestMsg;

    switch (pstRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_WORLD_UPDATEROLE_REQUEST:
        {
            OnUpdateRoleRequest(pstHandleResult);
            break;
        }

    default:
        {
            break;
        }
    }
}

void CRoleDBUpdateRoleHandler::OnUpdateRoleRequest(SHandleResult* pstHandleResult)
{
	World_UpdateRole_Request* pstReq = m_pstRequestMsg->mutable_m_stmsgbody()->mutable_m_stworld_updaterole_request();

    TRACE_THREAD(m_iThreadIdx, "Handling UpdateRoleRequest, uin: %u"
             "need response: %d\n", 
		pstReq->stroleid().uin(), pstReq->bneedresponse());

    pstHandleResult->iNeedResponse = pstReq->bneedresponse(); // 是否需要回复

    // update role响应消息
    GameProtocolMsg* pstResponseMsg = &(pstHandleResult->stResponseMsg);

    // 响应消息头
    GenerateResponseMsgHead(pstResponseMsg, 0, MSGID_WORLD_UPDATEROLE_RESPONSE, pstReq->stroleid().uin());

    // 响应消息体
	World_UpdateRole_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stworld_updaterole_response();
	pstResp->mutable_stroleid()->CopyFrom(pstReq->stroleid());

	int iRes = UpdateRole(pstReq);
	if (iRes != 0)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to execute db query\n");
		FillFailedResponse(T_ROLEDB_SQL_EXECUTE_FAILED, pstResponseMsg);
		return;
	}

    FillSuccessfulResponse(pstResponseMsg);
}

int CRoleDBUpdateRoleHandler::UpdateRole(const World_UpdateRole_Request* pstUpdateRoleRequest)
{
    if (!pstUpdateRoleRequest)
	{
		return -1;
	}

    //获取连接的数据库相关的配置
    const ONEROLEDBINFO* pstDBConfig = (CRoleDBApp::m_stRoleDBConfigManager).GetOneRoleDBInfoByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get roledb config, invalid thread index %d\n", m_iThreadIdx);
        return -2;
    }

    //设置要操作的数据库相关信息
    m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);

    //初始化SQL语句
    int iLength = 0;
    char* pszQueryString = m_szQueryString[m_iThreadIdx];
    int iRet = GenerateQueryString(*pstUpdateRoleRequest, pszQueryString, sizeof(m_szQueryString[m_iThreadIdx])-1, iLength);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to generate update query sql string, ret %d\n", iRet);
        return iRet;
    }

    //执行
    iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Fail to execute select sql query, uin %u\n", pstUpdateRoleRequest->stroleid().uin());
        return iRet;
    }

	DEBUG_THREAD(m_iThreadIdx, "The number of affected rows is %d\n", m_pDatabase->GetAffectedRows());

	return 0;
}

int CRoleDBUpdateRoleHandler::GenerateQueryString(const World_UpdateRole_Request& rstUpdateReq, char* pszBuff, int iBuffLen, int& iLength)
{
    if(!pszBuff)
    {
        TRACE_THREAD(m_iThreadIdx, "Fail to generate update query string, invlaid buff pointer!\n");
        return T_ROLEDB_SYSTEM_PARA_ERR;
    }

    memset(pszBuff, 0, iBuffLen);

    SAFE_SPRINTF(pszBuff, iBuffLen-1, "update %s set ", MYSQL_USERINFO_TABLE);
    char* pEnd = pszBuff + strlen(pszBuff);

    MYSQL& stDBConn = m_pDatabase->GetCurMysqlConn();

    const GameUserInfo& rstUserInfo = rstUpdateReq.stuserinfo();

    //1.玩家基本信息
    if(rstUserInfo.strbaseinfo().size() != 0)
    {
        SAFE_SPRINTF(pEnd, iBuffLen-1, "base_info=");
        pEnd += strlen("base_info=");

        //玩家基本信息 base_info
        *pEnd++ = '\'';
        pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strbaseinfo().c_str(), rstUserInfo.strbaseinfo().size());
        *pEnd++ = '\'';
    }

    //2.玩家任务信息
    if(rstUserInfo.strquestinfo().size() != 0)
    {
        *pEnd++ = ',';

        SAFE_SPRINTF(pEnd, iBuffLen-1, "quest_info=");

        pEnd += strlen("quest_info=");

        //玩家的任务信息字段
        *pEnd++ = '\'';
        pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strquestinfo().c_str(), rstUserInfo.strquestinfo().size());
        *pEnd++ = '\'';
    }

    //3.玩家的物品信息
    if(rstUserInfo.striteminfo().size() != 0)
    {
        *pEnd++ = ',';

        SAFE_SPRINTF(pEnd, iBuffLen-1, "item_info=");

        pEnd += strlen("item_info=");

        //玩家的物品信息 item_info
        *pEnd++ = '\'';
        pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.striteminfo().c_str(), rstUserInfo.striteminfo().size());
        *pEnd++ = '\'';
    }

    //4.玩家的战斗信息
    if(rstUserInfo.strfightinfo().size() != 0)
    {
        *pEnd++ = ',';

        SAFE_SPRINTF(pEnd, iBuffLen-1, "fight_info=");

        pEnd += strlen("fight_info=");

        //玩家的战斗信息
        *pEnd++ = '\'';
        pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strfightinfo().c_str(), rstUserInfo.strfightinfo().size());
        *pEnd++ = '\'';
    }

    //5.玩家的好友信息
    if(rstUserInfo.strfriendinfo().size() != 0)
    {
        *pEnd++ = ',';

        SAFE_SPRINTF(pEnd, iBuffLen-1, "friend_info=");

        pEnd += strlen("friend_info=");

        //玩家的好友信息
        *pEnd++ = '\'';
        pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strfriendinfo().c_str(), rstUserInfo.strfriendinfo().size());
        *pEnd++ = '\'';
    }

    //6.玩家的保留字段1
    if(rstUserInfo.strreserved1().size() != 0)
    {        
        *pEnd++ = ',';

        SAFE_SPRINTF(pEnd, iBuffLen-1, "reserved1=");

        pEnd += strlen("reserved1=");

        //保留字段1的更新
        *pEnd++ = '\'';
        pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strreserved1().c_str(), rstUserInfo.strreserved1().size());
        *pEnd++ = '\'';
    }

    //7.玩家的保留字段2
    if(rstUserInfo.strreserved2().size() != 0)
    {        
        *pEnd++ = ',';

        SAFE_SPRINTF(pEnd, iBuffLen-1, "reserved2=");

        pEnd += strlen("reserved2=");

        //保留字段2的更新
        *pEnd++ = '\'';
        pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strreserved2().c_str(), rstUserInfo.strreserved2().size());
        *pEnd++ = '\'';
    }

    pEnd += SAFE_SPRINTF(pEnd, iBuffLen-1, " where uin = %u and seq = %u\n", rstUpdateReq.stroleid().uin(), rstUpdateReq.stroleid().uiseq());
    
    iLength = pEnd - pszBuff;

    return T_SERVER_SUCESS;
}

void CRoleDBUpdateRoleHandler::FillFailedResponse(const unsigned int uiResultID,
        GameProtocolMsg* pstResponseMsg)
{
	World_UpdateRole_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stworld_updaterole_response();
	pstResp->set_iresult(uiResultID);

    TRACE_THREAD(m_iThreadIdx, "Info of failed UpdateRoleResponse: result: %d\n", uiResultID);

	return;
}

void CRoleDBUpdateRoleHandler::FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg)
{
	World_UpdateRole_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stworld_updaterole_response();
	pstResp->set_iresult(T_SERVER_SUCESS);

    TRACE_THREAD(m_iThreadIdx, "Info of UpdateRoleResponse: result: %d, uin: %u\n",
             T_SERVER_SUCESS, pstResp->stroleid().uin());

	return;
}
