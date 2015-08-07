#ifndef __FETCH_FRIEND_INFO_HANDLER_HPP__
#define __FETCH_FRIEND_INFO_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "RoleDBPublic.hpp"

class CRoleDBHandlerSet;

class CFetchFriendInfoHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // 访问数据库的指针
    GameProtocolMsg* m_pstRequestMsg; // 待处理的消息

	int m_iThreadIdx;
public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // 本类的对象只能在CRoleDBHandlerSet类中创建
    friend class CRoleDBHandlerSet;

    //每个工作线程绑定一个mysql客户端连接
    CFetchFriendInfoHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:
    int QueryFriendInfo(unsigned int uin, Zone_GetFriendInfo_Response* pstResponse);
    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg);
};

#endif // __FETCH_FRIEND_INFO_HANDLER_HPP__
