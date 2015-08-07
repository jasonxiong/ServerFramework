
#include "GameProtocol.hpp"
#include "GameRole.hpp"
#include "ZoneObjectHelper.hpp"
#include "UnitUtility.hpp"
#include "GMCommandManager.hpp"
#include "ZoneMsgHelper.hpp"
#include "ModuleHelper.hpp"
#include "ConfigManager.hpp"

#include "GMCommandHandler.hpp"

GameProtocolMsg CGMCommandHandler::m_stGameMsg;

CGMCommandHandler::~CGMCommandHandler(void)
{

}

CGMCommandHandler::CGMCommandHandler()
{
    m_stGameMsg.Clear();

    m_vParams.clear();
}

int CGMCommandHandler::OnClientMsg()
{
    switch (m_pRequestMsg->m_stmsghead().m_uimsgid())
    {
        case MSGID_ZONE_GAMEMASTER_REQUEST:
            {
                OnRequestGM();
            }
            break;

        default:
            {
                LOGERROR("Failed to handler request msg, invalid msgid: %u\n", m_pRequestMsg->m_stmsghead().m_uimsgid());
                return -1;
            }
            break;
    }

    return 0;
}

int CGMCommandHandler::ParseParameter()
{
    //说明： 输入的GM命令的参数格式要求为:
    //      1.GM命令以字符 "//" 作为开始,与实际GM命令之间使用空格分隔;
    //      2.后面跟随的为用户实际输入的GM命令，以空格为分隔符;
    const char* pszParam = m_pRequestMsg->m_stmsgbody().m_stzone_gamemaster_request().strcommand().c_str();
    if(strlen(pszParam) < 3)
    {
        LOGERROR("Failed to parse GM command param %s, uin %u\n", pszParam, m_pRoleObj->GetUin());
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //首先检查起始字符,是否为 "//"
    if(pszParam[0]!='/' || pszParam[1]!='/')
    {
        LOGERROR("Failed to parse GM command param %s, uin %u\n", pszParam, m_pRoleObj->GetUin());
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //再跳过一个空格，开始解析后面的命令参数
    int iBegin = 3;
    for(int i=3; i<(int)strlen(pszParam)+1; ++i)
    {
        if(pszParam[i]!=' ' && pszParam[i]!='\0')
        {
            continue;
        }

        //跳过空格的字符串
        if(i-iBegin != 0)
        {
            //一个参数结束
            m_vParams.push_back(std::string(pszParam+iBegin, i-iBegin));
        }

        //开始取下一个有效参数
        iBegin = i+1;
    }

    return T_SERVER_SUCESS;
}

int CGMCommandHandler::OnRequestGM()
{
    unsigned uin = m_pRequestMsg->m_stmsghead().m_uin();

    //清空参数
    m_vParams.clear();
    m_stGameMsg.Clear();

    //校验参数的有效性
    int iRet = SecurityCheck();
    if(iRet)
    {
        LOGERROR("Failed to check request param, uin %u, ret %d\n", uin, iRet);
        SendFailedResponse(T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);
        return iRet;
    }

    //解析命令行参数
    iRet = ParseParameter();
    if(iRet)
    {
        LOGERROR("Failed to parse GM param, uin %u, ret %d\n", uin, iRet);
        SendFailedResponse(T_ZONE_SYSTEM_PARA_ERR, *m_pNetHead);
        return iRet;
    }

    //判断是否具有GM权限
    if(CheckIsGMUser())
    {
        //是有效的GM权限的用户，设置GM相关权限
        CUnitUtility::SetUnitStatus(&(m_pRoleObj->GetUnitInfo()), EGUS_ISGM);
    }

    if(!CUnitUtility::IsUnitStatusSet(&(m_pRoleObj->GetUnitInfo()), EGUS_ISGM))
    {
        LOGERROR("Failed to do GM request, uin: %u is no GM!\n", uin);
        SendFailedResponse(T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);
        return -3;
    }

    //执行GM命令
    iRet = RunCommand();
    if(iRet)
    {
        LOGERROR("Failed to run GM command, uin %u, ret %d\n", uin, iRet);
        SendFailedResponse(T_ZONE_GM_COMMAND_FAILED, *m_pNetHead);
        return iRet;
    }

    //返回给客户端执行成功
    SendSuccessfulResponse();

    return T_SERVER_SUCESS;
}

int CGMCommandHandler::RunCommand()
{
    //查找处理的Handler
    IGMBaseCommand* pCommandHandler = CGMCommandManager::Instance()->GetCommandHandler(m_vParams[0]);
    if(!pCommandHandler)
    {
        LOGERROR("Failed to get GM command handler, uin %u, command name %s", m_pRoleObj->GetUin(), m_vParams[0].c_str());
        return -3;
    }

    int iCommandType = CGMCommandManager::Instance()->GetHandlerType(m_vParams[0]);

    //执行GM命令
    int iRet = pCommandHandler->Run(m_pRoleObj, iCommandType, m_vParams);
    if(iRet)
    {
        LOGERROR("Failed to run GM command, uin %u, ret %d\n", m_pRoleObj->GetUin(), iRet);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

//检查是否是GM用户
int CGMCommandHandler::CheckIsGMUser()
{
    //首先检查是否GM玩家的uin
    CQMGMPrivConfigManager& rstGMPrivConfigMgr = CModuleHelper::GetConfigManager()->GetGMPrivConfigManager();
    if(rstGMPrivConfigMgr.CheckIsGMUin(m_pRoleObj->GetUin()))
    {
        return true;
    }

    unsigned int uClientIP = inet_network(m_pSession->GetClientIP());
    if(rstGMPrivConfigMgr.CheckIsGMIP(uClientIP))
    {
        return true;
    }

    return false;
}

int CGMCommandHandler::SendFailedResponse(int iResultID, const TNetHead_V2& rstNetHead)
{
    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_GAMEMASTER_RESPONSE);

    m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_gamemaster_response()->set_iresult(iResultID);

    CZoneMsgHelper::SendZoneMsgToClient(m_stGameMsg, rstNetHead);

    return 0;
}

int CGMCommandHandler::SendSuccessfulResponse()
{
    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_GAMEMASTER_RESPONSE);

    CGameRoleObj* pRoleObj = m_pSession->GetBindingRole();
    ASSERT_AND_LOG_RTN_INT(pRoleObj);

    Zone_GameMaster_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_gamemaster_response();
    pstResp->set_iresult(T_SERVER_SUCESS);

    CZoneMsgHelper::SendZoneMsgToRole(m_stGameMsg, pRoleObj);

    return 0;
}
