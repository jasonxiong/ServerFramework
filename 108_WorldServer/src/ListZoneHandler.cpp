
#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "LogAdapter.hpp"

#include "ListZoneHandler.hpp"

CListZoneHandler::CListZoneHandler(void)
{
}

CListZoneHandler::~CListZoneHandler(void)
{
}

GameProtocolMsg CListZoneHandler::m_stWorldMsg;

int CListZoneHandler::OnClientMsg(const void* pMsg)
{
    ASSERT_AND_LOG_RTN_INT(pMsg);

    m_pRequestMsg = (GameProtocolMsg*)pMsg;
    if(m_pRequestMsg->m_stmsghead().m_uimsgid() != MSGID_ACCOUNT_LISTZONE_REQUEST)
    {
        return -1;
    }

    //初始化返回消息头
    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg,
                                     m_pRequestMsg->m_stmsghead().m_uisessionfd(),
                                     MSGID_ACCOUNT_LISTZONE_RESPONSE,
                                     m_pRequestMsg->m_stmsghead().m_uin());

    Account_ListZone_Response* pstResp = m_stWorldMsg.mutable_m_stmsgbody()->mutable_m_staccount_listzone_response();
    pstResp->set_iresult(T_SERVER_SUCESS);
    pstResp->set_uworldid(CModuleHelper::GetWorldID());

    //封装所有活跃的Zone分区
    const TZoneConfList& stZoneList = CModuleHelper::GetConfigManager()->GetZoneConfList();
    for(int i=0; i<stZoneList.m_ushLen; ++i)
    {
        if(!CModuleHelper::GetZoneTick()->IsZoneActive(stZoneList.m_astZoneConf[i].m_ushZoneID))
        {
            continue;
        }

        ZoneInfo* pstZoneInfo = pstResp->mutable_stzonelist()->add_stzoneinfos();
        pstZoneInfo->set_uzoneid(stZoneList.m_astZoneConf[i].m_ushZoneID);
        pstZoneInfo->set_strzonename(stZoneList.m_astZoneConf[i].m_szZoneName);
        pstZoneInfo->set_uzonestate(CModuleHelper::GetZoneTick()->GetZoneStatus(stZoneList.m_astZoneConf[i].m_ushZoneID).m_ucState);
        pstZoneInfo->set_strzoneip(stZoneList.m_astZoneConf[i].m_szHostIP);
        pstZoneInfo->set_uzoneport(stZoneList.m_astZoneConf[i].m_ushPort);
    }

    CWorldMsgHelper::SendWorldMsgToAccount(m_stWorldMsg);

    return 0;
}





