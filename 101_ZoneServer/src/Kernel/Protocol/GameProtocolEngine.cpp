#include <math.h>

#include <google/protobuf/text_format.h>
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "LogFile.hpp"
#include "AppLoop.hpp"
#include "HandlerFactory.hpp"
#include "GameProtocolEngine.hpp"
#include "HandlerHelper.hpp"
#include "RttUtility.hpp"
#include "ModuleHelper.hpp"
#include "SortArray.hpp"
#include "ZoneObjectHelper.hpp"

using namespace ServerLib;

bool IsNeedPlayerLog(unsigned int uiUin, unsigned int uiMsgID)
{
	if (!CModuleHelper::IsPlayerLogEnabled())
	{
		return false;
	}

	if (uiUin > 0)
	{
		return true;
	}

	return false;
}

int CGameProtocolEngine::Initialize(bool bResumeMode)
{
	memset(m_szGameMsg, 0, sizeof(m_szGameMsg));

	m_stMsg.Clear();

	return 0;
}

int CGameProtocolEngine::OnRecvCode(char* pszMsgBuffer, int iMsgLength, EGameServerID enMsgPeer, int iInstanceID)
{
	int iRet = -1;

	TNetHead_V2* pstNetHead = (TNetHead_V2*)pszMsgBuffer;
	EZoneMsgType enMsgType = EKMT_SERVER;

	m_stMsg.Clear();

	if (enMsgPeer == GAME_SERVER_LOTUSZONE)
	{
		enMsgType = EKMT_CLIENT;
		//socket id 做转换
		unsigned  int uiSessionID = ntohl(pstNetHead->m_uiSocketFD);
		uiSessionID += iInstanceID * MAX_FD_NUMBER;
		pstNetHead->m_uiSocketFD = htonl(uiSessionID);

		// 空包，连接断开
		if (iMsgLength == NETHEAD_V2_SIZE)
		{
			GameCSMsgHead* pstHead = m_stMsg.mutable_m_stmsghead();
			pstHead->set_m_uimsgid(MSGID_LOGOUTSERVER_REQUEST);
		}
		else
		{
			//解码获得ZoneMsg和NetHead
			iRet = DecodeClient(pszMsgBuffer, iMsgLength, m_stMsg, enMsgPeer);
			if(iRet < 0)
			{
				LOGERROR("Decode client fail: %d\n", iRet);
				return -1;
			}

			//todo jasonxiong2 有效包解码后需要校验玩家session key是否有效
		}
	}
	else
	{
		pstNetHead = NULL;
		iRet = DecodeClient(pszMsgBuffer, iMsgLength, m_stMsg, enMsgPeer);
		if(iRet < 0)
		{
			LOGERROR("Decode client fail: %d\n", iRet);
			return -1;
		}
	}

	//根据MsgID获取消息处理器Handler
	unsigned int uiMsgID = m_stMsg.m_stmsghead().m_uimsgid();

	IHandler* pHandler = CHandlerFactory::GetHandler(uiMsgID, enMsgType);
	if(!pHandler)
	{
		LOGERROR("Protocol Engine : Unknown command %d.\n", uiMsgID);
		return -2;
	}
	 
	//把消息交由Handler处理
	pHandler->SetClientMsg(&m_stMsg, pstNetHead, enMsgPeer);
	pHandler->SetMsgID(uiMsgID);

	iRet = pHandler->OnClientMsg();
	if(iRet < 0)
	{
		LOGERROR("Process msg fail: %u, %d, %d\n", m_stMsg.m_stmsghead().m_uin(), uiMsgID, iRet);
		return -3;
	}

	return 0;
}

