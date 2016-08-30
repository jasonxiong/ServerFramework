#ifndef __ZONE_MSG_HELPER_HPP__
#define __ZONE_MSG_HELPER_HPP__

#include "GameProtocol.hpp"
#include "GameSession.hpp"
#include "ConfigHelper.hpp"
#include "LogAdapter.hpp"
#include "GameObjCommDef.hpp"

// 封装服务器下发给客户端的各种消息, 作用是:
// 1. 方便IHandler之外的模块使用

class CGameRoleObj;

typedef struct tagRoleObjList
{
    int m_iRoleNumber;
    CGameRoleObj* m_apstRoleObj[MAX_ROLE_OBJECT_NUMBER_IN_ZONE];
}TRoleObjList;

class CZoneMsgHelper
{
public:
    // 初始化消息头部
    static void GenerateMsgHead(GameProtocolMsg& rstGameMsg, const unsigned int uiMsgID);

public:
    // 发送消息到客户端
    static int SendZoneMsgToClient(GameProtocolMsg& rstZoneMsg, const TNetHead_V2& rstNetHead);

    // 发送消息到World
    static int SendZoneMsgToWorld(const GameProtocolMsg& rstZoneMsg);

public:
    // 发送消息给pRoleObj
    static int SendZoneMsgToRole(GameProtocolMsg& rstGameMsg, CGameRoleObj *pRoleObj); 

    // 发送消息给很多人
    static int SendZoneMsgToRoleList(GameProtocolMsg& rstZoneMsg, const TRoleObjList& rstRoleList);

    // 发送消息给本线所有用户
    static int SendZoneMsgToZoneAll(GameProtocolMsg& rstZoneMsg);

    // 发送消息给本线所有用户, 除了pMe
    static int SendZoneMsgToZoneAllExcludeMe(GameProtocolMsg& rstZoneMsg, CGameRoleObj* pRoleObj);  

public:

    static inline void AddRoleToList(TRoleObjList& rstRoleList, CGameRoleObj* pRoleObj)
    {
        ASSERT_AND_LOG_RTN_VOID(rstRoleList.m_iRoleNumber < MAX_ROLE_OBJECT_NUMBER_IN_ZONE);

        rstRoleList.m_apstRoleObj[rstRoleList.m_iRoleNumber] = pRoleObj;
        ++rstRoleList.m_iRoleNumber;
    }
	
public:

    // 系统世界频道消息
    static void WorldChat(char* pszMessage);

private:
    static GameProtocolMsg m_stZoneMsg;
};


#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
