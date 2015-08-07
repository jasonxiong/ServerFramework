
#ifndef __GAME_PROTOCOL_ENGINE_HPP__
#define __GAME_PROTOCOL_ENGINE_HPP__

#include "UnixTime.hpp"
#include "GameProtocol.hpp"
#include "SessionDefine.hpp"
#include "GameRole.hpp"
#include "GameMsgTransceiver.hpp"
#include "MsgStatistic.hpp"

const int MAX_MSGBUFFER_SIZE = 2048000;

const int MAX_SOCKETFD_NUMBER = 200;

const unsigned int SEND_MULTI_SESSION = 0xFFFFFFFF;
const unsigned int SEND_ALL_SESSION = 0xFFFFFFFF;
const unsigned int SEND_ALL_SESSION_BUTONE = 0xFFFFFFFE;

class CAppLoop;
class CZoneSceneObj;

class CGameProtocolEngine
{
public:
	int Initialize(bool bResumeMode);
	void SetService(CAppLoop* pAppLoop);

public:
    // 将数据解包, 并交给Handler处理
    int OnRecvCode(char* pszMsgBuffer, int iMsgLength, EGameServerID enMsgPeer, int iInstanceID = 0);

    // 将数据打包, 并发送到对端
	int SendZoneMsg(GameProtocolMsg& rstZoneMsg, const TNetHead_V2& rstNetHead, EGameServerID enMsgPeer);

	// 将批量数据发送到多个lotus
	int SendZoneMsgListToMultiLotus(GameProtocolMsg& rstZoneMsg, 
			unsigned int auiSocketFD[MAX_SOCKETFD_NUMBER], 
			unsigned int uiSocketNumber);

	// 将批量数据发送到Lotus
	int SendZoneMsgListToLotus(GameProtocolMsg& rstZoneMsg, 
		unsigned int auiSocketFD[MAX_SOCKETFD_NUMBER], 
		unsigned int uiSocketNumber, 
		int iInstanceID, int iStaticSocketNumber);

private:
    // 通过protobuf解析收到的数据
	int DecodeClient(const char* pszMsgBuffer, const int iMsgLength, GameProtocolMsg& rstMsg, EGameServerID enMsgPeer);

private:
	char m_szGameMsg[MAX_MSGBUFFER_SIZE];

	GameProtocolMsg m_stMsg;
	CAppLoop* m_pAppLoop;
};

#endif
