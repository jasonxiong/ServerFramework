#ifndef __PUBLIC_HPP__
#define __PUBLIC_HPP__

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"

typedef struct
{
    GameProtocolMsg stResponseMsg; // 响应消息
    int iNeedResponse;       // 是否需要回复：1表示需要回复，0表示不需要回复
} SHandleResult;

const int NETHEAD_V2_SIZE = 24;

typedef struct tagNetHead_V2
{
    unsigned int    m_uiSocketFD; //!<套接字
    unsigned int    m_uiSocketTime;    //!<套接字创建时刻
    unsigned int    m_uiSrcIP; //!<源地址
    unsigned short  m_ushSrcPort; //!<源端口
    unsigned short  m_ushReservedValue01; //!<字节对齐，未用
    unsigned int    m_uiCodeTime; //!<消息时刻
    unsigned int    m_uiCodeFlag; //!<消息标志，用于实现套接字控制
} TNetHead_V2;

const int MAX_MSGBUFFER_SIZE = 2048000;

const int MAX_PATH_LEN = 256;      // 绝对路径的最大长度
const int MAX_SHELL_CMD_LEN = 512; // shell命令的最大长度

#endif // __PUBLIC_HPP__
