#ifndef __DELETE_NAME_HANDLER_HPP__
#define __DELETE_NAME_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "NameDBPublic.hpp"

class CNameDBHandlerSet;

// MSGID_DELETENAME_REQUEST 消息处理者
class CDeleteNameHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // 访问数据库的指针
    GameProtocolMsg* m_pstRequestMsg; // 待处理的消息

	int m_iThreadIdx;

    //生成的SQL语句
    static char m_szQueryString[GameConfig::NAME_TABLE_SPLIT_FACTOR][256];

public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // 本类的对象只能在CNameDBHandlerSet类中创建
    friend class CNameDBHandlerSet;
    CDeleteNameHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:

    //删除Name信息
    int DeleteName(const std::string& strName, int iType);

    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg);
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
