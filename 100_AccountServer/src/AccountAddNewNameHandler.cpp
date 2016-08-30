#include <assert.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "FixedHashCache.hpp"
#include "LRUHashCache.hpp"
#include "SessionObj.hpp"
#include "AccountSingleton.hpp"
#include "CreateRoleRequestObj.hpp"

#include "AccountAddNewNameHandler.hpp"

CAccountAddNewNameHandler::CAccountAddNewNameHandler()
{
    m_pRequestMsg = NULL;
}

void CAccountAddNewNameHandler::OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg, SHandleResult* pstResult)
{
    // 不使用Result
    ASSERT_AND_LOG_RTN_VOID(pstNetHead);
    ASSERT_AND_LOG_RTN_VOID(pstMsg);

    m_pRequestMsg = pstMsg;

    //获取返回值
    const AddNewName_Response& rstResp = pstMsg->m_stmsgbody().m_staddnewname_response();
    if(rstResp.iresult() != 0)
    {
        //添加失败，返回创建帐号失败给客户端
        SendCreateRoleFailedToLotus(rstResp.name_id(), rstResp.iresult());

        //清除cache中缓存的节点
        CFixedHashCache<CCreateRoleRequestObj>::DeleteByUin(rstResp.name_id());

        return;
    }

    //添加角色名字成功，发送创建角色的请求
    int iRet = SendCreateRoleRequestToWorld(rstResp.strname(), rstResp.name_id());
    if(iRet)
    {
        LOGERROR("Failed to send create role request to world server, uin %u, name %s\n", rstResp.name_id(), rstResp.strname().c_str());

        SendCreateRoleFailedToLotus(rstResp.name_id(), iRet);

        //清除cache中缓存的节点
        CFixedHashCache<CCreateRoleRequestObj>::DeleteByUin(rstResp.name_id());

        //创建角色失败，删除NameDB中的名字
        SendDeleteNameRequestToWorld(rstResp.strname(), rstResp.itype(), rstResp.name_id());

        return;
    }

    return;
}

//发送创建角色的请求
int CAccountAddNewNameHandler::SendCreateRoleRequestToWorld(const std::string& strName, unsigned int uin)
{
    CCreateRoleRequestObj* pstRequestObj = CFixedHashCache<CCreateRoleRequestObj>::GetByUin(uin);
    if(!pstRequestObj)
    {
        LOGERROR("Failed to send create role request to world svr, uin %u\n", uin);
        return -1;
    }

    static GameProtocolMsg stCreateRoleMsg;
    GenerateMsgHead(&stCreateRoleMsg, m_pRequestMsg->m_stmsghead().m_uisessionfd(), MSGID_ACCOUNT_CREATEROLE_REQUEST, uin);

    CreateRole_Account_Request* pstReq = stCreateRoleMsg.mutable_m_stmsgbody()->mutable_m_staccountcreaterolerequest();
    pstReq->set_worldid(pstRequestObj->GetWorldID());
    pstReq->set_uin(pstRequestObj->GetUin());
    pstReq->set_sznickname(pstRequestObj->GetNickName());

    if(EncodeAndSendCode(SSProtocolEngine, NULL, &stCreateRoleMsg, GAME_SERVER_WORLD) != 0)
    {
        LOGERROR("Failed to send create role request to world!\n");
        return -2;
    }

    return 0;
}

//发送创建角色失败的请求到Lotus
void CAccountAddNewNameHandler::SendCreateRoleFailedToLotus(unsigned int uin, int iResult)
{
    static GameProtocolMsg stResponseMsg;

    unsigned int uiSessionFd = m_pRequestMsg->m_stmsghead().m_uisessionfd();

    GenerateMsgHead(&stResponseMsg, uiSessionFd, MSGID_ACCOUNT_CREATEROLE_RESPONSE, uin);

    CreateRole_Account_Response* pstResp = stResponseMsg.mutable_m_stmsgbody()->mutable_m_staccountcreateroleresponse();
    pstResp->set_iresult(iResult);

    CSessionObj* pstSessionObj = SessionManager->GetSession(uiSessionFd);
    if(!pstSessionObj)
    {
        LOGERROR("Failed to get session obj, session fd %d, uin %u\n", uiSessionFd, uin);
        return;
    }

    if(EncodeAndSendCode(CSProtocolEngine, pstSessionObj->GetNetHead(), &stResponseMsg, GAME_SERVER_LOTUSACCOUNT) != 0)
    {
        LOGERROR("Failed to send create role failed response to lotus!\n");
        return;
    }

    return;
}

//删除NameDB中的名字
int CAccountAddNewNameHandler::SendDeleteNameRequestToWorld(const std::string& strName, int iType, unsigned uin)
{
    static GameProtocolMsg stDeleteNameMsg;
    GenerateMsgHead(&stDeleteNameMsg, m_pRequestMsg->m_stmsghead().m_uisessionfd(), MSGID_DELETENAME_REQUEST, uin);

    DeleteName_Request* pstReq = stDeleteNameMsg.mutable_m_stmsgbody()->mutable_m_stdeletename_request();
    pstReq->set_strname(strName);
    pstReq->set_itype(EN_NAME_TYPE_ROLE);

    if(EncodeAndSendCode(SSProtocolEngine, NULL, &stDeleteNameMsg, GAME_SERVER_WORLD) != 0)
    {
        LOGERROR("Failed to send delete name request to world!\n");
        return -2;
    }

    return 0;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
