#include "AccountDBThread.hpp"
#include "StringUtility.hpp"
#include "AccountDBLogManager.hpp"
#include "AccountDBApp.hpp"
#include "AppDef.hpp"

void* ThreadProc( void *pvArgs )
{
	if( !pvArgs )
	{
		return NULL;
	}

	CAccountDBThread *pThread = (CAccountDBThread *)pvArgs;

	//run the thread logic
	pThread->Run();

	return NULL;
}

int CAccountDBThread::Initialize(bool bResume, const int iThreadIdx)
{
	int iRt = 0;

	//初始化线程id
	m_iThreadIdx = iThreadIdx;	

	iRt = InitBase();
	if (iRt != 0)
	{
		return -1;
	}

	//init codequeue
	iRt = InitCodeQueue(bResume, iThreadIdx);
	if (iRt != 0)
	{
		return -4;
	}

	iRt = m_stProtocolEngine.Initialize(iThreadIdx);
	if (iRt != 0)
	{
		return -6;
	}

	iRt = m_stAccountDBHandlerSet.Initialize(iThreadIdx);
	if (iRt != 0)
	{
		return -9;
	}

	//create thread
	iRt = CreateThread();
	if (iRt != 0)
	{
		return -12;
	}
		
	return 0;
}

int CAccountDBThread::CreateThread()
{
	int iRt = 0;
	iRt = pthread_attr_init(&m_stAttr);
	if (iRt != 0)
	{
		TRACE_THREAD(m_iThreadIdx, "Error: pthread_attr_init fail. rt:%d\n", iRt);
		return -1;
	}

	// to complete with system thread
	iRt = pthread_attr_setscope(&m_stAttr, PTHREAD_SCOPE_SYSTEM);
	if (iRt != 0)
	{
		TRACE_THREAD(m_iThreadIdx, "Error: pthread_attr_setscope fail. rt:%d\n", iRt);
		return -3;
	}
	
	iRt = pthread_attr_setdetachstate(&m_stAttr, PTHREAD_CREATE_JOINABLE);
	if (iRt != 0)
	{
		TRACE_THREAD(m_iThreadIdx, "Error: pthread_attr_setdetachstate fail. rt:%d\n", iRt);
		return -5;
	}

	iRt = pthread_create(&m_hTrd, &m_stAttr, ThreadProc, (void *)this);
	if (iRt != 0)
	{
		TRACE_THREAD(m_iThreadIdx, "Error: pthread_create fail. rt:%d\n", iRt);
		return -7;
	}
	
	return 0;
}

int CAccountDBThread::InitBase()
{
	//memset some struct
	memset(m_szCodeBuf, 0, sizeof(m_szCodeBuf));

	return 0;
}

int CAccountDBThread::InitCodeQueue(bool bResume, const int iThreadIdx)
{
	//initialize codequeue


	const int SHMPATHLEN = 256;
	const int CODEQUEUESIZE = 16777216;
	
	//in codequeue
	char szInShmPath[SHMPATHLEN]={};
	SAFE_SPRINTF(szInShmPath, sizeof(szInShmPath) - 1, "roledbthreadin%d.shm", m_iThreadIdx);
	m_pInputQueue = CCodeQueue::CreateBySharedMemory(szInShmPath, m_iThreadIdx, CODEQUEUESIZE);
	if (!m_pInputQueue)
	{
		TRACE_THREAD(m_iThreadIdx, "Error: init input queue fail. thread idx:%d\n", m_iThreadIdx);
		return -1;
	}

	m_pInputQueue->Initialize(bResume);

	//out codequeue
	char szOutShmPath[SHMPATHLEN]={};
	SAFE_SPRINTF(szOutShmPath, sizeof(szOutShmPath) - 1, "roledbthreadout%d.shm", m_iThreadIdx);
	m_pOutputQueue = CCodeQueue::CreateBySharedMemory(szOutShmPath, m_iThreadIdx, CODEQUEUESIZE);
	if (!m_pOutputQueue)
	{
		TRACE_THREAD(m_iThreadIdx, "Error: init output queue fail. thread idx:%d\n", m_iThreadIdx);
		return -3;
	}

	m_pOutputQueue->Initialize(bResume);

	return 0;
}
	
	
int CAccountDBThread::Run()
{
	int iRet = 0;
	int iCodeLen = 0;

    SHandleResult stMsgHandleResult;

	while (true)
	{
		//process all msg
		while (true)
		{
			iRet = ReceiveMsg(iCodeLen);
			//receive one msg
			if (0 == iRet)
			{
				iRet = m_stProtocolEngine.Decode((unsigned char*)m_szCodeBuf, iCodeLen, &m_stGameMsg);
				//decode success
				if (0 == iRet)
				{
					DEBUG_THREAD(m_iThreadIdx, "Receive code OK, Uin = %u, Msg = %d\n", 
						m_stGameMsg.m_stmsghead().m_uin(),
						m_stGameMsg.m_stmsghead().m_uimsgid());

					stMsgHandleResult.iNeedResponse = 0;
					stMsgHandleResult.stResponseMsg.Clear();

					iRet = ProcessMsg(stMsgHandleResult);
					if (0 == iRet)
					{					
						// 不需要回复
						if (!stMsgHandleResult.iNeedResponse)
						{
							continue;
						}

						iRet = EncodeAndSendMsg(stMsgHandleResult);
						if (iRet != 0)
						{
							TRACE_THREAD(m_iThreadIdx, "Error: encode and push fail. rt:%d\n", iRet);
						}
					}
				}
			}
			else
			{
				//no msg in
				break;
			}
		}
		
		usleep(APP_ACCOUNTDB_MAX_SLEEP_USEC);
	}

	return 0;
}

