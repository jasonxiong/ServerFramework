#ifndef __SS_PROTOCOL_ENGINE_HPP__
#define __SS_PROTOCOL_ENGINE_HPP__

#include "SingletonTemplate.hpp"
#include "ProtocolEngine.hpp"

using namespace ServerLib;

// 除了LotusServer之外的其他的后台Server之间的协议解析，不包含TNetHead_V2结构
class S2SProtocolEngine : public IProtocolEngine
{
private:
    // 本类的对象只能在CSingleton<S2SProtocolEngine>类中创建
    friend class CSingleton<S2SProtocolEngine>;
    S2SProtocolEngine() {}

public:
    virtual int Initialize();
    virtual int Decode(unsigned char* pszCodeBuf, const int iCodeLen,
                       TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg);
    virtual int Encode(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                       unsigned char* pszCodeBuf, int iBufLen, int& iCodeLen);
};

#endif // __SS_PROTOCOL_ENGINE_HPP__


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
