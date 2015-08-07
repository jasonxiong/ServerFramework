#ifndef __HANDLER_HPP__
#define __HANDLER_HPP__

#include "Public.hpp"

// 消息处理者抽象类
class CHandler
{
protected:
    CHandler() {};

public:
    virtual ~CHandler() {};

    // 消息处理
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult) = 0;

protected:
    virtual void GenerateResponseMsgHead(GameProtocolMsg* pstServerMsg, 
                                         unsigned int uiSessionID, unsigned int uiMsgID, unsigned int uiUin);
};

#endif // __HANDLER_HPP__