int CAccountDBThread::ReceiveMsg(int& riCodeLength)
{
	int iRet = 0;
	//receive new msg from main thread
	int iMaxLength = MAX_MSGBUFFER_SIZE - 1;
	iRet = m_pInputQueue->PopOneCode(m_szCodeBuf, iMaxLength, riCodeLength);

	if(iRet < 0 || riCodeLength <= (int)sizeof(int))
	{
		return -1;
	}

	DEBUG_THREAD(m_iThreadIdx, "Debug: receive code. len:%d\n", riCodeLength);
	return 0;
}


int CAccountDBThread::ProcessMsg(SHandleResult& stMsgHandleResult)
{
	//process the msg
	CHandler* pHandler = m_stAccountDBHandlerSet.GetHandler(m_stGameMsg.m_stmsghead().m_uimsgid());
	if (!pHandler)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to find a message handler, msg id: %u\n", m_stGameMsg.m_stmsghead().m_uimsgid());
		return -1;
	}

	stMsgHandleResult.iNeedResponse = 1; // 默认需要回复
	//m_stMsgHandleResult.stResponseMsg.mutable_m_stmsghead()->set_m_uimsgid(0);
	pHandler->OnClientMsg(&m_stGameMsg, &stMsgHandleResult);
	
	return 0;
}

int CAccountDBThread::EncodeAndSendMsg(SHandleResult& stHandleResult)
{
	int iBufLen = sizeof(m_szCodeBuf);
	int iCodeLen = 0;
	int iRet = 0;

	// 编码本地数据为网络数据
	iRet = m_stProtocolEngine.Encode(&stHandleResult.stResponseMsg, (unsigned char*)m_szCodeBuf, iBufLen, iCodeLen);
	if (iRet != 0)
	{
		TRACE_THREAD(m_iThreadIdx, "Error: encode fail. rt:%d\n", iRet);
		return -1;
	}


	// 压入队列给主线程
	iRet = m_pOutputQueue->PushOneCode((const unsigned char*)m_szCodeBuf, iCodeLen);
	if (iRet != 0)
	{
		TRACE_THREAD(m_iThreadIdx, "Error: push code fail. rt:%d\n", iRet);
		return -3;
	}

	DEBUG_THREAD(m_iThreadIdx, "Push code into main thread. MsgID = %d, Uin = %u, len:%d, idx:%d\n", 
				 stHandleResult.stResponseMsg.m_stmsghead().m_uimsgid(),
				 stHandleResult.stResponseMsg.m_stmsghead().m_uin(),
				 iCodeLen, m_iThreadIdx);

	return 0;
}
	
int CAccountDBThread::PushCode(const unsigned char* pMsg, int iCodeLength)
{
	if (!pMsg)
	{
		return -1;
	}
	
	return m_pInputQueue->PushOneCode(pMsg, iCodeLength);
}


int CAccountDBThread::PopCode(unsigned char* pMsg, int iMaxLength, int& riLength)
{
	if (!pMsg)
	{
		return -1;
	}
	
	return m_pOutputQueue->PopOneCode(pMsg, iMaxLength, riLength);
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
