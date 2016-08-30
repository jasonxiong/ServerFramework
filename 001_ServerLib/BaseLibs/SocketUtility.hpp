/**
*@file SocketUtility.hpp
*@author jasonxiong
*@date 2009-11-05
*@version 1.0
*@brief 处理套接字相关操作
*
*
*/

#ifndef __SOCKET_UTIL_HPP__
#define __SOCKET_UTIL_HPP__

//在这添加标准库头文件
#include <netinet/in.h>

namespace ServerLib
{
class CSocketUtility
{
private:
    CSocketUtility() {}
    ~CSocketUtility() {}

public:
    /**
    *设置套接字为非阻塞
    *@param[in] iSocketFD 需要设置的套接字
    *@return 0 success
    */
    static int SetNBlock(int iSocketFD);

    /**
    *设置套接字Nagle算法关闭
    *@param[in] iSocketFD 需要设置的套接字
    *@return 0 success
    */
    static int SetNagleOff(int iSocketFD);

    /**
    *设置地址重用
    *@param[in] iSocketFD 需要设置地址重用的FD
    *@return 0 success
    */
    static int SetReuseAddr(int iSocketFD);

    /**
    *设置TCP保活
    *@param[in] iSocketFD 需要设置保活的FD
    *@return 0 success
    */
    static int SetKeepalive(int iSocketFD);

    /**
    *禁止延迟关闭
    *@param[in] iSocketFD 需要设置保活的FD
    *@return 0 success
    */
    static int SetLingerOff(int iSocketFD);

    /**
    *将Socket绑定IP
    *@param[in] iSocketFD
    *@param[in] pszBindIP 要绑定的IP地址
    *@param[in] ushBindPort 要绑定的端口地址
    *@return 0 success
    */
    static int BindSocket(int iSocketFD, const char* pszBindIP, unsigned short ushBindPort);

    /**
    *连接到远端IP
    *@param[in] iSocketFD
    *@param[in] pszBindIP 要连接的IP地址
    *@param[in] ushBindPort 要连接的端口地址
    *@return 0 success
    */
    static int Connect(int iSocketFD, const char* pszServerIP, unsigned short ushServerPort);

    /**
    *设置套接字所占的系统接收缓冲区大小
    *@param[in] iSocketFD 需要设置的套接字
    *@param[in] riRecvBufLen 要设置的系统接收缓冲区大小
    *@param[out] riRecvBufLen 设置后的系统接收缓冲区大小
    *@return 0 success
    */
    static int SetSockRecvBufLen(int iSocketFD, int& riRecvBufLen);

    /**
    *设置套接字所占的系统发送缓冲区大小
    *@param[in] iSocketFD 需要设置的套接字
    *@param[in] riSendBufLen 要设置的系统发送缓冲区大小
    *@param[out] riSendBufLen 设置后的系统发送缓冲区大小
    *@return 0 success
    */
    static int SetSockSendBufLen(int iSocketFD, int& riSendBufLen);

    /**
    *将用sockaddr_in表示的IP地址转换为字符串
    *@param[in] pstSockAddr 要转换的地址
    *@param[out] szAddr 用字符串表示的IPv4:port地址
    *@return 0 success
    */
    static int SockAddrToString(sockaddr_in *pstSockAddr, char *szAddr);

    /**
    *将用int32表示的IP地址转换为字符串
    *@param[in] iIPAddr int32表示的IP地址
    *@param[in] szAddr 点分十进制表示的IP地址
    *@return 0 success
    */
    static int IPInt32ToString(int iIPAddr, char* szAddr);
};
}

#endif //__SOCKET_UTIL_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
