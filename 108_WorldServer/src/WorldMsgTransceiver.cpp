
#include "Int64Utility.hpp"
#include "GameProtocol.hpp"
#include "ModuleHelper.hpp"
#include "LogAdapter.hpp"

#include "WorldMsgTransceiver.hpp"

using namespace ServerLib;

int CWorldMsgTransceiver::Initialize(bool bResumeMode)
{
    int iWorldID = CModuleHelper::GetWorldID();

    //初始化World的ZMQ通信通道
    uint64_t ullWorldServerID = GetServerBusID(iWorldID, GAME_SERVER_WORLD);

    //初始化Account到World的ZMQ通信通道
    uint64_t ullAccountServerID = GetServerBusID(iWorldID, GAME_SERVER_ACCOUNT);
    const char* pszAccount2WorldAddr = GetZmqBusAddress(ullAccountServerID, ullWorldServerID);
    if(!pszAccount2WorldAddr)
    {
        TRACESVR("Fail to get zmq bus addr, client id %"PRIu64", server id %"PRIu64"\n", ullAccountServerID, ullWorldServerID);
        return -1;
    }

    int iRet = m_oAccount2WorldSvr.ZmqInit(pszAccount2WorldAddr, EN_ZMQ_SOCKET_PAIR, EN_ZMQ_PROC_TCP, EN_ZMQ_SERVICE_SERVER);
    if(iRet)
    {
        TRACESVR("Fail to init zmq bus from account to world, ret 0x%0x\n", iRet);
        return iRet;
    }

    //初始化World到NameDB的ZMQ通信通道
    uint64_t ullNameDBServerID = GetServerBusID(iWorldID, GAME_SERVER_NAMEDB);
    const char* pszWorld2NameDBAddr = GetZmqBusAddress(ullWorldServerID, ullNameDBServerID);
    if(!pszWorld2NameDBAddr)
    {
        TRACESVR("Fail to get zmq bus addr, client id %"PRIu64", server id %"PRIu64"\n", ullWorldServerID, ullNameDBServerID);
        return -1;
    }

    iRet = m_oWorld2NameDBClient.ZmqInit(pszWorld2NameDBAddr, EN_ZMQ_SOCKET_PAIR, EN_ZMQ_PROC_TCP, EN_ZMQ_SERVICE_CLIENT);
    if(iRet)
    {
        TRACESVR("Fail to init zmq bus from world to NameDB, ret 0x%0x\n", iRet);
        return iRet;
    }

    //初始化Zone到World的ZMQ通信通道
    m_iZone2WorldBusNum = 0;
    for(int i=1; i<MAX_ZONE_PER_WORLD; ++i)
    {
        uint64_t ullZoneServerID = GetServerBusID(iWorldID, GAME_SERVER_ZONE, 1, i);
        const char* pszZone2WorldAddr = GetZmqBusAddress(ullZoneServerID, ullWorldServerID);
        if(!pszZone2WorldAddr)
        {
            TRACESVR("Fail to get zmq bus addr, client id %"PRIu64", server id %"PRIu64", zone %d\n", ullZoneServerID, ullWorldServerID, i);
            continue;
        }

        //是一个有效配置的ZMQ BUS
        ++m_iZone2WorldBusNum;

        int iRet = m_szZone2WorldSvrs[i].ZmqInit(pszZone2WorldAddr, EN_ZMQ_SOCKET_PAIR, EN_ZMQ_PROC_TCP, EN_ZMQ_SERVICE_SERVER);
        if(iRet)
        {
            TRACESVR("Fail to init zmq bus from zone to world, ret 0x%0x\n", iRet);
            return iRet;
        }
    }

    //初始化World到RoleDB的ZMQ通信通道
    uint64_t ullRoleDBServerID = GetServerBusID(iWorldID, GAME_SERVER_ROLEDB);
    const char* pszWorld2RoleDBAddr = GetZmqBusAddress(ullWorldServerID, ullRoleDBServerID);
    if(!pszWorld2RoleDBAddr)
    {
        TRACESVR("Fail to get zqm bus addr, client id %"PRIu64", server id %"PRIu64"\n", ullWorldServerID, ullRoleDBServerID);
        return -2;
    }

    iRet = m_oWorld2RoleDBClient.ZmqInit(pszWorld2RoleDBAddr, EN_ZMQ_SOCKET_PAIR, EN_ZMQ_PROC_TCP, EN_ZMQ_SERVICE_CLIENT);
    if(iRet)
    {
        TRACESVR("Fail to init zmq bus from world to roledb, ret 0x%0x\n", iRet);
        return iRet;
    }

    //去掉Cluster
    /*
    //初始化World到Cluster的ZMQ通信通道
    uint64_t ullClusterServerID = GetServerBusID(0, GAME_SERVER_CLUSTER);
    const char* pszWorld2ClusterAddr = GetZmqBusAddress(ullWorldServerID, ullClusterServerID);
    if(!pszWorld2ClusterAddr)
    {
        TRACESVR("Fail to get zmq bus addr, client id %"PRIu64", server id %"PRIu64"\n", ullWorldServerID, ullClusterServerID);
        return -3;
    }

    iRet = m_oWorld2ClusterClient.ZmqInit(pszWorld2ClusterAddr, EN_ZMQ_SOCKET_PAIR, EN_ZMQ_PROC_TCP, EN_ZMQ_SERVICE_CLIENT);
    if(iRet)
    {
        TRACESVR("Fail to init zmq bus from world to cluster, ret 0x%0x\n", iRet);
        return iRet;
    }
    */

    return 0;
}

