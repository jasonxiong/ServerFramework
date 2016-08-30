#include <string.h>

#include "HandlerFactory.hpp"
#include "LogAdapter.hpp"
#include "GameConfigDefine.hpp"
#include "ConfigManager.hpp"

using namespace ServerLib;

IHandler* CHandlerFactory::m_apHandler[MAX_HANDLER_NUMBER];
TZoneMsgConfig CHandlerFactory::m_astMsgConfig[MAX_HANDLER_NUMBER];


IHandler* CHandlerFactory::GetHandler(const unsigned int uiMsgID, EZoneMsgType enMsgType)
{
	if(uiMsgID >= MAX_HANDLER_NUMBER)
	{
        TRACESVR("NULL handler, msg id: %d\n", uiMsgID);
		return NULL;
	}

	if (!m_apHandler[uiMsgID])
	{
		return NULL;
	}

	TZoneMsgConfig* pstMsgConfig = CHandlerFactory::GetMsgConfig(uiMsgID);
	if (!pstMsgConfig)
	{
		return NULL;
	}

    // 客户端消息只能调用客户端Handler, 服务器消息可以直接调用客户端Handler
    if(enMsgType == pstMsgConfig->m_enMsgType || EKMT_SERVER == enMsgType)
    {
        return m_apHandler[uiMsgID];
    }
    else
    {
        return NULL;
    }
}

TZoneMsgConfig* CHandlerFactory::GetMsgConfig(const unsigned int uiMsgID)
{
	if(uiMsgID >= MAX_HANDLER_NUMBER)
	{
		TRACESVR("NULL handler, msg id: %d\n", uiMsgID);
		return NULL;
	}

	return &m_astMsgConfig[uiMsgID];
}

int CHandlerFactory::RegisterHandler(const unsigned int uiMsgID, const IHandler* pHandler, 
									 EZoneMsgType enMsgType, 
									 unsigned int uiHandlerFlag)
{
	if(uiMsgID >= MAX_HANDLER_NUMBER)
	{
		TRACESVR("Inv MsgID: %d\n", uiMsgID);
		return -1;
	}

    // 防止重复注册
    if (m_apHandler[uiMsgID])
    {
        TRACESVR("Duplicated Handler Registered: uiMsgID = %d, Handler = %p\n", uiMsgID, pHandler);
        abort();
    }

	TZoneMsgConfig* pstMsgConfig = CHandlerFactory::GetMsgConfig(uiMsgID);
	if (!pstMsgConfig)
	{
		abort();
	}

	m_apHandler[uiMsgID] = (IHandler*)pHandler;
    pstMsgConfig->m_enMsgType = enMsgType;
	pstMsgConfig->m_uiMsgFlag = uiHandlerFlag;

	pstMsgConfig->m_iMsgInterval = DEFAULT_MSG_INTERVAL;

	return 0;
}


void CHandlerFactory::DisabledMsg(unsigned int uiMsgID)
{
	if (uiMsgID >= MAX_HANDLER_NUMBER)
	{
		TRACESVR("DisabledMsg failed, MsgID:%u\n", uiMsgID);
		return;
	}

	m_astMsgConfig[uiMsgID].m_bDisabled = true;
}

void CHandlerFactory::ClearAllDisableMsg()
{
	for (int i = 0; i < MAX_HANDLER_NUMBER; i++)
	{
		m_astMsgConfig[i].m_iMsgInterval = 0;
		m_astMsgConfig[i].m_bDisabled = false;
	}
}
bool CHandlerFactory::IsMsgDisabled(const unsigned int uiMsgID)
{
	if (uiMsgID >= MAX_HANDLER_NUMBER)
	{
		TRACESVR("DisabledMsg failed, MsgID:%u\n", uiMsgID);
		return false;
	}

	return m_astMsgConfig[uiMsgID].m_bDisabled;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
