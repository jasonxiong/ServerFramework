#include <stdlib.h>
#include <string.h>

#include "AppDef.hpp"
#include "NameDBLogManager.hpp"
#include "UnixTime.hpp"
#include "NowTime.hpp"
#include "StringUtility.hpp"
#include "NameDBApp.hpp"

#include "AddNameHandler.hpp"

using namespace ServerLib;

//生成的SQL语句
char CAddNameHandler::m_szQueryString[GameConfig::NAME_TABLE_SPLIT_FACTOR][1024];

CAddNameHandler::CAddNameHandler(DBClientWrapper* pDatabase)
{
    m_pDatabase = pDatabase;
}

void CAddNameHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg,
        SHandleResult* pstHandleResult)
{
    if (!pstRequestMsg || !pstHandleResult)
	{
		return;
	}

    // 增加名字记录的请求
    m_pstRequestMsg = pstRequestMsg;

    switch (m_pstRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_ADDNEWNAME_REQUEST:
        {
            OnAddNameRequest(pstHandleResult);
            break;
        }

    default:
        {
            break;
        }
    }

    return;
}

//进行必要的参数检查
int CAddNameHandler::CheckParams()
{
    //检查请求的参数
    const AddNewName_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_staddnewname_request();

    if(rstReq.itype()<=EN_NAME_TYPE_INVALID || rstReq.itype()>=EN_NAME_TYPE_MAX)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to add new name , invalid type %d\n", rstReq.itype());
        return T_COMMON_SYSTEM_PARA_ERR;
    }

    if(rstReq.strname().size() == 0)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to add new name, invalid param!\n");
        return T_COMMON_SYSTEM_PARA_ERR;
    }

    return T_SERVER_SUCESS;
}

void CAddNameHandler::OnAddNameRequest(SHandleResult* pstHandleResult)
{
	if (!pstHandleResult)
	{
		return;
	}

    pstHandleResult->iNeedResponse = true;  //需要回复

    //请求的消息
    const AddNewName_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_staddnewname_request();

    //生成响应的消息头
    const GameCSMsgHead& rstHead = m_pstRequestMsg->m_stmsghead();
    GenerateResponseMsgHead(&pstHandleResult->stResponseMsg, rstHead.m_uisessionfd(), MSGID_ADDNEWNAME_RESPONSE, 0);

    int iRet = CheckParams();
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to add new name request, param check failed, ret %d\n", iRet);
        FillFailedResponse(iRet, rstReq.itype(), rstReq.name_id(), &pstHandleResult->stResponseMsg);

        return;
    }

    //检查帐号是否存在
    bool bIsExist = false;
    iRet = CheckNameExist(rstReq.strname(), rstReq.itype(), bIsExist);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to check name exist, ret 0x%0x\n", iRet);
        FillFailedResponse(iRet, rstReq.itype(), rstReq.name_id(), &pstHandleResult->stResponseMsg);

        return;
    }

    if(bIsExist)
    {
        //帐号已经存在
        TRACE_THREAD(m_iThreadIdx, "Failed to add new name, already exist, ret 0x%0x\n", T_NAMEDB_NAME_EXISTS);
        FillFailedResponse(T_NAMEDB_NAME_EXISTS, rstReq.itype(), rstReq.name_id(), &pstHandleResult->stResponseMsg);

        return;
    }


    //插入新的记录
    iRet = AddNewRecord(rstReq.strname(), rstReq.itype(), rstReq.name_id());
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to add new name record, name %s, type %d, ret %d\n", rstReq.strname().c_str(), rstReq.itype(), iRet);
        FillFailedResponse(iRet, rstReq.itype(), rstReq.name_id(), &pstHandleResult->stResponseMsg);

        return;
    }

    FillSuccessfulResponse(rstReq.strname(), rstReq.itype(), rstReq.name_id(), &pstHandleResult->stResponseMsg);

    return;
}

//检查帐号是否存在
int CAddNameHandler::CheckNameExist(const std::string& strName, int iType, bool& bIsExist)
{
    //设置连接的DB
    const ONENAMEDBINFO* pstDBConfig = (CNameDBApp::m_stNameDBConfigManager).GetOneNameDBInfoByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get name db config, index %d\n", m_iThreadIdx);
        return -1;
    }

    int iRet = m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to set mysql db info, ret %d\n", iRet);
        return iRet;
    }

    char* pszQueryString = m_szQueryString[m_iThreadIdx];
    int iLength = SAFE_SPRINTF(pszQueryString, sizeof(m_szQueryString[m_iThreadIdx])-1, "select nameid from %s where nickname='%s' and nametype=%d", 
                 MYSQL_NAMEINFO_TABLE, strName.c_str(), iType);

    iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, true);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute sql query, ret %d\n", iRet);
        return iRet;
    }

    if(m_pDatabase->GetNumberRows() != 0)
    {
        //已经存在该名字
        bIsExist = true;
    }
    else
    {
        bIsExist = false;
    }

    return T_SERVER_SUCESS;
}

//插入新的记录
int CAddNameHandler::AddNewRecord(const std::string& strName, int iNameType, unsigned uNameID)                                 
{
    //设置连接的DB
    const ONENAMEDBINFO* pstDBConfig = (CNameDBApp::m_stNameDBConfigManager).GetOneNameDBInfoByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get name db config, index %d\n", m_iThreadIdx);
        return -1;
    }

    int iRet = m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to set mysql db info, ret %d\n", iRet);
        return iRet;
    }

    char* pszQueryString = m_szQueryString[m_iThreadIdx];
    int iLength = SAFE_SPRINTF(pszQueryString, sizeof(m_szQueryString[m_iThreadIdx])-1, \
                 "insert into %s(nickname,nametype,nameid) "
                 "values('%s',%d,%u)", 
                 MYSQL_NAMEINFO_TABLE, 
                 strName.c_str(), iNameType, uNameID);

    iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute sql query, ret %d\n", iRet);
        return iRet;
    }

    TRACE_THREAD(m_iThreadIdx, "Success to add new name: %s, type %d\n", strName.c_str(), iNameType);

    return T_SERVER_SUCESS;
}

void CAddNameHandler::FillFailedResponse(const unsigned int uiResultID, int iNameType, unsigned uNameID, GameProtocolMsg* pstResponseMsg)
{
	if (!pstResponseMsg)
	{
		return;
	}

    AddNewName_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staddnewname_response();
    pstResp->set_iresult(uiResultID);
    pstResp->set_itype(iNameType);
    pstResp->set_name_id(uNameID);

    TRACE_THREAD(m_iThreadIdx, "Info of failed AddNewNameResponse: result: %u\n", uiResultID);

    return;
}

void CAddNameHandler::FillSuccessfulResponse(const std::string& strName, int iNameType, unsigned uNameID, GameProtocolMsg* pstResponseMsg)
{
    if(!pstResponseMsg)
    {
        return;
    }

    AddNewName_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staddnewname_response();
    pstResp->set_iresult(T_SERVER_SUCESS);
    pstResp->set_itype(iNameType);
    pstResp->set_name_id(uNameID);
    pstResp->set_strname(strName);

    return;
}



----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