ZmqBus* CWorldMsgTransceiver::GetZmqBus(EGameServerID enMsgPeer, int iInstance)
{
    switch (enMsgPeer)
    {
    case GAME_SERVER_ZONE:
        {
            return &m_szZone2WorldSvrs[iInstance];
        }

    case GAME_SERVER_ROLEDB:
        {
            return &m_oWorld2RoleDBClient;
        }

    case GAME_SERVER_ACCOUNT:
        {
            return &m_oAccount2WorldSvr;
        }

        /*
    case GAME_SERVER_CLUSTER:
        {
            return &m_oWorld2ClusterClient;
        }
        */

    case GAME_SERVER_NAMEDB:
        {
            return &m_oWorld2NameDBClient;
        }

    default:
        {
            break;
        }
    }

    return NULL;
}

// 发送和接收消息
int CWorldMsgTransceiver::SendOneMsg(const char* pszMsg, int iMsgLength, EGameServerID enMsgPeer, int iInstance)
{
    ZmqBus* pMsgBus = GetZmqBus(enMsgPeer, iInstance);
    if(!pMsgBus)
    {
        return -1;
    }

    int iRet = pMsgBus->ZmqSend(pszMsg, iMsgLength, 0);

    return iRet;
}

int CWorldMsgTransceiver::RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, EGameServerID& enMsgPeer, int& iInstance)
{
    int iRet = 0;
    ZmqBus* pMsgBus = NULL;

    if(enMsgPeer == GAME_SERVER_UNDEFINE)
    {
        //todo jasonxiong 后续确认后根据实际需要连接的对端做出修改
        static EGameServerID szGameServerType[GAME_SERVER_MAX] = {GAME_SERVER_ACCOUNT, GAME_SERVER_ZONE, GAME_SERVER_CLUSTER, GAME_SERVER_ROLEDB, GAME_SERVER_NAMEDB}; 
        static int iRoundIndex = 0;
        iRoundIndex = (iRoundIndex+1) % GAME_SERVER_MAX;

        switch (szGameServerType[iRoundIndex])
        {
        case GAME_SERVER_ACCOUNT:
            {
                //从Account Server收包
                enMsgPeer = GAME_SERVER_ACCOUNT;
                iInstance = 1;
            }
            break;

        case GAME_SERVER_ZONE:
            {
                //从Zone Server收包
                enMsgPeer = GAME_SERVER_ZONE;
                
                static int iZoneIDIndex = 0;
                if(m_iZone2WorldBusNum != 0)
                {
                    iZoneIDIndex = (iZoneIDIndex+1) % (m_iZone2WorldBusNum+1);
                    iZoneIDIndex = (iZoneIDIndex==0) ? 1 : iZoneIDIndex;
                }
                else
                {
                    iZoneIDIndex = 1;
                }

                iInstance = iZoneIDIndex;
            }
            break;

        case GAME_SERVER_ROLEDB:
            {
                //从RoleDB收包
                enMsgPeer = GAME_SERVER_ROLEDB;
                iInstance = 1;
            }
            break;

            /*
        case GAME_SERVER_CLUSTER:
            {
                //从Cluster Server收包
                enMsgPeer = GAME_SERVER_CLUSTER;
                iInstance = 1;
            }
            break;
            */

        case GAME_SERVER_NAMEDB:
            {
                //从NameDB Server收包
                enMsgPeer = GAME_SERVER_NAMEDB;
                iInstance = 1;
            }
            break;

        default:
            {
                break;
            }
        }
    }

    pMsgBus = GetZmqBus(enMsgPeer, iInstance);
    if(!pMsgBus)
    {
        return -1;
    }

    iRet = pMsgBus->ZmqRecv(pszMsg, iMaxOutMsgLen, riMsgLength, 0);

    return iRet;
}
