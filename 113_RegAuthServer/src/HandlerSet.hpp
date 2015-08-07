#ifndef __HANDLER_SET_HPP__
#define __HANDLER_SET_HPP__

#include <string.h>

#include "RegAuthPublic.hpp"
#include "Handler.hpp"

typedef enum tagenRegAuthMsgType
{
    EKMT_CLIENT = 1,    // 客户端消息
    EKMT_SERVER = 2,    // 服务器消息
}ERegAuthMsgType;

typedef struct tagRegAuthMsgHandler
{
    IHandler* m_pHandler;       // 消息处理函数
    ERegAuthMsgType m_enMsgType;   // 消息类型
}TRegAuthMsgHandler;

// 消息处理者管理器
class CHandlerSet
{
protected:
    // 用数组表示的消息处理者集合
    TRegAuthMsgHandler m_apHandler[MAX_REGAUTH_HANDLER_NUMBER];

protected:
    CHandlerSet();

public:
    virtual ~CHandlerSet() {}

public:
    // 初始化消息处理者集合中的各个消息处理者，返回值为0表示成功，其他表示失败
    virtual int Initialize();

    // 根据消息id返回该消息的处理者
    IHandler* GetHandler(const unsigned int uiMsgID, ERegAuthMsgType enMsgType = EKMT_SERVER);

protected:
    // 根据消息id注册它的处理者，返回值为0表示成功，其他表示失败
    int RegisterHandler(const unsigned int uiMsgID, IHandler* pHandler, ERegAuthMsgType enMsgType = EKMT_SERVER);
};

#endif // __HANDLER_SET_HPP__

