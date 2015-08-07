#include <string.h>

#include "AppDef.hpp"
#include "NameDBLogManager.hpp"
#include "StringUtility.hpp"
#include "NameDBApp.hpp"

#include "DeleteNameHandler.hpp"

using namespace ServerLib;

//执行的SQL语句
char CDeleteNameHandler::m_szQueryString[GameConfig::NAME_TABLE_SPLIT_FACTOR][256];

CDeleteNameHandler::CDeleteNameHandler(DBClientWrapper* pDatabase)
{
    m_pDatabase = pDatabase;
}

void CDeleteNameHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg,
        SHandleResult* pstHandleResult)
{
    if (!pstRequestMsg || !pstHandleResult)
	{
		return;
	}

    //Delete请求不需要回包
    pstHandleResult->iNeedResponse = false;

    //Delete Name 的请求消息
    m_pstRequestMsg = pstRequestMsg;
    const DeleteName_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_stdeletename_request();

    TRACE_THREAD(m_iThreadIdx, "Handling DeleteNameRequest, name: %s, type %d\n", rstReq.strname().c_str(), rstReq.itype());

    // 响应消息头
    GenerateResponseMsgHead(&pstHandleResult->stResponseMsg, 
                            m_pstRequestMsg->m_stmsghead().m_uisessionfd(),
                            MSGID_DELETENAME_RESPONSE, 
                            0);

    //根据NameID和NameType删除记录
    int iRet = DeleteName(rstReq.strname(), rstReq.itype());
    if (iRet)
    {
        FillFailedResponse(iRet, &pstHandleResult->stResponseMsg);
        return;
    }

    FillSuccessfulResponse(&pstHandleResult->stResponseMsg);

    return;
}

//删除Name
int CDeleteNameHandler::DeleteName(const std::string& strName, int iType)
{
    //拉取NAMEDB数据库相关配置
    const ONENAMEDBINFO* pstDBConfig = (CNameDBApp::m_stNameDBConfigManager).GetOneNameDBInfoByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get name db config, thread index %d\n", m_iThreadIdx);
        return -1;
    }

    //设置要操作的数据库相关信息
    m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);

    char* pszQueryString = m_szQueryString[m_iThreadIdx];

    //生成删除的SQL语句
    int iLength = SAFE_SPRINTF(pszQueryString, sizeof(m_szQueryString[m_iThreadIdx])-1, "delete from %s where nickname= '%s' and nametype=%d", 
                 MYSQL_NAMEINFO_TABLE, strName.c_str(), iType);

    int iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute delete sql query, name %s, type %d, ret %d\n", strName.c_str(), iType, iRet);
        return iRet;
    }

    DEBUG_THREAD(m_iThreadIdx, "The number of affected rows is %d\n", m_pDatabase->GetAffectedRows());

    return 0;
}

void CDeleteNameHandler::FillFailedResponse(const unsigned int uiResultID,
        GameProtocolMsg* pstResponseMsg)
{
	if (!pstResponseMsg)
	{
		return;
	}
    
    DeleteName_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stdeletename_response();
    pstResp->set_iresult(uiResultID);

    TRACE_THREAD(m_iThreadIdx, "Info of failed DeleteNameResponse: result: %d\n", uiResultID);
}

void CDeleteNameHandler::FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg)
{
	if (!pstResponseMsg)
	{
		return;
	}

    DeleteName_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stdeletename_response();
    pstResp->set_iresult(T_SERVER_SUCESS);

    TRACE_THREAD(m_iThreadIdx, "Info of DeleteNameResponse: result: %d\n", T_SERVER_SUCESS);

    return;
}
