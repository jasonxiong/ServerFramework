#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "SessionManager.hpp"

int CSessionManager::Initialize(bool bResume)
{
    if (bResume)
    {
        return 0;
    }

    for (int i = 0; i < (int)MAX_ROLE_OBJECT_NUMBER_IN_WORLD; i++)
    {
        m_astSessionList[0][i].SetActive(false);
        m_astSessionList[1][i].SetActive(false);
    }

    m_unSeed = 0;
    m_uiActiveNumber = 0;
    return 0;
}

CSessionObj* CSessionManager::CreateSession(TNetHead_V2& stNetHead)
{
    unsigned int uiSessionFD = ntohl(stNetHead.m_uiSocketFD);
    // TRACESVR("session fd: %u\n", uiSessionFD);

    CSessionObj* pSessionObj = GetSessionByFd(uiSessionFD);
    if (pSessionObj == NULL)
    {
        return NULL;
    }

    if (pSessionObj->IsActive())
    {
        return pSessionObj;
    }

    pSessionObj->Create(stNetHead, GenerateValue());
    m_uiActiveNumber++;

    return pSessionObj;
}

int CSessionManager::DeleteSession(const unsigned int uiSessionFD)
{
    LOGDETAIL("delete session, sockfd: %u\n", uiSessionFD);

    CSessionObj* pSessionObj = GetSessionByFd(uiSessionFD);
    if (pSessionObj == NULL)
    {
        return 0;
    }

    pSessionObj->SetActive(false);
    m_uiActiveNumber--;

    return 0;
}

unsigned short CSessionManager::GenerateValue()
{
    unsigned short unValue = ++m_unSeed;
    if (0xffff == unValue) // 增长到最大值时重置为0，重新计数
    {
        m_unSeed = 0;
    }

    return unValue;
}

int CSessionManager::CheckSession(TNetHead_V2& stNetHead)
{
    //// 检查该fd的session缓存是否存在，如果存在则拒绝服务
    unsigned int uiSessionFD = ntohl(stNetHead.m_uiSocketFD);

    // 检查session缓存中的结点个数是否达到上限，达到上限则拒绝服务
    if (GetSessionByFd(uiSessionFD) == NULL)
    {
        return T_ACCOUNT_SERVER_BUSY;
    }

    return T_SERVER_SUCESS;
}

CSessionObj* CSessionManager::GetSessionByFd(unsigned int uiSessionFD)
{
    if (uiSessionFD >= (unsigned int)MAX_FD_NUMBER * 2)
    {
        return NULL;
    }

    int iCodeQueueNum = uiSessionFD / MAX_FD_NUMBER;
    int iFdIdx = uiSessionFD % MAX_ROLE_OBJECT_NUMBER_IN_WORLD;

    if (iCodeQueueNum >= MAX_CODEQUEUE_NUM)
    {
        return NULL;
    }
    if (iFdIdx >= (int)MAX_ROLE_OBJECT_NUMBER_IN_WORLD)
    {
        return NULL;
    }

    return &(m_astSessionList[iCodeQueueNum][iFdIdx]);

}

CSessionObj* CSessionManager::GetSession(const unsigned int uiSessionFD, const unsigned short unValue)
{
    CSessionObj* pSessionObj = GetSessionByFd(uiSessionFD);
    if (pSessionObj->IsActive())
    {
        return pSessionObj; // 为同一个session
    }

    return NULL; // session不存在或者value值不同
}

void CSessionManager::ClearCache(time_t& stTime)
{
    return;
}
