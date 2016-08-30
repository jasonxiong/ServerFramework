#ifndef __SS_PROTOCOL_ENGINE_HPP__
#define __SS_PROTOCOL_ENGINE_HPP__

#include "GameProtocol.hpp"

class CSSProtocolEngine
{
public:
    CSSProtocolEngine() {};
    int Initialize(const int iThreadIdx);
    int Decode(unsigned char* pszCodeBuf, const int iCodeLen, GameProtocolMsg* pstMsg);
    int Encode(GameProtocolMsg* pstMsg, unsigned char* pszCodeBuf, int iBufLen, int& iCodeLen);

private:
	int m_iThreadIdx;
};

#endif // __SS_PROTOCOL_ENGINE_HPP__


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
