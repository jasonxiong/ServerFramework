#include <string.h>
#include <arpa/inet.h>

#include "Handler.hpp"
#include "AccountSingleton.hpp"

int  IHandler::EncodeAndSendCode(IProtocolEngine* pProtocolEngine,
                         TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg ,EGameServerID enMsgPeer)
{
//  ASSERT_AND_LOG_RTN_INT(pProtocolEngine);
//  ASSERT_AND_LOG_RTN_INT(pstNetHead);
//  ASSERT_AND_LOG_RTN_INT(pstMsg);

    unsigned char szCodeBuf[MAX_CODE_LEN];
    int iBufLen = sizeof(szCodeBuf);
    int iCodeLen;

    // 初始化code buffer和code length
    memset(szCodeBuf, 0, sizeof(szCodeBuf));
    iCodeLen = 0;

    int iInstanceID = 0;
    if (GAME_SERVER_LOTUSACCOUNT == enMsgPeer)
    {
        TNetHead_V2 stTmpNetHead = *pstNetHead;
        //socket id 做转换
        unsigned  int uiSessionID = ntohl(pstNetHead->m_uiSocketFD);
        iInstanceID = uiSessionID / MAX_FD_NUMBER;
        stTmpNetHead.m_uiSocketFD = htonl(uiSessionID % MAX_FD_NUMBER);
        pProtocolEngine->Encode(&stTmpNetHead, pstMsg,
                (unsigned char*)szCodeBuf, iBufLen, iCodeLen);

    }
    else
    {
        // 编码本地数据为网络数据
        pProtocolEngine->Encode(pstNetHead, pstMsg,
                (unsigned char*)szCodeBuf, iBufLen, iCodeLen);
    }
    // 发送网络数据
    int iRes = AccountMsgTransceiver->SendOneMsg((char*)szCodeBuf, iCodeLen, enMsgPeer, iInstanceID);

    if (iRes < 0)
    {
        return -1;
    }

    return 0;
}

void IHandler::GenerateMsgHead(GameProtocolMsg* pstAccountMsg,
                               unsigned int uiSessionFd, unsigned int uiMsgID, unsigned int uiUin)
{
    ASSERT_AND_LOG_RTN_VOID(pstAccountMsg);
    pstAccountMsg->Clear();

    GameCSMsgHead* pstMsgHead = pstAccountMsg->mutable_m_stmsghead();
    pstMsgHead->set_m_uisessionfd(uiSessionFd);
    pstMsgHead->set_m_uimsgid((ProtocolMsgID)uiMsgID);
    pstMsgHead->set_m_uin(uiUin);

    return;
}
