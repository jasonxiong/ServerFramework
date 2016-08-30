/**
*@file TCPConnector.hpp
*@author jasonxiong
*@date 2009-11-19
*@version 1.0
*@brief TCP连接类，管理TCP连接，负责TCP收发包
*
*	将TCP连接分为两类
*	（1）主动连接，需要调用connet去连接对方
*	（2）被动连接，通过监听后Accept获取到FD来创建连接
*/

#ifndef __TCP_CONNECTOR_HPP__
#define __TCP_CONNECTOR_HPP__

#include <time.h>
#include <sys/types.h>

#include "ErrorDef.hpp"
#include "CommonDef.hpp"

namespace ServerLib
{

const int MIN_TCP_USR_RECV_BUFFER_SIZE = 16 * 1024; //!<设置的TCP用户接收缓冲区的最小大小
const int MIN_TCP_USR_SEND_BUFFER_SIZE = 16 * 1024; //!<设置的TCP用户发送缓冲区的最小大小
const int MIN_TCP_SYS_RECV_BUFFER_SIZE = 16 * 1024; //!<设置的TCP系统接收缓冲区的最小大小
const int DEFAULT_TCP_SYS_RECV_BUFFER_SIZE = 2 * 1024 * 1024; //!<默认的TCP系统接受缓冲区大小
const int MIN_TCP_SYS_SEND_BUFFER_SIZE = 16 * 1024; //!<设置的TCP系统发送缓冲区的最小大小
const int DEFAULT_TCP_SYS_SEND_BUFFER_SIZE = 2 * 1024 * 1024; //!<默认的TCP系统发送缓冲区大小
const int DEFAULT_MAX_NOT_RECV_TIME = 300; //!<默认的最久未收包时间，到达这时间需要重连
const int MIN_NB_CONNECT_TIMEOUT = 1000; //!<最小的非阻塞连接超时时间，1毫秒
const int DEFAULT_NB_CONNECT_TIMEOUT = 1000000; //!<默认的非阻塞连接超时时间，1秒

typedef struct tagTCPConnectConfig
{
    int m_iSocketFD; //!<指定FD，仅在被动连接套接字中有效
    int m_iType; //!<TCP连接类型，目前0为主动连接套接字，1为被动连接套接字
    char m_szServerIP[MAX_IPV4_LENGTH]; //!<远端IP
    unsigned short m_ushServerPort; //!<远端Port
    char m_szBindIP[MAX_IPV4_LENGTH]; //!<本地IP，不绑定需要将m_szBindIP[0]='\0'
    unsigned short m_ushBindPort; //!<本地端口
    char m_cUseNBConnect; //!<使用非阻塞连接标志
    int m_iNBConnectTimeOut; //!<非阻塞方式连接时超时时间，单位us，仅在m_cUseNBConnect不为0时有效
    int m_iSysRecvBufSize; //!<系统接收缓存大小
    int m_iSysSendBufSize; //!<系统发送缓存大小
    int m_iMaxNotRecvTime; //!<最长未收包时间，保活机制
} TTCPConnectCfg;

/**
*@brief TCP连接状态
*
*
*/
typedef enum eTCPConnStates
{
    tcs_closed = 0, //!<连接已关闭
    tcs_opened = 1, //!<连接打开（创建）
    tcs_connected = 2, //!<已连接
    tcs_error = 3,  //!<连接出现错误
    tcs_code_mismatch = 4, //!<连接出现编解码错位
} ETCPCONNSTATES;

//!用户缓冲区分配方式
typedef enum enmUsrBufAllocType
{
    EUBT_ALLOC_BY_TCP_DEFAULT = 0,  //!<默认内置缓冲区
    EUBT_ALLOC_BY_TCP_CONNECTOR = 1, //!<由TCPConnector自行分配
    EUBT_ALLOC_BY_SHARED_MEMORY = 2, //!<由共享内存分配
} ENMTCPUSRBUFALLOCTYPE;

typedef enum eRecvErrs
{
    ERR_RECV_NOSOCK = -1,
    ERR_RECV_NOBUFF = -2,
    ERR_RECV_REMOTE = -3,
    ERR_RECV_FALIED = -4
} ERECVERRS;

typedef enum eSendErrs
{
    ERR_SEND_NOSOCK = -1,
    ERR_SEND_NOBUFF = -2,
    ERR_SEND_FAILED = -3,
} ESENDERRS;

/**
*@brief TCP连接类型
*
*
*/
typedef enum enmTCPConnectorType
{
    ETCT_CONNECT = 0, //!<主动连接
    ETCT_ACCEPTED = 1, //!<被动连接
} ENMTCPCONNECTORTYPE;

class CTCPConnector
{
public:
    /**
    * 采用默认内置缓冲区
    */
    CTCPConnector();

    /**
    *动态内存方式创建CTCPConnector的用户缓冲区
    *@param[in] iUsrRecvBufSize 用户接收缓冲区大小
    *@param[in] iUsrSendBufSize 用户发送缓冲区大小
    *@return 0 success
    */
    CTCPConnector(int iUsrRecvBufSize, int iUsrSendBufSize);
    ~CTCPConnector();

