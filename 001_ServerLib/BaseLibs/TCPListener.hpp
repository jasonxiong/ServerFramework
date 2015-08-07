/**
*@file TCPListener.hpp
*@author jasonxiong
*@date 2009-11-19
*@version 1.0
*@brief TCP连接监听类
*
*
*/

#ifndef __TCP_LISTENER_HPP__
#define __TCP_LISTENER_HPP__

#include "ErrorDef.hpp"
#include "CommonDef.hpp"

namespace ServerLib
{

typedef struct tagTCPListenerConfig
{
    char m_szBindIP[MAX_IPV4_LENGTH]; //!<本地IP
    unsigned short m_ushBindPort; //!<本地端口
    int m_iBacklogFDNum; //!<在监听队列中积累的FD个数
} TTCPListenerCfg;

const int MIN_BACKLOG_FD_NUMBER = 1; //!<在监听时允许积累的FD最小数目
const int DEFAULT_BACKLOG_FD_NUMBER = 10; //!<默认监听时允许积累的FD数目
const int MAX_BACKLOG_FD_NUMBER = 1024; //!<在监听时允许积累的FD最大数目

typedef enum eTCPListenerStates
{
    tls_closed = 0, //!<已关闭
    tls_opened = 1, //!<打开监听
    tls_error = 2,  //!<出现错误
} ETCPLISTENSTATES;

class CTCPListener
{
public:
    CTCPListener();
    ~CTCPListener();

public:
    /**
    *简单初始化
    *@param[in] pszBindIP 监听要绑定的IP，可以传NULL
    *@param[in] ushBindPort 监听要绑定的端口
    *@return 0 success
    */
    int Initialize(const char* pszBindIP, const unsigned short ushBindPort);

    ///**
    //*简单初始化
    //*@param[in] uiBindIP 监听要绑定的IP
    //*@param[in] ushBindPort 监听要绑定的端口
    //*@return 0 success
    //*/
    //int Initialize(const unsigned int uiBindIP, const unsigned short ushBindPort);

    //!初始化监听配置
    int Initialize(const TTCPListenerCfg& rstListenerConfig);

    //!关闭连接
    int CloseSocket();

    //!获取监听套接字
    int GetSocketFD() const
    {
        return m_iSocketFD;
    }

    /**
    *接受一个连接
    *@param[out] riAcceptFD 接受的连接FD
    *@param[out] ruiIP 接受的连接IP
    *@param[out] rushPort 接受的连接端口
    *@return 0 success
    */
    int Accept(int& riAcceptFD, unsigned int& ruiIP, unsigned short& rushPort);

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

private:
    int m_iSocketFD;
    int m_iStatus; //!<连接状态
    TTCPListenerCfg m_stTCPListenConfig;
    int m_iErrorNO; //!错误码

};

}

#endif //__TCP_LISTENER_HPP__
///:~
