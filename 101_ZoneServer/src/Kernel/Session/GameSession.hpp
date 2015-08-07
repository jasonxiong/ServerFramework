#ifndef __GAME_SESSION_HPP__
#define __GAME_SESSION_HPP__

#include "SessionDefine.hpp"
#include "ObjAllocator.hpp"
#include "ConfigHelper.hpp"
#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"

using namespace ServerLib;
using namespace GameConfig;

const int MAX_GAMESESSION_NUMBER = MAX_ROLE_OBJECT_NUMBER_IN_ZONE;

class CGameRoleObj;

class CGameSessionObj : public CObj
{
public:
    CGameSessionObj();
    virtual ~CGameSessionObj();
    virtual int Initialize();
    DECLARE_DYN

public:
    ////////////////////////////////////////////////////////////////////////////////
    // 会话信息
    void SetRoleID(const RoleID& stRoleID) { m_stRoleID.CopyFrom(stRoleID); };
    const RoleID& GetRoleID() { return m_stRoleID; };

    // 会话ID (Socket FD)
    void SetID(int iID) {m_iID = iID;}
    int GetID() {return m_iID;}

    // 会话连接
    void SetNetHead(const TNetHead_V2* pNetHead);
    const TNetHead_V2& GetNetHead(){return m_stNetHead;};

    const char* GetClientIP() {return &m_szClientIP[0];}

    // 会话客户端版本号
    void SetClientVersion(int iVersion) {m_iClientVersion = iVersion;};
    int GetClientVersion() {return m_iClientVersion;};

    // 登录后, 绑定的角色对象
    CGameRoleObj* GetBindingRole();
    void SetBindingRole(const CGameRoleObj* pZoneRoleObj);

    // 创建时间
    int GetCreateTime() {return m_iCreateTime;};

public:
    void SetSessionKey(const char* pszSessionKey);
    const char* szGetSessionKey() {return m_szSessionKey;}

private:

    //socket FD
    int m_iID;
    TNetHead_V2 m_stNetHead;
    int m_iRoleIdx;
    int m_iClientVersion;
    int m_iCreateTime;

    char m_szClientIP[MAX_IPV4_LENGTH];

    char m_szSessionKey[MAX_SESSIONKEY_LENGTH];

    RoleID m_stRoleID;

public:
    unsigned char GetLoginReason() {return m_ucLoginReason;}
    void SetLoginReason(unsigned char ucLoginReason) {m_ucLoginReason = ucLoginReason;}

private:
    unsigned char m_ucLoginReason;
};

#endif

