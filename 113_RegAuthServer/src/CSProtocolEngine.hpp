#ifndef __CS_PROTOCOL_ENGINE_HPP__
#define __CS_PROTOCOL_ENGINE_HPP__

#include "SingletonTemplate.hpp"
#include "ProtocolEngine.hpp"

using namespace ServerLib;

// LotusServer与其他的后台Server之间的协议解析，包含TNetHead_V2结构的解析
class C2SProtocolEngine : public IProtocolEngine
{
private:
    // 本类的对象只能在CSingleton<C2SProtocolEngine>类中创建
    friend class CSingleton<C2SProtocolEngine>;
    C2SProtocolEngine() {}

public:
    virtual int Initialize();
    virtual int Decode(unsigned char* pszCodeBuf, const int iCodeLen,
                       TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg);
    virtual int Encode(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                       unsigned char* pszCodeBuf, int iBufLen, int& iCodeLen);
};

#endif // __CS_PROTOCOL_ENGINE_HPP__

