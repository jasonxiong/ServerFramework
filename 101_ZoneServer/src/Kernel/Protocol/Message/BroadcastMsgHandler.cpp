﻿#include "BroadcastMsgHandler.hpp"
#include "LogAdapter.hpp"
#include "ZoneMsgHelper.hpp"

CBroadcastMsgHandler::~CBroadcastMsgHandler(void)
{
}

int CBroadcastMsgHandler::OnClientMsg()
{
	CZoneMsgHelper::SendZoneMsgToZoneAll(*m_pRequestMsg);

    return 0;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
