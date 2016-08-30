
#ifndef __I_HANDLER_HPP__
#define __I_HANDLER_HPP__

#include "GameProtocol.hpp"
#include "SessionDefine.hpp"
#include "ConfigHelper.hpp"

class CGameRoleObj;
class CGameSessionObj;

class IHandler
{
public:
	IHandler();
	virtual ~IHandler();

	// 保存请求参数
	void SetClientMsg(const GameProtocolMsg* pRequestMsg, const TNetHead_V2* pNetHead, EGameServerID enMsgPeer = GAME_SERVER_LOTUSZONE);

	void SetMsgID(int iMsgID)
	{
		m_iMsgID = iMsgID;
	}

	int GetMsgID()const
	{
		return m_iMsgID;
	}

public:
    // 消息处理入口
	virtual int OnClientMsg() = 0;

protected:
    // 检查发送者
    virtual int SecurityCheck();

protected:
	// 消息变量
	GameProtocolMsg* m_pRequestMsg;
	TNetHead_V2* m_pNetHead;
	CGameSessionObj* m_pSession;
	CGameRoleObj* m_pRoleObj;
	EGameServerID m_enMsgPeer;	

	int m_iMsgID;
};



#endif



----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