    /**
    *通过共享内存来创建CTempMemoryMng（注意这样创建的CTempMemoryMng不会初始化，不会改变所在共享内存的值）
    *@param[in] pszKeyFileName 共享内存Attach的文件名
    *@param[in] ucKeyPrjID 共享内存的ProjectID
    *@param[in] iUsrRecvBufSize 用户接收缓冲区大小
    *@param[in] iUsrSendBufSize 用户发送缓冲区大小
    *@return 0 success
    */
    static CTCPConnector* CreateBySharedMemory(const char* pszKeyFileName,
            const unsigned char ucKeyPrjID,
            int iUsrRecvBufSize, int iUsrSendBufSize);

public:
    /**
    *主动连接时初始化
    *@param[in] pszServerIP 要连接的服务器IP
    *@param[in] ushServerPort 要连接的服务器端口
    *@param[in] pszBindIP 要绑定的本地IP，可以为NULL，表示不绑定
    *@param[in] ushBindPort 要绑定的本地端口
    *@return 0 success
    */
    int Initialize(const char* pszServerIP, unsigned short ushServerPort,
                   const char* pszBindIP, unsigned short ushBindPort);

    /**
    *被动连接时初始化
    *@param[in] iFD accept获取的FD
    *@param[in] pszRemoteIP 发起连接的远端IP
    *@param[in] ushRemotePort 发起连接的远端端口
    *@return 0 success
    */
    int Initialize(int iFD, const char* pszRemoteIP, unsigned short ushRemotePort);

    /**
    *连接初始化（完全配置）
    *@param[in] rstTCPConfig TCP连接配置
    *@return 0 success
    */
    int Initialize(const TTCPConnectCfg& rstTCPConfig);

    //!检查连接是否正常
    int CheckActive();

    //!重新建立连接
    int ReConnect();

    //!关闭连接
    int CloseSocket();

    /**
    *在连接套接字有网卡数据未接收时，调用RecvData函数来接收数据到用户接收缓冲区
    *@return 0 success
    */
    int RecvData();

    /**
    *连接套接字调用这个函数发送数据到远端FD
    *@param[in] pszCodeBuf 发送的数据Buf
    *@param[in] iCodeLength 发送的数据长度
    *@return 0 success
    */
    int SendData(const unsigned char* pszCodeBuf, int iCodeLength);

    /**
    *获取数据指针和数据长度
    *@param[out] rpcDataBeg 获取数据开始位置
    *@param[out] riDataLenth 接收到的数据长度，为0表示没有数据
    *@return 0 success
    */
    int GetData(unsigned char*& rpcDataBeg, int& riDataLenth);

    /**
    *清空接收缓冲区，从头开始清空指定大小
    *@param[in] iFlushLength 需要清空的缓冲区大小，为-1时表示全部清空
    *@return 0 success
    */
    int FlushRecvBuf(int iFlushLength);

    //!获取SocketFD
    int GetSocketFD() const
    {
        return m_iSocketFD;
    }

    //!获取当前连接状态
    int GetStatus() const
    {
        return m_iStatus;
    }
    //连接被关闭
    void SetClosed()
    {
        m_iStatus = tcs_closed;
    }
    //判断是否连接
    bool IsConnected() const
    {
        return m_iStatus == tcs_connected;
    }

    //!获取系统接收缓冲区大小
    int GetSysRecvBufSize() const
    {
        return m_iSysRecvBufSize;
    }

    //!获取系统发送缓冲区大小
    int GetSysSendBufSize() const
    {
        return m_iSysSendBufSize;
    }

    //!获取用户接收缓冲区大小
    int GetUsrRecvBufSize() const
    {
        return m_iUsrRecvBufSize;
    }

    //!获取用户发送缓冲区大小
    int GetUsrSendBufSize() const
    {
        return m_iUsrSendBufSize;
    }

    //!获取配置
    const TTCPConnectCfg& GetConnectConfig() const
    {
        return m_stConnectCfg;
    }

    //!在接口返回错误时，调用这个函数获取错误号
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    //!创建Socket
    int CreateSocket();

    //!创建主动连接
    int CreateConnector();

    //!连接到服务器
    int ConnectToServer();

    //!创建被动连接
    int CreateAcceptor();

    //!设置错误号
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

    //!回收用户缓冲区
    int FreeUsrBuffer();

private:
    int m_iSocketFD; //!<连接的SocketFD
    int m_iStatus; //!<连接状态
    int m_iErrorNO; //!错误码

#ifdef _POSIX_MT_
    pthread_mutex_t m_stMutex;			//操作的互斥变量
#endif

    TTCPConnectCfg m_stConnectCfg; //连接配置

    int m_iSysRecvBufSize; //!<系统接收缓存大小
    int m_iSysSendBufSize; //!<系统发送缓存大小

    int m_iUsrBufAllocType; //!<用户缓冲区分配方式
    int m_iUsrRecvBufSize; //!<用户进程接收缓存大小
    int m_iReadBegin; //!<用户进程接收缓存头指针
    int m_iReadEnd; //!<用户进程接收缓存末尾指针
    unsigned char* m_abyRecvBuffer; //!<用户接收缓冲区
    int m_iUsrSendBufSize; //!<用户进程发送缓存大小
    int m_iPostBegin; //!<用户进程发送缓存头指针
    int m_iPostEnd; //!<用户进程发送缓存末尾指针
    unsigned char* m_abyPostBuffer; //!<用户发送缓冲区

    time_t m_tLastRecvPacketTime; //!<上一次接收到数据的时间

    static const int MAX_BUFFER_LENGTH = 4096000;
    unsigned char m_szRecvBufferContent[MAX_BUFFER_LENGTH];
    unsigned char m_szPostBufferContent[MAX_BUFFER_LENGTH];
};

}

#endif //__TCP_CONNECTOR_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
