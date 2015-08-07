#include "RoleDBLogManager.hpp"
#include "RoleDBApp.hpp"
#include "SSProtocolEngine.hpp"
#include "StringUtility.hpp"
#include "ConfigHelper.hpp"

int CSSProtocolEngine::Initialize(const int iThreadIdx)
{
	m_iThreadIdx = iThreadIdx;

    return 0;
}

int CSSProtocolEngine::Decode(unsigned char* pszCodeBuf, const int iCodeLen, GameProtocolMsg* pstMsg)
{
    // 不使用NetHead
    if (!pszCodeBuf || !pstMsg)
	{
		return -1;
	}

	// 网络数据,跳过2字节len + 4字节 uin
	int iBuffLen = iCodeLen - sizeof(unsigned short) - sizeof(unsigned int);
	char* pszBuff = (char*)pszCodeBuf + sizeof(unsigned short) + sizeof(unsigned int);   

    DEBUG_THREAD(m_iThreadIdx, "Decode Buff len: %d\n", iBuffLen);

	//解码
	bool bRet = pstMsg->ParseFromArray(pszBuff, iBuffLen);
	if(!bRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Fail to parse proto from buff!\n");
		return -2;
	}

	DEBUG_THREAD(m_iThreadIdx, "Success to parse proto data!\n");

    return 0;
}

int CSSProtocolEngine::Encode(GameProtocolMsg* pstMsg, unsigned char* pszCodeBuf, int iBufLen, int& iCodeLen)
{
    // 不使用NetHead
    if (!pstMsg)
	{
		return -1;
	}

    if (!pszCodeBuf)
	{
		return -3;
	}

	//对protobuf进行编码
	int iProtoMsgLen = pstMsg->ByteSize();
	if(iProtoMsgLen >= iBufLen)
	{
		TRACE_THREAD(m_iThreadIdx, "Fail to parse from data array, real size %d, buff size %d\n", pstMsg->ByteSize(), iBufLen);
		return -4;
	}

    // 编码数据
	bool bRet = pstMsg->SerializeToArray(pszCodeBuf+sizeof(unsigned short), iBufLen);
	if(!bRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Fail to serialize proto to array!\n");
		return -5;
	}

	iCodeLen = iProtoMsgLen + sizeof(unsigned short);
	pszCodeBuf[0] = iCodeLen / 256;
	pszCodeBuf[1] = iCodeLen % 256;

	DEBUG_THREAD(m_iThreadIdx, "Success to serialize proto to array!\n");

    return 0;
}
