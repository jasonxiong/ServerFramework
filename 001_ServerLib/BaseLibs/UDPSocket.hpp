/**
*@file UDPSocket.hpp
*@author jasonxiong
*@date 2010-01-26
*@version 1.0
*@brief 实现UDP套接字
*
*
*/

#ifndef __UDP_SOCKET_HPP__
#define __UDP_SOCKET_HPP__

#include <time.h>
#include <sys/types.h>

#include "CommonDef.hpp"

namespace ServerLib
{

typedef struct tagUDPSocketConfig
{
    char m_szServerIP[MAX_IPV4_LENGTH]; //!<远端IP
    unsigned short m_ushServerPort; //!<远端Port
    char m_szBindIP[MAX_IPV4_LENGTH]; //!<本地IP，不绑定需要将m_szBindIP[0]='\0'
    unsigned short m_ushBindPort; //!<本地端口
    int m_iSysRecvBufSize; //!<系统接收缓存大小
    int m_iSysSendBufSize; //!<系统发送缓存大小
} TUDPSocketConfig;

//!用户缓冲区分配方式
typedef enum enmUDPRecvBufAllocType
{
    EURT_ALLOC_BY_UDP_DEFAULT = 0, //!<默认内置缓存区
    EURT_ALLOC_BY_UDP_SOCKET = 1, //!<由CUDPSocket自行分配
    EURT_ALLOC_BY_SHARED_MEMORY = 2, //!<由共享内存分配
} ENMUDPUSRBUFALLOCTYPE;

/**
*@brief UDP连接状态
*
*
*/
typedef enum enmUDPSocketStates
{
    EUSS_CLOSED = 0, //!<连接已关闭
    EUSS_OPENED = 1, //!<连接打开（创建）
    EUSS_ERROR = 2,  //!<连接出现错误
} ENMUDPSOKCETSTATES;

const int MIN_UDP_SYS_RECV_BUFFER_SIZE = 16 * 1024; //!<设置的UDP系统接收缓冲区的最小大小
const int DEFAULT_UDP_SYS_RECV_BUFFER_SIZE = 2 * 1024 * 1024; //!<默认的UDP系统接受缓冲区大小
const int MIN_UDP_SYS_SEND_BUFFER_SIZE = 16 * 1024; //!<设置的UDP系统发送缓冲区的最小大小
const int DEFAULT_UDP_SYS_SEND_BUFFER_SIZE = 2 * 1024 * 1024; //!<默认的UDP系统发送缓冲区大小

class CUDPSocket
{
public:
    ~CUDPSocket();

public:
    /**
    * 采用默认内置数据缓冲区
    */
    CUDPSocket();

    /**
    *动态内存方式创建CUDPSocket的用户缓冲区
    *@param[in] iUsrRecvBufSize 用户接收缓冲区大小
    *@return 0 success
    */
    CUDPSocket(int iUsrRecvBufSize);

    /**
    *通过共享内存来创建CUDPSocket（注意这样创建的CUDPSocket不会初始化，不会改变所在共享内存的值）
    *@param[in] pszKeyFileName 共享内存Attach的文件名
    *@param[in] ucKeyPrjID 共享内存的ProjectID
    *@param[in] iUsrRecvBufSize 用户接收缓冲区大小
    *@return 0 success
    */
    static CUDPSocket* CreateBySharedMemory(const char* pszKeyFileName,
                                            const unsigned char ucKeyPrjID, int iUsrRecvBufSize);

public:
    /**
    *初始化UDP套接字
    *@param[in] pszServerIP 要连接的服务器IP
    *@param[in] ushServerPort 要连接的服务器端口
    *@param[in] pszBindIP 要绑定的本地IP，可以为NULL，表示不绑定
    *@param[in] ushBindPort 要绑定的本地端口
    *@return 0 success
    */
    int Initialize(const char* pszServerIP, unsigned short ushServerPort,
                   const char* pszBindIP, unsigned short ushBindPort);

    /**
    *初始化UDP套接字（完全配置）
    *@param[in] rstUDPConfig UDP连接配置
    *@return 0 success
    */
    int Initialize(const TUDPSocketConfig& rstUDPConfig);

    //!关闭套接字
    int CloseSocket();

    /**
    *在连接套接字有网卡数据未接收时，调用RecvData函数来接收数据到用户接收缓冲区
    *@param[out] pszSrcIP 数据的发送方IP
    *@param[out] pushSrcPort 数据的发送方Port
    *@return 0 success
    */
    int RecvData(char* pszSrcIP = NULL, unsigned short* pushSrcPort = NULL);

    /**
    *对之前初始化时的ServerIP发送数据
    *@param[in] pszCodeBuf 发送的数据Buf
    *@param[in] iCodeLength 发送的数据长度
    *@return 0 success
    */
    int SendData(const unsigned char* pszCodeBuf, int iCodeLength);

    /**
    *指定目的地调用这个函数发送数据到远端FD
    *@param[in] pszCodeBuf 发送的数据Buf
    *@param[in] iCodeLength 发送的数据长度
    *@param[in] pszDstIP 目的IP
    *@param[in] ushDstPort 目的端口
    *@return 0 success
    */
    int SendDataTo(const unsigned char* pszCodeBuf, int iCodeLength, const char* pszDstIP, unsigned short ushDstPort);

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

    //!获取配置
    const TUDPSocketConfig& GetConnectConfig() const
    {
        return m_stUDPSocketCfg;
    }

    //!在接口返回错误时，调用这个函数获取错误号
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    //!创建Socket
    int CreateSocket();

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

    TUDPSocketConfig m_stUDPSocketCfg; //连接配置

    int m_iSysRecvBufSize; //!<系统接收缓存大小
    int m_iSysSendBufSize; //!<系统发送缓存大小

    int m_iUsrBufAllocType; //!<用户缓冲区分配方式
    int m_iUsrRecvBufSize; //!<用户进程接收缓存大小
    int m_iReadBegin; //!<用户进程接收缓存头指针
    int m_iReadEnd; //!<用户进程接收缓存末尾指针
    unsigned char* m_abyRecvBuffer; //!<用户接收缓冲区

    static const int MAX_BUFFER_LENGTH = 40960;
    unsigned char m_szBufferContent[MAX_BUFFER_LENGTH];
};

}


#endif //__UDP_SOCKET_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
