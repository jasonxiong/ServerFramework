#include <string.h>

#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"
#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "LogAdapter.hpp"

#include "DeleteRoleAccountHandler.hpp"

CDeleteRoleAccountHandler::~CDeleteRoleAccountHandler()
{

}

GameProtocolMsg CDeleteRoleAccountHandler::m_stWorldMsg;
int CDeleteRoleAccountHandler::OnClientMsg(const void* pMsg)
{
	ASSERT_AND_LOG_RTN_INT(pMsg);

	m_pMsg = (GameProtocolMsg*)pMsg;

	switch (m_pMsg->m_stmsghead().m_uimsgid())
	{
	case MSGID_ACCOUNT_DELETEROLE_REQUEST:
		{
			OnRequestDeleteRoleAccount();
			break;
		}
	case MSGID_ACCOUNT_DELETEROLE_RESPONSE:
		{
			OnResponseDeleteRoleAccount();
			break;
		}
	default:
		{
			break;
		}
	}

	return 0;
}


int CDeleteRoleAccountHandler::OnRequestDeleteRoleAccount()
{
	const DeleteRole_Account_Request& rstReq = m_pMsg->m_stmsgbody().m_staccountdeleterolerequest();

    unsigned int uiUin = rstReq.stroleid().uin();
	if (uiUin == 0)
	{
		LOGERROR("Wrong DeleteRoleAccount Request, Uin %d\n", uiUin);
		return -1;
	}

	LOGDEBUG("Correct Uin DeleteRoleAccount Request, Uin %d\n", uiUin);

    // 不能删除在线的角色
    CWorldRoleStatusWObj* pRoleObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(rstReq.stroleid());
    if (pRoleObj)
    {
        LOGERROR("Cannot Delete Online Role!\n");

        CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg,
										 m_pMsg->m_stmsghead().m_uisessionfd(), MSGID_ACCOUNT_DELETEROLE_RESPONSE, uiUin);
        DeleteRole_Account_Response* pstResp = m_stWorldMsg.mutable_m_stmsgbody()->mutable_m_staccountdeleteroleresponse();
		pstResp->set_iresult(T_WORLD_SESSION_EXIST);
        CWorldMsgHelper::SendWorldMsgToAccount(m_stWorldMsg);

        return -2;
    }

    int iRet = CWorldMsgHelper::SendWorldMsgToRoleDB(*m_pMsg);

	return iRet;
}

int CDeleteRoleAccountHandler::OnResponseDeleteRoleAccount()
{
	unsigned int uiUin;
	uiUin = m_pMsg->m_stmsghead().m_uin();
	if (uiUin == 0)
	{
		LOGERROR("Wrong DeleteRoleAccount Response, Uin %d\n", uiUin);
		return -1;
	}

	LOGDEBUG("Correct Uin DeleteRoleAccount Response, Uin %d\n", uiUin);

    int iRet = CWorldMsgHelper::SendWorldMsgToAccount(*m_pMsg);

	return iRet;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
