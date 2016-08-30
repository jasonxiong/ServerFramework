#ifndef __ACCOUNTDB_THREAD_HPP__
#define __ACCOUNTDB_THREAD_HPP__

#include <pthread.h>

#include "SSProtocolEngine.hpp"
#include "AccountDBHandlerSet.hpp"
#include "CodeQueue.hpp"

using namespace ServerLib;

// AccountDB线程
class CAccountDBThread
{
public:
    int Initialize(bool bResume, const int iThreadIdx);
	int InitBase();
	int InitCodeQueue(bool bResume, const int iThreadIdx);
    int Run();//thread 

public:
	int GetIdx() const {return m_iThreadIdx;}

public:
	//receive msg from incode queue
	int ReceiveMsg(int& riCodeLength);
	//process msg
	int ProcessMsg(SHandleResult& stMsgHandleResult);
	//send msg to main thread
	int EncodeAndSendMsg(SHandleResult& stMsgHandleResult);

private:
	int CreateThread();

public:
	//push code into queue
	int PushCode(const unsigned char* pMsg, int iCodeLength);
	//pop code
	int PopCode(unsigned char* pMsg, int iMaxLength, int& riLength);
	
public:
	CSSProtocolEngine* GetProtocolEngine() {return &m_stProtocolEngine;}

private:
    CSSProtocolEngine m_stProtocolEngine;
    CAccountDBHandlerSet m_stAccountDBHandlerSet;

private:
	CCodeQueue* m_pInputQueue; //输入消息队列，来自内部服务器，由主线程分发
	CCodeQueue* m_pOutputQueue; //输出消息队列
	//thread index in array
	int m_iThreadIdx;

private:
	pthread_t m_hTrd;
	pthread_attr_t m_stAttr;
	
private:
    GameProtocolMsg m_stGameMsg;
    unsigned char m_szCodeBuf[MAX_MSGBUFFER_SIZE];
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
