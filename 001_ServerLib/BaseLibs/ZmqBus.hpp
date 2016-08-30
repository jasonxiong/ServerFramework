#ifndef __ZMQ_WRAPPER_HPP__
#define __ZMQ_WRAPPER_HPP__

#include "zmq.h"
#include "zmq_utils.h"

//对于ZMQ的封装，在游戏服务器中，所有的服务器内部通信都使用ZMQ

enum enZmqSocketType
{
    EN_ZMQ_SOCKET_PAIR = 1,             //ZMQ_PAIR
    EN_ZMQ_SOCKET_PUB = 2,              //ZMQ_PUB
    EN_ZMQ_SOCKET_SUB = 3,              //ZMQ_SUB
    EN_ZMQ_SOCKET_REQ = 4,              //ZMQ_REQ
    EN_ZMQ_SOCKET_REP = 5,              //ZMQ_REP
};

enum enZmqProcType
{
    EN_ZMQ_PROC_INPROC = 1,             //进程内通信
    EN_ZMQ_PROC_IPC = 2,                //进程间IPC通信
    EN_ZMQ_PROC_TCP = 3,                //TCP网络通信
};

enum enZmqServiceType
{
    EN_ZMQ_SERVICE_SERVER = 1,          //服务器端，调用bind
    EN_ZMQ_SERVICE_CLIENT = 2,          //客户端，调用connect
};

class ZmqBus
{
public:
    ZmqBus();
    ~ZmqBus();

    int ZmqInit(const char* pszAddress, enZmqSocketType eSocketType, enZmqProcType eProcType, enZmqServiceType eServiceType);

    int ZmqReinit();

    int ZmqSend(const char* pszBuff, int iMsgLen, int iFlags);

    int ZmqRecv(char* pszBuff, int iMaxLen, int& iMsgLen, int iFlags);

    void ZmqFin();

private:
    int GetSocketType(enZmqSocketType eSocketType);

    int GetRealSockAddr(enZmqProcType eProcType, const char* pszAddress);

private:
    //zmq context是线程安全的，所有线程公用一个
    static void* m_spZmqCtx;

    void* m_pZmqSocket;

    //绑定的地址
    char m_szSocketAddr[128];

    //实际绑定的地址，增加了ProcType的地址
    char m_szRealSockAddr[256];

    //连接的类型
    int m_iSocketType;

    //通信的进程模型类型
    int m_iProcType;

    //这条连接的类型，是服务器还是客户端
    int m_iServiceType;
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
