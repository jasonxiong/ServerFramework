#ifndef __WORLD_MSG_HELPER_HPP__
#define __WORLD_MSG_HELPER_HPP__

#include "WorldGlobalDefine.hpp"

class CWorldMsgHelper
{
public:
    static int GenerateMsgHead(GameProtocolMsg& rstWorldMsg,
                               unsigned int uiSessionFD, unsigned int msgID, unsigned int uiUin);
    static int SendWorldMsgToRoleDB(const GameProtocolMsg& rstWorldMsg);
    static int SendWorldMsgToWGS(const GameProtocolMsg& rstWorldMsg, int iZoneID, bool bHomeZone = false); // bHomeZone是否发家园线
    static int SendWorldMsgToAccount(const GameProtocolMsg& rstWorldMsg);
    static int SendWorldMsgToMailDB(const GameProtocolMsg& rstWorldMs);
    static int SendWorldMsgToGuildDB(const GameProtocolMsg& rstWorldMsg);
    static int SendWorldMsgToNameDB(const GameProtocolMsg& rstWorldMsg);
    static int SendWorldMsgToCluster(const GameProtocolMsg& rstWorldMsg);
    static int BroadcastNewZoneConf();

private:
    static GameProtocolMsg m_stWorldMsg;
};


#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
