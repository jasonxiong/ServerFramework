#ifndef __FREQ_CTRL_HPP
#define __FREQ_CTRL_HPP

#include "TimeUtility.hpp"
#include "LogAdapter.hpp"

// 操作频率控制器, 用于限制某些操作的平均频率和峰值频率. 应用场景如DB访问等.

namespace ServerLib
{
template <int TYPE_ID>
class CFreqencyCtroller
{
public:

    // 设置控制参数
    // 平均频率: iMeanTimes / iMeanPeroid  (次/秒)
    // 峰值频率: iPeakFreq	(次/秒)
    void SetParameters(time_t iMeanPeroid, int iMeanTimes, int iPeakFreq);

    // 检查是否可以执行iTimes次操作
    // 返回值:
    //		  true: 允许执行，并累计iTimes次操作
    //		  false: 不允许执行
    bool CheckFrequency(int iTimes);

private:
    CFreqencyCtroller<TYPE_ID>() {};
public:
    static CFreqencyCtroller<TYPE_ID>* Instance();

private:
    static CFreqencyCtroller<TYPE_ID>* m_pInstance;

    // 控制参数
    time_t m_iMeanPeroidCfg;
    int m_iMeanTimesCfg;
    int m_iPeakFreqCfg;

    // 运行参数 - 平均频率
    time_t m_iMeanLastTime;
    int m_iMeanAccTimes;

    // 运行参数 - 峰值频率
    time_t m_iPeakLastTime;
    int m_iPeakAccTimes;
};

template <int TYPE_ID>
CFreqencyCtroller<TYPE_ID>* CFreqencyCtroller<TYPE_ID>::m_pInstance = NULL;

template <int TYPE_ID>
CFreqencyCtroller<TYPE_ID>* CFreqencyCtroller<TYPE_ID>::Instance()
{
    if (!m_pInstance)
    {
        m_pInstance = new CFreqencyCtroller();
    }

    return m_pInstance;
}

template <int TYPE_ID>
void CFreqencyCtroller<TYPE_ID>::SetParameters(time_t iMeanPeroid, int iMeanTimes, int iPeakFreq)
{
    m_iMeanPeroidCfg = iMeanPeroid;
    m_iMeanTimesCfg = iMeanTimes;
    m_iPeakFreqCfg = iPeakFreq;

    int iMeanFreqCfg = m_iMeanTimesCfg / m_iMeanPeroidCfg;
    if (m_iPeakFreqCfg > iMeanFreqCfg)
    {
        m_iPeakFreqCfg = iMeanFreqCfg;
    }

    m_iMeanLastTime = time(NULL);
    m_iMeanAccTimes = 0;

    m_iPeakLastTime = time(NULL);
    m_iPeakAccTimes = 0;
}

template <int TYPE_ID>
bool CFreqencyCtroller<TYPE_ID>::CheckFrequency(int iTimes)
{
    if (iTimes <= 0)
    {
        return false;
    }

    time_t iNowTime = CTimeUtility::m_uiTimeTick;

    // 检查平均频率
    time_t iMeanSlapTime = iNowTime - m_iMeanLastTime;
    if (iMeanSlapTime >= m_iMeanPeroidCfg)
    {
        m_iMeanLastTime = iNowTime;
        m_iMeanAccTimes = 0;
    }

    int iMeanAccTimes = m_iMeanAccTimes + iTimes;
    if (iMeanAccTimes > m_iMeanTimesCfg)
    {
        TRACESVR("MeanFreq Check Failed: Peroid = %ld, Times = %d, Last = %ld, Now = %ld, AccTimes = %d\n",
                 m_iMeanPeroidCfg, m_iMeanTimesCfg, m_iMeanLastTime, iNowTime, iMeanAccTimes);
        return false;
    }

    // 检查峰值频率
    time_t iPeakSlapTime = iNowTime - m_iPeakLastTime;
    if (iPeakSlapTime >= 1)
    {
        m_iPeakLastTime = iNowTime;
        m_iPeakAccTimes = 0;
    }

    int iPeakAccTimes = m_iPeakAccTimes + iTimes;
    if (iPeakAccTimes > m_iPeakFreqCfg)
    {
        TRACESVR("PeakFreq Check Failed: Freq = %d, Last = %ld, Now = %ld, PeakTimes = %d\n",
                 m_iPeakFreqCfg, m_iPeakLastTime, iNowTime, iPeakAccTimes);
        return false;
    }

    // 更新频率值
    m_iMeanAccTimes = iMeanAccTimes;
    m_iPeakAccTimes = iPeakAccTimes;

    return true;
}
}

#endif

