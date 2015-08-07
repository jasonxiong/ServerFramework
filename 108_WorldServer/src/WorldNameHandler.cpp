#include <string.h>

#include "ProtoDataUtility.hpp"
#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "LogoutHandler.hpp"
#include "LogAdapter.hpp"
#include "HashUtility.hpp"

#include "WorldNameHandler.hpp"

using namespace ServerLib;

GameProtocolMsg CWorldNameHandler::m_stWorldMsg;

static unsigned int GetNameHash(const std::string& strName)
{
    long lHash = CHashUtility::BKDRHash(strName.c_str(), strName.size());

    return (unsigned)lHash;
}

CWorldNameHandler::~CWorldNameHandler()
{
}

int CWorldNameHandler::OnClientMsg(const void* pMsg)
{
	ASSERT_AND_LOG_RTN_INT(pMsg);

	m_pMsg = (GameProtocolMsg*)pMsg;

	switch (m_pMsg->m_stmsghead().m_uimsgid())
	{
	case MSGID_ADDNEWNAME_REQUEST:
		{
			OnRequestAddNewName();
			break;
		}

	case MSGID_ADDNEWNAME_RESPONSE:
		{
			OnResponseAddNewName();
			break;
		}

	case MSGID_DELETENAME_REQUEST:
		{
			OnRequestDeleteName();
			break;
		}

	default:
		{
			break;
		}
	}

	return 0;
}

//更新名字请求的处理
int CWorldNameHandler::OnRequestAddNewName()
{
	AddNewName_Request* pstReq = m_pMsg->mutable_m_stmsgbody()->mutable_m_staddnewname_request();
	unsigned uiUin = pstReq->name_id();

    LOGDEBUG("AddNewName: Uin = %u\n", uiUin);

	//将消息头中的uin替换为name的hash
	m_pMsg->mutable_m_stmsghead()->set_m_uin(GetNameHash(pstReq->strname()));

    //转发更新名字的请求到NameDB
    int iRet = CWorldMsgHelper::SendWorldMsgToNameDB(*m_pMsg);
    if(iRet < 0)
    {
        LOGERROR("AddNewName Failed: iRet = %d\n", iRet);

		SendAddNewNameResponseToAccount(pstReq->strname(), uiUin, T_WORLD_SERVER_BUSY, pstReq->itype());

        return -2;
    }

    return 0;
}

//更新名字返回的处理
int CWorldNameHandler::OnResponseAddNewName()
{
	//直接转发给AccountServer
	const AddNewName_Response& rstResp = m_pMsg->m_stmsgbody().m_staddnewname_response();
	unsigned int uiUin = rstResp.name_id();
	int iResult = rstResp.iresult();

	SendAddNewNameResponseToAccount(rstResp.strname(), uiUin, iResult, rstResp.itype());

	return T_SERVER_SUCESS;
}

int CWorldNameHandler::SendAddNewNameResponseToAccount(const std::string& strName, unsigned int uiUin, int iResult, int iType)
{
    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, m_pMsg->m_stmsghead().m_uisessionfd(), MSGID_ADDNEWNAME_RESPONSE, uiUin);

	AddNewName_Response* pstResp = m_stWorldMsg.mutable_m_stmsgbody()->mutable_m_staddnewname_response();
	pstResp->set_iresult(iResult);
	pstResp->set_name_id(uiUin);
	pstResp->set_strname(strName);
	pstResp->set_itype(iType);
    
    CWorldMsgHelper::SendWorldMsgToAccount(m_stWorldMsg);
	return 0;
}

//删除名字的请求处理
int CWorldNameHandler::OnRequestDeleteName()
{
	DeleteName_Request* pstReq = m_pMsg->mutable_m_stmsgbody()->mutable_m_stdeletename_request();
    LOGDEBUG("Delete Name: name = %s, type = %d\n", pstReq->strname().c_str(), pstReq->itype());

	//将消息头中的uin替换为name的hash
	m_pMsg->mutable_m_stmsghead()->set_m_uin(GetNameHash(pstReq->strname()));

    //转发删除名字的请求到NameDB
    int iRet = CWorldMsgHelper::SendWorldMsgToNameDB(*m_pMsg);
    if(iRet < 0)
    {
        LOGERROR("Delete Name Failed: iRet = %d\n", iRet);

        return -2;
    }

    return 0;
}