int CGameProtocolEngine::DecodeClient(const char* pszMsgBuffer, const int iMsgLength, GameProtocolMsg& rstMsg, EGameServerID enMsgPeer)
{
	//网络数据解码成本地数据
	int iBuffOffSet = 0;

	int iMsgType = ESMT_FROM_CLIENT;

	if (enMsgPeer == GAME_SERVER_LOTUSZONE)
	{
		iBuffOffSet = NETHEAD_V2_SIZE + sizeof(unsigned short);
	}
	else
	{
		iBuffOffSet = sizeof(unsigned short);
		iMsgType = ESMT_FROM_SERVER;
	}

	int iNetBuffLen = iMsgLength - iBuffOffSet;
	const char* pNetBuff = pszMsgBuffer + iBuffOffSet;

	if(!m_stMsg.ParseFromArray(pNetBuff, iNetBuffLen))
	{
		LOGERROR("Failed to parse proto msg from buff, len %d\n", iNetBuffLen);
		return T_ZONE_PROTOBUFF_ERR;
	}

	unsigned int uiUin = m_stMsg.m_stmsghead().m_uin();
	unsigned int uiMsgID = m_stMsg.m_stmsghead().m_uimsgid();

	timeval stTime;
	stTime.tv_sec = stTime.tv_usec = 0;

	//todo jasonxiong 消息统计这边暂时先设置为0，等后续开发再确认是否需要修改
	MsgStatisticSingleton::Instance()->AddMsgStat(uiMsgID, ESMR_SUCCEED, 
												  iMsgLength, stTime, iMsgType);

	if(enMsgPeer == GAME_SERVER_LOTUSZONE)
	{
		if (IsNeedPlayerLog(uiUin, uiMsgID))
		{
			FILE* pProtoLogFile = PLAYERPROTOFILE(uiUin);
			if (pProtoLogFile)
			{
				time_t tNow = time(NULL);
				struct tm stTempTm;
				struct tm *pTempTm = localtime_r(&tNow, &stTempTm);
	
				fprintf(pProtoLogFile, "[%04d-%02d-%2d %02d:%02d:%02d] : Recv ======================================\n", 
					pTempTm->tm_year + 1900, pTempTm->tm_mon + 1, pTempTm->tm_mday,
					pTempTm->tm_hour, pTempTm->tm_min, pTempTm->tm_sec);
	
				//打印消息内容到文件
				static std::string strMsgInfo;
				::google::protobuf::TextFormat::PrintToString(m_stMsg, &strMsgInfo);
				fprintf(pProtoLogFile, "%s\n\n", strMsgInfo.c_str());
	
				fflush(pProtoLogFile);
			}
		}
	}

	return T_SERVER_SUCESS;
}

