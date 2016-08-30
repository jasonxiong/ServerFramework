#include <time.h>

#include "GameProtocol.hpp"
#include "WorldBillLog.hpp"
#include "ConfigHelper.hpp"
#include "StringUtility.hpp"
#include "WorldRoleStatus.hpp"
#include "TimeValue.hpp"
#include "ModuleHelper.hpp"

const int CWorldBillLog::MAX_BILL_EVENT_PER_SECOND = 65535;
int CWorldBillLog::m_iLastEventTime;
unsigned short CWorldBillLog::m_usEventSequence;

// 本地Bill日志
CBillLogAdapter CWorldBillLog::m_stBillLog;

///////////////////////////////////////////////////////////////////////////////////////////////////
// 日志接口
void CWorldBillLog::TraceAddMoney(const unsigned uin, int iLoginType, int64_t iAmount)
{
    /*
    <!-- 晶钻兑换日志 -->
    <struct name="AddMoney"           id="LOG_ADDMONEY"       desc="晶钻兑换日志" version="1" >
    <entry name="iEventId"          type="biguint"          desc="事件ID, 用于标识同类消息的唯一性" />
    <entry name="iSrcUin"           type="uint"             desc="充值的QQ号" />
    <entry name="iDstUin"           type="uint"             desc="被重置的QQ号" />
    <entry name="dtEventTime"       type="datetime"         desc="事件时间" />
    <entry name="iClientIp"         type="uint"             desc="购买者IP" />
    <entry name="iSource"           type="int"              desc="充值渠道" />
    <entry name="iAmount"           type="bigint"           desc="金额(晶钻)" />
    <entry name="iRemark"           type="biguint"          desc="流水" />
    <entry name="iDesc"             type="string "          size="64"   desc="备注信息"/>
    </struct>
    */

    //todo jasonxiong 所有的记账系统后续需要重新开发
    /*
    TEventId stEventId;
    GenerateLogHead(LOG_ADDMONEY, stEventId, 0);

    ADDMONEY& rstAddMoney = m_stLogPkg.detail.addMoney;
    rstAddMoney.iEventId = *(tdr_ulonglong*)&stEventId;
    rstAddMoney.dtEventTime = TdrDateTime(time(NULL));
    rstAddMoney.iSrcUin = rstRoleID.m_uiUin;
    rstAddMoney.iDstUin = rstRoleID.m_uiUin;
    rstAddMoney.iClientIp = 0;
    rstAddMoney.iAmount = iAmount;
	rstAddMoney.iLoginType = iLoginType;
    WriteBillLog();
    */
    return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int CWorldBillLog::Initialize()
{
    //todo jasonxiong 后面重新开发记账系统
    /*
    // 初始化TDR句柄
    char szProtocolPath[256];

    GetCommonResourcePath(szProtocolPath, sizeof(szProtocolPath), KYLINPROTOCOL_FILE);

    ///根据xml文件创建元数据库
    int iRet = tdr_load_metalib(&m_pstWorldLib, szProtocolPath);
    if (TDR_ERR_IS_ERROR(iRet))
    {
        printf("tdr_load_metalib failed by file %s, for %s\n", szProtocolPath, tdr_error_string(iRet));
        ASSERT_AND_LOG_RTN_INT(0);
    }
    m_pstMsgMeta = tdr_get_meta_by_name(m_pstWorldLib, "LogPkg");
    if (TDR_ERR_IS_ERROR(iRet))
    {
        printf("tdr_get_meta_by_name GameProtocolMsg, for %s\n", tdr_error_string(iRet));
        ASSERT_AND_LOG_RTN_INT(0);
    }

    // 初始化TLOG设施
    m_pstTLogCtx = NULL;
    m_pstTLogCategory = NULL;

    m_pstTLogCtx = tlog_init_from_file(TLOG_CONFIG_FILE);
    if (m_pstTLogCtx)
    {
        m_pstTLogCategory = tlog_get_category(m_pstTLogCtx, "StarStory");
    }

    m_usEventSequence = 0;
    m_iLastEventTime = 0;
    */
    return 0;
}

void CWorldBillLog::GenerateLogHead(int iMsgID, TEventId& rstEventId, const unsigned char ucZoneID)
{
    //todo jasonxiong 暂时先注释掉，后面重新开发记账系统
    /*
    memset(&m_stLogPkg, 0, sizeof(m_stLogPkg));
    m_stLogPkg.cmd = iMsgID;

    if (rstEventId.m_iEventTime == 0)
    {
        // 初始化事件流水号

        CTimeValue tvNow;
        tvNow.RefreshTime();
        int iEventTime = tvNow.GetTimeValue().tv_sec;
        m_usEventSequence++;

        if (iEventTime == m_iLastEventTime)
        {
            if (m_usEventSequence >= MAX_BILL_EVENT_PER_SECOND)
            {
                // 事件创建太频繁!
                TRACESVR("Alert: Event Too Frequently!\n");
                m_usEventSequence = 0;
            }
        }
        else
        {
            m_iLastEventTime = iEventTime;
			m_usEventSequence = 0;
        }

        rstEventId.m_iEventTime = iEventTime;
        rstEventId.m_ucWorldID = CModuleHelper::GetWorldID();
        rstEventId.m_ucZoneID = ucZoneID;
        rstEventId.m_usEventSeq = m_usEventSequence;
    }
    */

    return;
}

int CWorldBillLog::WriteBillLog()
{
    //todo jasonxiong 暂时先注释掉，后面重新开发记账系统

    /*
    TDRDATA stHost;
    TDRDATA stNet;
    TDRPRINTFORMAT stFormat;

    stFormat.chIndentChar = 0;
    stFormat.iNoVarName = 1;
    stFormat.pszSepStr = "|";

    stHost.pszBuff = (char *)&m_stLogPkg;
    stHost.iBuff = sizeof(LOGPKG);

    static char m_szSendBuff[1024*5];
    stNet.pszBuff = m_szSendBuff;
    stNet.iBuff = sizeof(m_szSendBuff);

    int iRet = tdr_sprintf_ex(m_pstMsgMeta, &stNet, &stHost,0, &stFormat);
    if(iRet != 0)
    {
        return iRet;
    }

    // 写入远TLog日志
    if (m_pstTLogCategory)
    {
        tlog_info(m_pstTLogCategory, 0, 0, m_szSendBuff);
    }

    // 写入本地BillLog日志文件
    m_stBillLog.CheckBillPath();
    if (m_stBillLog.m_stLogFile.OpenLogFile())
    {
        TRACESVR("Cannot Open BillLog!\n");
        return -2;
    }

    FILE* pBillLogFile = m_stBillLog.m_stLogFile.GetFilePointer();
    if (!pBillLogFile)
    {
        TRACESVR("Cannot Get BillLog FILE!\n");
        return -3;
    }

    // 写入日志
    fprintf(pBillLogFile, "%s", m_szSendBuff);

    // 刷新日志
    fflush(pBillLogFile);

    // 关闭日志文件
    m_stBillLog.m_stLogFile.CloseFile();
    */
    
    return 0;
}



	

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
