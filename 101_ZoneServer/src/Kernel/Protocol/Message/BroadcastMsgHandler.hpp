
#ifndef __BROADCASTMSG_HANDLER_HPP__
#define __BROADCASTMSG_HANDLER_HPP__

#include "Handler.hpp"

class CBroadcastMsgHandler : public IHandler
{
public:
    virtual ~CBroadcastMsgHandler();

public:
    virtual int OnClientMsg();
};



#endif
