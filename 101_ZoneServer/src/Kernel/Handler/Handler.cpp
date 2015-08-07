#include <typeinfo>

#include "Handler.hpp"
#include "HandlerHelper.hpp"
#include "LogAdapter.hpp"
#include "GameRole.hpp"
#include "GameSession.hpp"
#include "HandlerFactory.hpp"

using namespace ServerLib;

IHandler::IHandler()
{
	m_pRequestMsg = NULL;
	m_pNetHead = NULL;
	m_pRoleObj = NULL;
	m_pSession = NULL;
}

IHandler::~IHandler()
{
}

// 保存请求参数
void IHandler::SetClientMsg(const GameProtocolMsg* pRequestMsg, 
							const TNetHead_V2* pNetHead,
							EGameServerID enMsgPeer)
{
	m_pRequestMsg = (GameProtocolMsg*)pRequestMsg;
	m_pNetHead = (TNetHead_V2*)pNetHead;
	m_enMsgPeer = enMsgPeer;
	m_pRoleObj = NULL;
	m_pSession = NULL;

	CHandlerHelper::SetErrorCode(T_SERVER_SUCESS);
}

int IHandler::SecurityCheck()
{
	// 非客户端的消息不用检查
	if (!m_pNetHead)
	{
		return 0;
	}

    // 检查消息发送者
    m_pRoleObj = CHandlerHelper::GetMsgSender(m_pRequestMsg, m_pNetHead);
    if (!m_pRoleObj)
    { 
		if (m_pRequestMsg)
		{
			TRACESVR("in Handler:%s No role: %u\n", typeid(*this).name(), m_pRequestMsg->m_stmsghead().m_uin());
		}
		return -1;
    }

	if(m_pRoleObj->GetUin() != m_pRequestMsg->m_stmsghead().m_uin())
	{
		return -2;
	}

    m_pSession = m_pRoleObj->GetSession();
    if (!m_pSession)
    {
		TRACESVR("in handle:%s No session: %u\n", 
				typeid(*this).name(), m_pRoleObj->GetRoleID().uin());
        return -2;
    }

	int iMsgID = m_pRequestMsg->m_stmsghead().m_uimsgid();
	TZoneMsgConfig* pstMsgConfig = CHandlerFactory::GetMsgConfig(iMsgID);
	if (!pstMsgConfig)
	{
		return -3; //nolog
	}

	if (CHandlerFactory::IsMsgDisabled(iMsgID))
	{
		TRACESVR("uin:%u msgID:%d has disabled\n", m_pRoleObj->GetRoleID().uin(), iMsgID);
		//CPromptMsgHelper::SendPromptMsgToRole(m_pRoleObj, EQEC_Module_Disabled);
		return -10;
	}

	// 检查消息频率
	if (m_enMsgPeer == GAME_SERVER_LOTUSZONE)
	{
		//是否被频率限制
		if (m_pRoleObj->IsMsgFreqLimit(iMsgID))
		{
			TRACESVR("in handler:%s uin:%u send msg ID:%d too frequently\n", 
					typeid(*this).name(), m_pRoleObj->GetRoleID().uin(), iMsgID);
			return -4;
		}
	}
	
	m_pRoleObj->PushMsgID(iMsgID, m_enMsgPeer);
	return 0;	
}

