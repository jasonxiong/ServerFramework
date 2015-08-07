
#ifndef __HANDLER_HELPER_HPP__
#define __HANDLER_HELPER_HPP__

#include "GameProtocol.hpp"
#include "SessionDefine.hpp"
#include "ZoneErrorNumDef.hpp"

class CGameRoleObj;
class CGameSessionObj;

class CHandlerHelper
{
public:
	static CGameSessionObj* FindValidSession(const int iSessionID, const RoleID& stRoleID);
    
    static CGameRoleObj* GetMsgSender(GameProtocolMsg* pZoneMsg, const TNetHead_V2* pNetHead);

    // 设置和获取返回给客户端的错误码
    static void SetErrorCode(unsigned int enErrorCode);
    static unsigned int GetErrorCode();

private:
    static unsigned int m_enErrorCode;
};

#endif
