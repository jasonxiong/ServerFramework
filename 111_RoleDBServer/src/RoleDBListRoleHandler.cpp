#include <string.h>

#include "RoleDBLogManager.hpp"
#include "SeqConverter.hpp"
#include "StringUtility.hpp"
#include "RoleDBApp.hpp"
#include "ModuleHelper.hpp"
#include "AppDef.hpp"

#include "RoleDBListRoleHandler.hpp"

//SQL语句字符串缓冲区
char CRoleDBListRoleHandler::m_szQueryString[GameConfig::ROLE_TABLE_SPLIT_FACTOR][256];

CRoleDBListRoleHandler::CRoleDBListRoleHandler(DBClientWrapper* pDatabase)
{    
    m_pDatabase = pDatabase;
}

void CRoleDBListRoleHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg,
        SHandleResult* pstHandleResult)
{
    if (!pstRequestMsg || !pstHandleResult)
	{
		return;
	}

    // list role请求消息
    m_pstRequestMsg = pstRequestMsg;
	int iRet = -1;
	switch(m_pstRequestMsg->m_stmsghead().m_uimsgid())
	{
		case MSGID_ACCOUNT_LISTROLE_REQUEST:
			{
				iRet = AccountListRole(pstHandleResult);
				break;
			}
				
		default:
			{
				break;
			}			
	}

	DEBUG_THREAD(m_iThreadIdx, "MsgId:%d, iRet:%d\n" , m_pstRequestMsg->m_stmsghead().m_uimsgid(), iRet);
}

int CRoleDBListRoleHandler::AccountListRole(SHandleResult* pstHandleResult)
{
	Account_ListRole_Request* pstReq = m_pstRequestMsg->mutable_m_stmsgbody()->mutable_m_staccountlistrolerequest();
	unsigned int uiUin = pstReq->uin();
	unsigned short usWorldID = pstReq->world();

    short usNewWorldID = static_cast<short>(CModuleHelper::GetWorldID());

    // list role响应消息
    GameProtocolMsg* pstResponseMsg = &(pstHandleResult->stResponseMsg);

    // 响应消息头
    GenerateResponseMsgHead(pstResponseMsg, 
							m_pstRequestMsg->m_stmsghead().m_uisessionfd(), 
							MSGID_ACCOUNT_LISTROLE_RESPONSE, 
							uiUin);

    // 响应消息体
	Account_ListRole_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staccountlistroleresponse();

    // 根据uin在DBRoleInfo表中检索出seq，解析其中的world信息，返回指定world上的role列表
	int iErrnoNum = T_ROLEDB_SQL_EXECUTE_FAILED;
	int iRes = QueryAndParseRole(uiUin, usWorldID, usNewWorldID, pstResp, iErrnoNum);

	TRACE_THREAD(m_iThreadIdx, "Info of ListRoleResponse: result: %u, uin: %u, world: %u:%u, role number: %d\n",
		iRes, uiUin, usWorldID, usNewWorldID, pstResp->roles_size());

    if (iRes != 0)
    {
        FillFailedResponse(iErrnoNum, pstResponseMsg);
        return -1;
    }

    FillSuccessfulResponse(pstResponseMsg);
	return 0;
}

int CRoleDBListRoleHandler::QueryAndParseRole(const unsigned int uiUin, short nWorldID, short nNewWorldID,
        Account_ListRole_Response* pstListRoleResponse, int& iErrnoNum)
{
    if (!pstListRoleResponse)
	{
		return -1;
	}

	//查询DB中是否已经存在玩家帐号

	//读取ROLEDB相关的配置
	const ONEROLEDBINFO* pstDBConfig = (CRoleDBApp::m_stRoleDBConfigManager).GetOneRoleDBInfoByIndex(m_iThreadIdx);
	if(!pstDBConfig)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to get roledb config, invalid index %d\n", m_iThreadIdx);
		return -2;
	}

	//设置要访问的数据库信息
	m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);

	//构造SQL语句
	char* pszQueryString = m_szQueryString[m_iThreadIdx];
	static int iQueryStringLen = sizeof(m_szQueryString[m_iThreadIdx])-1;
	int iLength = 0;

	//1服用作开发服，2服用作预发布服,这两个服可能会从外网导入数据
	if(nNewWorldID==1 || nNewWorldID==2)
	{
		//开发服和预发布服，会从外网导数据进来
		iLength = SAFE_SPRINTF(pszQueryString, iQueryStringLen, 
							   "select uin,seq from %s where uin=%u\n", 
							   MYSQL_USERINFO_TABLE, uiUin);
	}
	else
	{
		//外网正式服，考虑合服的情况
		iLength = SAFE_SPRINTF(pszQueryString, iQueryStringLen, 
							   "select uin,seq from %s where uin = %u and floor(seq%%10000/10)=%d\n", 
							   MYSQL_USERINFO_TABLE, uiUin, nWorldID);
	}

	//执行
	int iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, true);
	if(iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to execute select sql query, uin %u\n", uiUin);
		return iRet;
	}

	//分析返回结果
	int iRowNum = m_pDatabase->GetNumberRows();
	if(iRowNum == 0)
	{
		//不存在玩家记录，直接返回成功
		return T_SERVER_SUCESS;
	}
	
	TRACE_THREAD(m_iThreadIdx, "List Role Num %d, uin %u, world id %d\n", iRowNum, uiUin, nWorldID);

	//存在玩家数据记录，封装数据并返回
	MYSQL_ROW pstResult = NULL;
	unsigned long* pLengths = NULL;
	unsigned int uFields = 0;
	for(int i=0; i<iRowNum; ++i)
	{
		iRet = m_pDatabase->FetchOneRow(pstResult, pLengths, uFields);
		if(iRet)
		{
			TRACE_THREAD(m_iThreadIdx, "Failed to fetch rows, uin %u, ret %d\n", uiUin, iRet);
			return iRet;
		}

		//判断列数是否相符
		if(uFields < 2)
		{
			TRACE_THREAD(m_iThreadIdx, "Wrong result, real fields %u, needed %u\n", uFields, 3);
			return -1;
		}

		//从结果中解析需要的字段
		RoleSummary* pstRoleSummary = pstListRoleResponse->add_roles();
		pstRoleSummary->mutable_stroleid()->set_uin(uiUin);

		//字段0是uin

		//字段1是seq
		unsigned int uRoleSeq = (unsigned)atoi(pstResult[1]);
		pstRoleSummary->mutable_stroleid()->set_uiseq(uRoleSeq);
	}

    return T_SERVER_SUCESS;
}

void CRoleDBListRoleHandler::FillFailedResponse(const unsigned int uiResultID,
        GameProtocolMsg* pstResponseMsg)
{
    Account_ListRole_Response* pstListRoleResponse = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staccountlistroleresponse();
	pstListRoleResponse->set_iresult(uiResultID);
}

void CRoleDBListRoleHandler::FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg)
{
	Account_ListRole_Response* pstListRoleResponse = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staccountlistroleresponse();

	pstListRoleResponse->set_iresult(T_SERVER_SUCESS);
	
	return;
}

