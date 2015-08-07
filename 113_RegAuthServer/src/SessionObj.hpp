#ifndef __SESSION_OBJ_HPP__
#define __SESSION_OBJ_HPP__

#include "ObjAllocator.hpp"
#include "Public.hpp"
#include "CacheTime.hpp"

using namespace ServerLib;

class CSessionObj : public CCacheTime
{
private:
    TNetHead_V2 m_stNetHead; // 来自客户端的socket连接信息，注意其中的有效字段都是网络字节序的
    unsigned short m_unValue; // 辅助数据，和NetHead中的sockfd一起标识这个session
    bool m_bActive; // 该session结点是否处于active状态，创建结点时置为active状态，释放结点时置为非active状态

public:
    CSessionObj();
    virtual ~CSessionObj();

public:
    int Create(TNetHead_V2& stNetHead, const unsigned short unValue);
    void SetActive(bool bActive);

public:
    TNetHead_V2* GetNetHead();
    bool IsActive();
    unsigned int GetSessionFD() const;
    unsigned short GetValue() const;
    void GetSockCreatedTime(char* pszTime, unsigned int uiBufLen);
    void GetMsgCreatedTime(char* pszTime, unsigned int uiBufLen);
};

#endif // __SESSION_OBJ_HPP__