int CGameProtocolEngine::SendZoneMsg(GameProtocolMsg& rstZoneMsg, const TNetHead_V2& rstNetHead, EGameServerID enMsgPeer)
{
	int iRet = -1;
	int iBuffOffSet = 0;

	if(!m_pAppLoop)
	{
		LOGERROR("Fatal: No AppLoop Set!\n");
		return -1;
	}

	int iMsgType = ESMT_TO_CLIENT;

	if (enMsgPeer == GAME_SERVER_LOTUSZONE)
	{
		iBuffOffSet = NETHEAD_V2_SIZE + sizeof(unsigned short);
	}
	else
	{
		iBuffOffSet = sizeof(unsigned short);
		iMsgType = ESMT_TO_SERVER;
	}

	char* pszBuff = m_szGameMsg + iBuffOffSet;
	int iBuffLen = sizeof(m_szGameMsg) - iBuffOffSet;

	unsigned int uiUin = rstZoneMsg.m_stmsghead().m_uin();
	unsigned int uiMsgID = rstZoneMsg.m_stmsghead().m_uimsgid();

	if(!rstZoneMsg.SerializeToArray(pszBuff, iBuffLen))
	{
		LOGERROR("protobuf serialize Failed: MsgID = %d, Error = %u\n", uiMsgID, T_ZONE_PROTOBUFF_ERR);

		FILE* fFile = SVRLOGFP();
		fprintf(fFile, "protobuf serialize Failed: Error = %d\n", T_ZONE_PROTOBUFF_ERR);
		fflush(fFile);

		return T_ZONE_PROTOBUFF_ERR;
	}

	unsigned short ushTotalLength = rstZoneMsg.ByteSize() + sizeof(unsigned short);

	if (enMsgPeer == GAME_SERVER_LOTUSZONE)
	{
		TNetHead_V2 stTmpNetHead = rstNetHead;
		//socket id 做转换
		unsigned  int uiSessionID = ntohl(rstNetHead.m_uiSocketFD);
		int iInstanceID = uiSessionID / MAX_FD_NUMBER;
		stTmpNetHead.m_uiSocketFD = htonl(uiSessionID % MAX_FD_NUMBER);

		*(TNetHead_V2*)&m_szGameMsg[0] = stTmpNetHead;
		m_szGameMsg[NETHEAD_V2_SIZE] = ushTotalLength / 256;
		m_szGameMsg[NETHEAD_V2_SIZE + 1] = ushTotalLength % 256;

		iRet = CModuleHelper::GetZoneMsgTransceiver()->SendOneMsg(m_szGameMsg, ushTotalLength + NETHEAD_V2_SIZE, enMsgPeer, iInstanceID);
	}
	else
	{   
		m_szGameMsg[0] = ushTotalLength / 256;
		m_szGameMsg[1] = ushTotalLength % 256;

		iRet = CModuleHelper::GetZoneMsgTransceiver()->SendOneMsg(m_szGameMsg, ushTotalLength, enMsgPeer);
	}

	if (iRet < 0)
	{
		LOGERROR("Fatal: Send Msg failed, iRet = %d, Msg = %p, TotalLen = %d, MsgPeer = %d\n", 
			iRet, m_szGameMsg, ushTotalLength + NETHEAD_V2_SIZE, enMsgPeer);
	}
	else
	{
		timeval stTime;
		stTime.tv_sec = stTime.tv_usec = 0;

		MsgStatisticSingleton::Instance()->AddMsgStat(uiMsgID, ESMR_SUCCEED, ushTotalLength, stTime, iMsgType);
	}

	if (enMsgPeer == GAME_SERVER_LOTUSZONE)
	{
		int iSessionID = ntohl(rstNetHead.m_uiSocketFD);
		CGameSessionObj* pSession = CModuleHelper::GetSessionManager()->FindSessionByID(iSessionID);
		if (pSession)
		{
			uiUin = pSession->GetRoleID().uin();
			if (IsNeedPlayerLog(uiUin, uiMsgID))
			{
				FILE* pProtoLogFile = PLAYERPROTOFILE(uiUin);
				if (pProtoLogFile)
				{
					time_t tNow = time(NULL);
					struct tm stTempTm;
					struct tm *pTempTm = localtime_r(&tNow, &stTempTm);

					fprintf(pProtoLogFile, "[%04d-%02d-%2d %02d:%02d:%02d] : Send ======================================\n", 
						pTempTm->tm_year + 1900, pTempTm->tm_mon + 1, pTempTm->tm_mday,
						pTempTm->tm_hour, pTempTm->tm_min, pTempTm->tm_sec);

					//打印消息内容到文件
					static std::string strMsgInfo;
					::google::protobuf::TextFormat::PrintToString(rstZoneMsg, &strMsgInfo);
					fprintf(pProtoLogFile, "%s\n\n", strMsgInfo.c_str());

					fflush(pProtoLogFile);
				}
			}
		}
	}

	return iRet;
}

