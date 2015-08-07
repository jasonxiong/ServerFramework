#ifndef __HANDLER_SET_HPP__
#define __HANDLER_SET_HPP__

#include <string.h>
#include "Handler.hpp"

using namespace GameConfig;

// 消息处理者管理器
class CHandlerSet
{
protected:
    // 用数组表示的消息处理者集合
    CHandler* m_apHandler[MAX_HANDLER_NUMBER];

protected:
    CHandlerSet();

public:
    virtual ~CHandlerSet() {};

public:
    // 初始化消息处理者集合中的各个消息处理者，返回值为0表示成功，其他表示失败
    virtual int Initialize();

    // 根据消息id返回该消息的处理者
    CHandler* GetHandler(const unsigned int uiMsgID);

protected:
    // 根据消息id注册它的处理者，返回值为0表示成功，其他表示失败
    int RegisterHandler(const unsigned int uiMsgID, CHandler* pHandler);
};

#endif // __HANDLER_SET_HPP__

