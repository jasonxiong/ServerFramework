
#ifndef __LIST_ZONE_HANDLER_HPP__
#define __LIST_ZONE_HANDLER_HPP__

#include "Handler.hpp"

class CAccountHandlerSet;

// 拉取本服的所有ZoneServer的详细信息
class CListZoneHandler : public IHandler
{
private:
    TNetHead_V2* m_pstNetHead;
    GameProtocolMsg* m_pstRequestMsg; // 待处理的消息

private:
    // 本类的对象只能在CAccountHandlerSet类中创建
    friend class CAccountHandlerSet;
    CListZoneHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult);
private:

    //将ListZone的请求转发给WorldServer
    void SendListZoneRequestToWorld();

    //将ListZone的返回转发给Client
    void SendListZoneResponseToLotus();
};

#endif


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
