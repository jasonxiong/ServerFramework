#include <assert.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "SSProtocolEngine.hpp"
#include "ConfigHelper.hpp"

int S2SProtocolEngine::Initialize()
{
    m_eType = PT_SS;
    return 0;
}

int S2SProtocolEngine::Decode(unsigned char* pszCodeBuf, const int iCodeLen,
                              TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg)
{
    // 不使用NetHead
    ASSERT_AND_LOG_RTN_INT(pszCodeBuf);
    ASSERT_AND_LOG_RTN_INT(pstMsg);

    // 网络数据
    int iBuffLen = iCodeLen - sizeof(unsigned short);
    char* pszBuff = (char*)pszCodeBuf + sizeof(unsigned short);

    // 解码
    bool bRet = pstMsg->ParseFromArray(pszBuff, iBuffLen);
    if(!bRet)
    {
        TRACESVR("fail to decode protobuf msg from array!\n");
        return -1;
    }

    return 0;
}

int S2SProtocolEngine::Encode(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                              unsigned char* pszCodeBuf, int iBufLen, int& iCodeLen)
{
    // 不使用NetHead
    ASSERT_AND_LOG_RTN_INT(pstMsg);
    ASSERT_AND_LOG_RTN_INT(pszCodeBuf);

    bool bRet = pstMsg->SerializeToArray((char*)pszCodeBuf+sizeof(unsigned short), iBufLen);
    if(!bRet)
    {
        TRACESVR("fail to serialize proto msg to array!\n");
        return -1;
    }

    iCodeLen = pstMsg->ByteSize() + sizeof(unsigned short);
    pszCodeBuf[0] = iCodeLen / 256;
    pszCodeBuf[1] = iCodeLen % 256;

    return 0;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
