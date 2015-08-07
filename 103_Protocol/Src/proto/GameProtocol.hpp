#ifndef __GAME_PROTOCOL_HPP__
#define __GAME_PROTOCOL_HPP__

//对协议的protobuf进行封装，方便业务调用

#include "GameProtocol.CS.pb.h"

typedef GameProtocol::CS::ProtocolCSMsg GameProtocolMsg;
typedef GameProtocol::CS::CSMsgHead     GameCSMsgHead;
typedef GameProtocol::CS::CSMsgBody     GameCSMsgBody;

#endif
