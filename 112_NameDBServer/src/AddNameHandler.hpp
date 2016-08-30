#ifndef __ADD_NAME_HANDLER_HPP__
#define __ADD_NAME_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "NameDBPublic.hpp"

//增加玩家名字的请求

class CNameDBHandlerSet;

// MSGID_ADDNEWNAME_REQUEST 消息处理者
class CAddNameHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // 访问数据库的指针
    GameProtocolMsg* m_pstRequestMsg; // 待处理的消息

	//所属线程idx
	int m_iThreadIdx;

    //生成的SQL语句
    static char m_szQueryString[GameConfig::NAME_TABLE_SPLIT_FACTOR][1024];

public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // 本类的对象只能在CNameDBHandlerSet类中创建
    friend class CNameDBHandlerSet;
    CAddNameHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:

    //进行必要的参数检查
    int CheckParams();

    void OnAddNameRequest(SHandleResult* pstHandleResult);

    int CheckNameExist(const std::string& strName, int iType, bool& bIsExist);

    //插入新的记录
    int AddNewRecord(const std::string& strName, int iNameType, unsigned uNameID);

private:
    void FillFailedResponse(const unsigned int uiResultID, int iNameType, unsigned uNameID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(const std::string& strName, int iNameType, unsigned uNameID, GameProtocolMsg* pstResponseMsg);
};

#endif // __ADD_NAME_HANDLER_HPP__

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
