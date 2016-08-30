#include <stdlib.h>
#include <string.h>

#include "RoleDBApp.hpp"
#include "RoleDBLogManager.hpp"
#include "StringUtility.hpp"
#include "RoleDBListRoleHandler.hpp"

#include "FetchFriendInfoHandler.hpp"

using namespace ServerLib;

//todo jasonxiong2 这个等到实际开发好友时再来开发

CFetchFriendInfoHandler::CFetchFriendInfoHandler(DBClientWrapper* pDatabase)
{
    m_pDatabase = pDatabase;
}

void CFetchFriendInfoHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg,
                                          SHandleResult* pstHandleResult)
{
    if (!pstRequestMsg || !pstHandleResult)
    {
        return;
    }

    // fetch friend info请求消息
    m_pstRequestMsg = pstRequestMsg;

    const Zone_GetFriendInfo_Request& stReq = m_pstRequestMsg->m_stmsgbody().m_stzone_getfriendinfo_request();;
    unsigned int uiUin = stReq.uin();
    unsigned int uiFriendUin = stReq.friend_uin();

    DEBUG_THREAD(m_iThreadIdx, "Handling FetchFriendInfoRequest, operator uin: %u, "
        "target uin: %u\n", uiUin, uiFriendUin);

    // fetch friend info响应消息
    GameProtocolMsg* pstRespMsg = &pstHandleResult->stResponseMsg;

    // 响应消息头
    GenerateResponseMsgHead(pstRespMsg, 0, MSGID_ZONE_FRIENDINFO_RESPONSE, uiUin);

    // 响应消息体
    Zone_GetFriendInfo_Response* pstResp = pstRespMsg->mutable_m_stmsgbody()->mutable_m_stzone_getfriendinfo_response();

    // 根据role id查询该角色的信息
    int iRes = QueryFriendInfo(uiUin, pstResp);
    if (iRes != 0)
    {
        FillFailedResponse(iRes, pstRespMsg);
        return;
    }

    // 填充成功回复
    FillSuccessfulResponse(pstRespMsg);
}

int CFetchFriendInfoHandler::QueryFriendInfo(unsigned int uin, Zone_GetFriendInfo_Response* pstResponse)
{
    if(!pstResponse)
    {
        return -3;
    }

    //todo jasonxiong 后续实际拉取好友消息处理时，需要重新实现这个函数

    /*

    char* pszSelectSqlString = "select base_info from t_magic_userdata where uin = %u";


    DBROLEINFO stObj;
    char szObjMetaName[] = "DBRoleInfo";
    int iObjSize = sizeof(DBROLEINFO);
    char szWhere[1024] = "";

    memset(&stObj, 0, iObjSize);
    stObj.fUin = pstRoleID->m_uiUin; // 主键
    stObj.fSeq = pstRoleID->m_uiSeq; // 主键

    int iRet = m_pDatabase->Execute(&stObj, iObjSize, szObjMetaName, TDR_DBOP_SELECT, szWhere);
    if (iRet != 0)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute db query\n");
        return EQEC_RoleDB_SqlExecuteFailed;
    }

    long lNumberRows = m_pDatabase->GetNumberRows();
    if (1 != lNumberRows)
    {
        TRACE_THREAD(m_iThreadIdx, "The number of rows is %ld\n", lNumberRows);
        return EQEC_RoleDB_RoleNotFound;
    }

    memset(&stObj, 0, iObjSize);
    iRet = m_pDatabase->FetchRows(&stObj, iObjSize, lNumberRows, szObjMetaName);
    if (iRet != 0)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to fetch select result\n");
        return EQEC_RoleDB_SqlExecuteFailed;
    }

    // role info
    pstResponse->m_stRoleInfo.m_iActiveTitleConfigID = 
        stObj.fGloryInfo.m_stTitleList.m_stOwnedTitleList.m_iActiveTitleConfigID;

*/
    return 0;
}

void CFetchFriendInfoHandler::FillFailedResponse(const unsigned int uiResultID,
                                                 GameProtocolMsg* pstResponseMsg)
{
    Zone_GetFriendInfo_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stzone_getfriendinfo_response();

    pstResp->set_iresult(uiResultID);

    DEBUG_THREAD(m_iThreadIdx, "Info of failed FetchFriendInfoResponse: result: %d\n",
        pstResp->iresult());
}

void CFetchFriendInfoHandler::FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg)
{
    Zone_GetFriendInfo_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stzone_getfriendinfo_response();

    pstResp->set_iresult(T_SERVER_SUCESS);

    return;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