// 将批量数据发送到Lotus
int CGameProtocolEngine::SendZoneMsgListToMultiLotus(GameProtocolMsg& rstZoneMsg, 
													   unsigned int auiSocketFD[MAX_SOCKETFD_NUMBER], 
													   unsigned int uiSocketNumber)
{
	int iRet;
	int iRealSocketNumber = uiSocketNumber;
	if (uiSocketNumber == SEND_ALL_SESSION)
	{
		iRealSocketNumber = GameTypeK32<CGameRoleObj>::GetUsedObjNumber();

		// 广播电信通道
		iRet = SendZoneMsgListToLotus(rstZoneMsg, auiSocketFD, SEND_ALL_SESSION, 0, iRealSocketNumber);
		if (iRet < 0)
		{
			LOGERROR("SendZoneMsgListToLotus 0 failed, iRet:%d\n", iRet);
		}

		// 广播联通通道
		iRet = SendZoneMsgListToLotus(rstZoneMsg, auiSocketFD, SEND_ALL_SESSION, 1, 0);
		if (iRet < 0)
		{
			LOGERROR("SendZoneMsgListToLotus 1 failed, iRet:%d\n", iRet);
		}

		return iRet;
	}
	else if (uiSocketNumber == SEND_ALL_SESSION_BUTONE)
	{
		iRealSocketNumber = GameTypeK32<CGameRoleObj>::GetUsedObjNumber() - 1;

		unsigned int uiSocketFD = ntohl(auiSocketFD[0]);
		if (uiSocketFD >= (unsigned int)MAX_FD_NUMBER)
		{
			// 广播电信通道
			iRet = SendZoneMsgListToLotus(rstZoneMsg, auiSocketFD, SEND_ALL_SESSION, 0, iRealSocketNumber);
			if (iRet < 0)
			{
				LOGERROR("SendZoneMsgListToLotus 0 failed, iRet:%d\n", iRet);
			}

			// 广播联通通道，除了auiSocketFD[0]
			iRet = SendZoneMsgListToLotus(rstZoneMsg, auiSocketFD, SEND_ALL_SESSION_BUTONE, 1, 0);
			if (iRet < 0)
			{
				LOGERROR("SendZoneMsgListToLotus 1 failed, iRet:%d\n", iRet);
			}
		}
		else
		{
			// 广播电信通道，除了auiSocketFD[0]
			iRet = SendZoneMsgListToLotus(rstZoneMsg, auiSocketFD, SEND_ALL_SESSION_BUTONE, 0, iRealSocketNumber);
			if (iRet < 0)
			{
				LOGERROR("SendZoneMsgListToLotus 0 failed, iRet:%d\n", iRet);
			}

			// 广播联通通道
			iRet = SendZoneMsgListToLotus(rstZoneMsg, auiSocketFD, SEND_ALL_SESSION, 1, 0);
			if (iRet < 0)
			{
				LOGERROR("SendZoneMsgListToLotus 1 failed, iRet:%d\n", iRet);
			}
		}

		return iRet;
	}


	for (unsigned int i = 0; i < uiSocketNumber; i++)
	{
		auiSocketFD[i] = ntohl(auiSocketFD[i]);
	}

	qsort(auiSocketFD, uiSocketNumber, sizeof(unsigned int), Cmp<unsigned int>);
	//now only two code queue ,send 2 code queue

	unsigned int uiToSendSecondSocketIdx = uiSocketNumber;
	for (unsigned int i = 0; i < uiSocketNumber; i++)
	{
		if (auiSocketFD[i] >= (unsigned int)MAX_FD_NUMBER)
		{
			uiToSendSecondSocketIdx = i;
			break;
		}
	}

	for (unsigned int i = 0; i < uiToSendSecondSocketIdx; i++)
	{
		auiSocketFD[i] = htonl(auiSocketFD[i]);
	}

	for (unsigned int i = uiToSendSecondSocketIdx; i < uiSocketNumber; i++)
	{
		auiSocketFD[i] = htonl(auiSocketFD[i]%MAX_FD_NUMBER);
	}

	if (uiToSendSecondSocketIdx > 0)
	{
		iRet = SendZoneMsgListToLotus(rstZoneMsg, auiSocketFD, uiToSendSecondSocketIdx, 0, uiToSendSecondSocketIdx);
		if (iRet < 0)
		{
			LOGERROR("SendZoneMsgListToLotus 0 failed, iRet:%d\n", iRet);
		}
	}

	if (uiToSendSecondSocketIdx < uiSocketNumber)
	{
		iRet = SendZoneMsgListToLotus(rstZoneMsg, auiSocketFD + uiToSendSecondSocketIdx, uiSocketNumber - uiToSendSecondSocketIdx, 1, uiSocketNumber - uiToSendSecondSocketIdx);
		if (iRet < 0)
		{
			LOGERROR("SendZoneMsgListToLotus 1 failed, iRet:%d\n", iRet);
		}
	}

	return 0;
}

