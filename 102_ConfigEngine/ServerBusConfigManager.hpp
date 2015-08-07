#ifndef __SERVER_BUS_CONFIG_MANAGER_HPP__
#define __SERVER_BUS_CONFIG_MANAGER_HPP__

#include <stdint.h>

#include "GameConfigDefine.hpp"

using namespace GameConfig;

//从 conf/GameServer.tcm 文件中读取服务器通信BUS的配置

struct ServerBusConfig
{
    uint64_t ullBusClientID;                    //ZMQ 通信通道Client的ID
    uint64_t ullBusServerID;                    //ZMQ通信通道Server的ID
    char szBusAddr[MAX_SERVER_BUS_ADDR_LEN];    //ZMQ通信通道的实际地址，格式为 ip:port

    ServerBusConfig()
    {
        memset(this, 0, sizeof(*this));
    }
};

class CServerBusConfigManager
{
public:
    CServerBusConfigManager() { memset(this, 0, sizeof(*this)); };
    ~CServerBusConfigManager() { };

public:
    int LoadServerBusConfig();

    const ServerBusConfig* GetServerBusConfig(uint64_t ullClientID, uint64_t ullServerID);

private:

    //内网服务器总共需要支持的通信Bus数量
    int m_iTotalBusNumber;

    //内网服务器支持的通信Bus的配置
    ServerBusConfig m_astServerBusConfig[MAX_SERVER_BUS_NUMBER];
};

#endif
