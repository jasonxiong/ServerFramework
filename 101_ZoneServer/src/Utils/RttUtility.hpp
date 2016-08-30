
#ifndef __RTT_CALCULATOR_HPP__
#define __RTT_CALCULATOR_HPP__

// 使用[Jacobson 1988]算法统计网络RTT

#include "GameConfigDefine.hpp"
#include "GameProtocol.hpp"
#include "TimeValue.hpp"

using namespace ServerLib;

typedef struct tagRoundTripTime
{
    double m_srtt;   // 平均RTT
    double m_drtt;   // 方差RTT
    double m_rtt;    // 计算后的RTT

    int Initialize()
    {
        m_srtt = 0.0;
        m_drtt = 0.75;
        m_rtt = 0.0;

        return 0;
    }
}TRoundTripTime;

class CRttUtility
{
public:
    // 将32位数转换为64位数
    static uint64_t Convert32to64(unsigned int high, unsigned int low);

    // 获取当前的毫秒时间
    static uint64_t GetNowMilliSeconds();

    // 获取时间的毫秒值
    static uint64_t GetMilliSeconds(CTimeValue& rstTimeValue);

    // 更新客户端Rtt
    static int UpdateRTT(const GameCSMsgHead& rstMsgHead);
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
