#ifndef __SESSION_MANAGER_HPP__
#define __SESSION_MANAGER_HPP__

#include "AccountPublic.hpp"
#include "SessionObj.hpp"
#include "SingletonTemplate.hpp"
#include "AccountMsgTransceiver.hpp"

const int MAX_CODEQUEUE_NUM = 2;

class CSessionManager
{
public:
	CSessionManager() {};

private:
    CSessionObj m_astSessionList[MAX_CODEQUEUE_NUM][MAX_ROLE_OBJECT_NUMBER_IN_WORLD];
    unsigned short m_unSeed; // 标识session的辅助数据的种子，即初始值
    unsigned int m_uiActiveNumber; // 处于active状态的缓存结点的个数

public:
    int Initialize(bool bResume);
    int CheckSession(TNetHead_V2& stNetHead);
    CSessionObj* CreateSession(TNetHead_V2& stNetHead);
    CSessionObj* GetSession(const unsigned int uiSessionFD, const unsigned short unValue = 0);
    int DeleteSession(const unsigned int uiSessionFD);

private:
    unsigned short GenerateValue();
	CSessionObj* GetSessionByFd(unsigned int uiSessionFD);

public:
    void ClearCache(time_t& stTime); // 清理在参数stTime之前（<=stTime）创建的cache结点
};

#endif // __SESSION_MANAGER_HPP__

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
