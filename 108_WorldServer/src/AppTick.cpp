#include "AppTick.hpp"
#include "LogAdapter.hpp"
#include "TimeUtility.hpp"
#include "ObjStatistic.hpp"
#include "ModuleHelper.hpp"

CAppTick::CAppTick()
{
}

int CAppTick::Initialize(bool bResumeMode)
{
    m_tvLastBaseTick.RefreshTime();

    m_stZoneTick.Initialize(bResumeMode);
    m_stOnlineStatTick.Initialize();

    m_iLastMsgStatTick = time(NULL);

    return 0;
}

//static const char *aaanme[] = {"CGuildApplyRequest", "CRoleGuildMapping", "CGuildObj"};

//const int aaanme_num = sizeof(aaanme)/sizeof(aaanme[0]);
int CAppTick::CountObjStat()
{
    //int i = 0;
    //ObjectStatisticSingleton::Instance()->SetObjectStatName(aaanme, aaanme_num);

    //todo jaosnxiong 暂时不需要考虑公会的实现
    /*
    ObjectStatisticSingleton::Instance()->AddObjectStat(i++, WorldTypeK64<CGuildApplyRequest>::GetUsedObjNumber());
    ObjectStatisticSingleton::Instance()->AddObjectStat(i++, WorldTypeK64<CRoleGuildMapping>::GetUsedObjNumber());
    ObjectStatisticSingleton::Instance()->AddObjectStat(i++, WorldTypeK32<CGuildObj>::GetUsedObjNumber());
    */

    return 0;
}

int CAppTick::OnTick()
{
    //每个tick都更新时间，给tick使用
    CTimeUtility::m_uiTimeTick = time(NULL);
    CTimeUtility::m_stTimeValueTick.RefreshTime();

    CTimeValue stNow = CTimeUtility::m_stTimeValueTick;

    CTimeValue tvSlape = stNow - m_tvLastBaseTick;
    int iMilliSeconds = tvSlape.GetTimeValue().tv_sec * 1000 + tvSlape.GetTimeValue().tv_usec / 1000;

    if (iMilliSeconds >= TICK_TIMEVAL_BASE)
    {
        //TRACESVR_EX(LOG_LEVEL_TICK, "Base tick: %d million seconds!\n", iMilliSeconds);

        m_stUnitTick.OnTick();

        // 暂时禁掉踢线功能, 太危险
        //在ontick里废除， 这里需要统计
        m_stZoneTick.OnTick();

        //上报world状态到cluster
        m_stOnlineStatTick.OnTick();

        m_tvLastBaseTick.RefreshTime();
    }

    int iSlapTime =  CTimeUtility::m_uiTimeTick - m_iLastMsgStatTick;
    if (iSlapTime >= STATISTIC_TIME)
    {
        CountObjStat();
        ObjectStatisticSingleton::Instance()->Print();
        ObjectStatisticSingleton::Instance()->Reset();

        m_iLastMsgStatTick = CTimeUtility::m_uiTimeTick;
    }

    return 0;
}
