#ifndef __HANDLER_HPP__
#define __HANDLER_HPP__

#include "CodeQueue.hpp"
#include "ProtocolEngine.hpp"
#include "Public.hpp"
#include "ConfigHelper.hpp"
#include "GameProtocol.hpp"

using namespace ServerLib;

// 消息处理者抽象类
class IHandler
{
protected:
    IHandler() {}

public:
    virtual ~IHandler() {}

    // 消息处理接口。
    // 除了跟LotusServer交互之外，其他的Server交互不需要pstNetHead参数
    // 如果在消息处理中转发消息，则不使用pstHandleResult参数，置为NULL即可
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult) = 0;
protected:
    static int EncodeAndSendCode(IProtocolEngine* pProtocolEngine,
                          TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg, EGameServerID enMsgPeer);

    static int EncodeAndSendCodeBySession(IProtocolEngine* pProtocolEngine,
                          unsigned uiSessionFd, GameProtocolMsg* pstMsg, EGameServerID enMsgPeer);

    static void GenerateMsgHead(GameProtocolMsg* pstRegAuthMsg,
                                unsigned int uiSessionFd, unsigned int uiMsgID, unsigned int uiAccountHash);
};

#endif // __HANDLER_HPP__