// 将批量数据发送到Lotus
int CGameProtocolEngine::SendZoneMsgListToLotus(GameProtocolMsg& rstZoneMsg, 
												  unsigned int auiSocketFD[MAX_SOCKETFD_NUMBER], 
												  unsigned int uiSocketNumber, int iInstanceID, int iStaticSocketNumber)
{
	int iRet = -1;
	int iBuffOffSet = 0;

	if(!m_pAppLoop)
	{
		LOGERROR("Fatal: No AppLoop Set!\n");
		return -1;
	}

	int iNetHeadUinListSize;
	switch (uiSocketNumber)
	{
	case SEND_ALL_SESSION:
		// 广播给所有的人
		iNetHeadUinListSize = 2 * sizeof(unsigned int);
		break;

	case SEND_ALL_SESSION_BUTONE:
		// 广播给除auiSocketFD[0]以外的人
		iNetHeadUinListSize = 3 * sizeof(unsigned int);
		break;

	default:
		// 普通广播
		iNetHeadUinListSize = (uiSocketNumber + 2) * sizeof(unsigned int);
		break;
	}

	iBuffOffSet = iNetHeadUinListSize + sizeof(unsigned short);
	char* pszBuff = m_szGameMsg + iBuffOffSet;
	int iBuffLen = sizeof(m_szGameMsg) - iBuffOffSet;

	unsigned int uiMsgID = rstZoneMsg.m_stmsghead().m_uimsgid();
	if (!rstZoneMsg.SerializeToArray(pszBuff, iBuffLen))
	{
		LOGERROR("protobuf serialize Failed: MsgID = %d, Error = %d\n", uiMsgID, T_ZONE_PROTOBUFF_ERR);

		FILE* fFile = SVRLOGFP();
		fprintf(fFile, "tdr_hton Failed: Error = %d\n", T_ZONE_PROTOBUFF_ERR);
		fflush(fFile);

		return T_ZONE_PROTOBUFF_ERR;
	}

	unsigned short ushTotalLength = rstZoneMsg.ByteSize()+ sizeof(unsigned short);
	if (ushTotalLength + iNetHeadUinListSize > MAX_MSGBUFFER_SIZE)
	{
		LOGERROR("FATAL: Msg Is Too Long, SocketNumber = %d\n", uiSocketNumber);
		return -1;
	}

	*(unsigned int*)&m_szGameMsg[0] = SEND_MULTI_SESSION; // 特殊标识位, 告诉Lotus按UinList格式解析后面的数据
	*(unsigned int*)&m_szGameMsg[sizeof(unsigned int)] = uiSocketNumber;
	switch (uiSocketNumber)
	{
	case SEND_ALL_SESSION:
		// 广播给所有的人
		break;

	case SEND_ALL_SESSION_BUTONE:
		// 广播给除auiSocketFD[0]以外的人
		memcpy(&m_szGameMsg[2*sizeof(unsigned int)], auiSocketFD, 1 * sizeof(unsigned int));
		break;

	default:
		// 普通广播
		memcpy(&m_szGameMsg[2*sizeof(unsigned int)], auiSocketFD, uiSocketNumber * sizeof(unsigned int));
		break;
	}

	m_szGameMsg[iNetHeadUinListSize] = ushTotalLength / 256;
	m_szGameMsg[iNetHeadUinListSize + 1] = ushTotalLength % 256;

	iRet = CModuleHelper::GetZoneMsgTransceiver()->SendOneMsg(m_szGameMsg, ushTotalLength + iNetHeadUinListSize, GAME_SERVER_LOTUSZONE, iInstanceID);
	if (iRet < 0)
	{
		LOGERROR("Fatal: Send Msg failed, iRet = %d, Msg = %p, TotalLen = %d\n", 
			iRet, m_szGameMsg, ushTotalLength + iNetHeadUinListSize);
	}
	else 
	{
		if (iStaticSocketNumber > 0)
		{
			timeval stTime;
			stTime.tv_sec = stTime.tv_usec = 0;

			MsgStatisticSingleton::Instance()->AddMsgStat(uiMsgID, ESMR_SUCCEED, 
				ushTotalLength, stTime, ESMT_TO_CLIENT, iStaticSocketNumber);
		}
	}

	return iRet;   
}

void CGameProtocolEngine::SetService(CAppLoop* pAppLoop)
{
	m_pAppLoop = pAppLoop;
}
