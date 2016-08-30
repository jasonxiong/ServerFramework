
#ifndef __SESSION_DEF_HPP__
#define __SESSION_DEF_HPP__



const int NETHEAD_V2_SIZE = 24;

typedef struct tagNetHead_V2
{ 
	unsigned int	m_uiSocketFD; //!<套接字
	unsigned int	m_uiSocketTime;	//!<套接字创建时刻
	unsigned int	m_uiSrcIP; //!<源地址
	unsigned short	m_ushSrcPort; //!<源端口
	unsigned short	m_ushReservedValue01; //!<字节对齐，未用
	unsigned int	m_uiCodeTime; //!<消息时刻
	unsigned int	m_uiCodeFlag; //!<消息标志，用于实现套接字控制
}TNetHead_V2;



#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
