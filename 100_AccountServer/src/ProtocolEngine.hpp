#ifndef __PROTOCOL_ENGINE_HPP__
#define __PROTOCOL_ENGINE_HPP__

#include "Public.hpp"

typedef enum
{
    PT_CS = 0, // 需要解析NetHead
    PT_SS,     // 不需要解析NetHead
    PT_MAX
} EProtocolType;

// 协议解析抽象类
class IProtocolEngine
{
protected:
    EProtocolType m_eType;

public:
    EProtocolType GetProtocolType() const
    {
        return m_eType;
    }

protected:
    IProtocolEngine()
    {
        m_eType = PT_MAX;
    }

public:
    virtual ~IProtocolEngine() {}

public:
    // 相关初始化
    virtual int Initialize() = 0;

    // 将code解码为TNetHead_V2结构和GameProtocolMsg结构，注意可能没有TNetHead_V2结构，传入NULL即可
    virtual int Decode(unsigned char* pszCodeBuf, const int iCodeLen,
                       TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg) = 0;

    // 将TNetHead_V2结构和GameProtocolMsg结构编码为code，注意可能没有TNetHead_V2结构，传入NULL即可
    virtual int Encode(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                       unsigned char* pszCodeBuf, int iBufLen, int& iCodeLen) = 0;
};

#endif // __PROTOCOL_ENGINE_HPP__

