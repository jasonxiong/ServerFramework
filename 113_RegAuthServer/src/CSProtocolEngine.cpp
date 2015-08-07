#include <assert.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "CSProtocolEngine.hpp"
#include "ConfigHelper.hpp"

int C2SProtocolEngine::Initialize()
{
    m_eType = PT_CS;
    return 0;
}

int C2SProtocolEngine::Decode(unsigned char* pszCodeBuf, const int iCodeLen,
                              TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg)
{
    ASSERT_AND_LOG_RTN_INT(pszCodeBuf);
    ASSERT_AND_LOG_RTN_INT(pstNetHead);
    ASSERT_AND_LOG_RTN_INT(pstMsg);

    // 填充NetHead
    memcpy(pstNetHead, pszCodeBuf, NETHEAD_V2_SIZE);

    // 空包，客户端连接断开
    if (NETHEAD_V2_SIZE == iCodeLen)
    {
        pstMsg->mutable_m_stmsghead()->set_m_uimsgid(MSGID_LOGOUTSERVER_REQUEST);

        LOGDEBUG("Received empty message, client session closed\n");
        return 0;
    }

    int iRealCodeOffset = NETHEAD_V2_SIZE + sizeof(unsigned short);

    // 网络数据
    int iBuffLen = iCodeLen - iRealCodeOffset;
    char* pszBuff = (char*)pszCodeBuf + iRealCodeOffset;

    // 解码
    if(!pstMsg->ParseFromArray(pszBuff, iBuffLen))
    {
        TRACESVR("fail to parse proto msg from msgbuff!\n");
        return -1;
    }

    return 0;
}

int C2SProtocolEngine::Encode(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                              unsigned char* pszCodeBuf, int iBufLen, int& iCodeLen)
{
    ASSERT_AND_LOG_RTN_INT(pstNetHead);
    ASSERT_AND_LOG_RTN_INT(pstMsg);
    ASSERT_AND_LOG_RTN_INT(pszCodeBuf);

    // 编码NetHead
    memcpy(pszCodeBuf, pstNetHead, NETHEAD_V2_SIZE);

    //缓冲区信息
    int iRealCodeOffset = NETHEAD_V2_SIZE + sizeof(unsigned short);
    char* pszRealBuff = (char*)pszCodeBuf + iRealCodeOffset;

    //编码数据
    iCodeLen = pstMsg->ByteSize();
    if(!pstMsg->SerializeToArray(pszRealBuff, iBufLen))
    {
        TRACESVR("fail to encode protobuf to array!\n");
        return -1;
    }

    int iCodeLengthExceptNetHead = iCodeLen + sizeof(unsigned short);
    pszCodeBuf[NETHEAD_V2_SIZE] = iCodeLengthExceptNetHead / 256;
    pszCodeBuf[NETHEAD_V2_SIZE + 1] = iCodeLengthExceptNetHead % 256;

    iCodeLen = iCodeLengthExceptNetHead + NETHEAD_V2_SIZE;

    return 0;
}
