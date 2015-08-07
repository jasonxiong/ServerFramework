#ifndef __GAME_ROLE_HPP__
#define __GAME_ROLE_HPP__

#include <vector>
#include <string.h>

#include "GameProtocol.hpp"
#include "SessionDefine.hpp"
#include "RttUtility.hpp"
#include "ObjAllocator.hpp"
#include "TimeUtility.hpp"
#include "LogAdapter.hpp"
#include "AppTick.hpp"
#include "GameObjCommDef.hpp"
#include "RepThingsManager.hpp"
#include "FightUnitManager.hpp"

using namespace ServerLib;

// 防沉迷状态
typedef enum tagEnumAAStatus
{
    EAS_HEALTHY     = 0,        // 健康0 ~ 3小时
    EAS_TIRED       = 1,        // 疲劳3 ~ 5小时
    EAS_UNHEALTHY   = 2,        // 不健康 >5小时
} TAAStatus;

// 消息记录
typedef struct
{
    unsigned short m_wMsgID;    //消息ID
    unsigned char m_ucSource;   //来源
    timeval m_stTimeval;        //时间
}MsgInfo;

const int MAX_MSG_QUEUE_LEN = 400;

typedef struct tagMsgInfoQueue
{
    int m_iMsgQueueIndex;
    MsgInfo m_astMsgInfo[MAX_MSG_QUEUE_LEN];

    tagMsgInfoQueue()
    {
        memset(this, 0, sizeof(*this));
    };
}MsgInfoQueue;

// 模块锁
typedef enum
{
    MAIL_LOCK = 0,
    MARKET_LOCK = 1,
    MAX_LOCK_NUM = 2
}ENUM_LOCK;

class CGameSessionObj;
class CGameSceneObj;

class CGameRoleObj : public CObj
{
public:
    CGameRoleObj();
    virtual ~CGameRoleObj();
    virtual int Initialize();
    virtual int Resume();
    DECLARE_DYN

///////////////////////////////////////////////////////////////////////
public:

    // 初始化玩家属性
    int InitRole(const RoleID& stRoleID);

    void SetRoleID(const RoleID& stRoleID) { m_stRoleInfo.stRoleID.CopyFrom(stRoleID); };
    const RoleID& GetRoleID() { return m_stRoleInfo.stRoleID; };
    unsigned int GetUin() const { return m_stRoleInfo.stRoleID.uin(); };
    void SetUin(unsigned int uiUin) { m_stRoleInfo.stRoleID.set_uin(uiUin); };

    TROLEINFO& GetRoleInfo() { return m_stRoleInfo; };
    TUNITINFO& GetUnitInfo() { return m_stRoleInfo.stUnitInfo; };

    int GetSessionID();
    void SetSessionID(int iSessionID);
    CGameSessionObj* GetSession();

    //sznickname 名字
    void SetNickName(const char* strNickName);
    const char* GetNickName();

    //uStatus 状态
    unsigned GetRoleStatus() { return m_stRoleInfo.stUnitInfo.uiUnitStatus; };
    void SetRoleStatus(int uStatus) { m_stRoleInfo.stUnitInfo.uiUnitStatus = uStatus; };

    //iLastLogin 上次登录时间
    int GetLastLoginTime() ;
    void SetLastLoginTime(int iLastLoginTime) ;

    //iLastLogout 上次下线时间
    int GetLastLogoutTime(){ return m_stRoleInfo.stBaseInfo.iLastLogout;}
    void SetLastLogoutTime(int iLastLogoutTime){m_stRoleInfo.stBaseInfo.iLastLogout = iLastLogoutTime; LOGDEBUG("Debug: set lastlogouttime:%d, uin:%d\n", m_stRoleInfo.stBaseInfo.iLastLogout, m_stRoleInfo.stRoleID.uin());}
    bool IsFirstLoginToday();

    //iCreateTime 帐号创建的时间
    void SetCreateTime(int iCreateTime);
    int GetCreateTime();
    
    //总在线时间
    void SetOnlineTotalTime(int iOnlineTime);
    int GetOnlineTotalTime();
    int GetOnlineThisTime()const;

    //iLoginCount 玩家总的登录次数
    int GetLoginCount() {return m_stRoleInfo.stBaseInfo.iLoginCount;}
    void SetLoginCount(int iCount) { m_stRoleInfo.stBaseInfo.iLoginCount = iCount;}

    //iForbidTalkingTime 禁止玩家发言的时间
    void SetForbidTalkingTime(int iForbidTalkingTime)
    {
        m_stRoleInfo.stBaseInfo.iForbidTalkingTime = iForbidTalkingTime;
    }

    int GetForbidTalkingTime() const
    {
        return m_stRoleInfo.stBaseInfo.iForbidTalkingTime;
    }

    //iLoginTime 本次登录的时间
    void InitLoginTime();
    int GetLoginTime() ;

    //iLogoutTime 本次登出游戏的时间
    int GetLogoutTime() ;
    void SetLogoutTime(int iLogoutTime) ;

    //todo jasonxiong1 等后面开发GM模块时再统一开发
    bool IsGM() { return false; };

    //玩家的战场信息
    void SetBattlefieObjID(int iBattlefiedObjIndex) { m_stRoleInfo.stBaseInfo.iBattlefieldObjIndex = iBattlefiedObjIndex; };
    int GetBattlefieObjID() { return m_stRoleInfo.stBaseInfo.iBattlefieldObjIndex; };

    //玩家的战斗胜负信息
    void SetCombatResult(bool bIsWin) { m_stRoleInfo.stBaseInfo.bIsSelfWin = bIsWin; };
    bool GetCombatResult() { return m_stRoleInfo.stBaseInfo.bIsSelfWin; };

public:
    //基础信息的数据库操作函数
    void UpdateBaseInfoToDB(BASEDBINFO& rstBaseInfo);
    void InitBaseInfoFromDB(const BASEDBINFO& rstBaseInfo);

///////////////////////////////////////////////////////////////////////
public:

    void SetKicker(const World_KickRole_Request& rstKicker) {m_stKicker.CopyFrom(rstKicker);}
    World_KickRole_Request& GetKicker() {return m_stKicker;}

///////////////////////////////////////////////////////////////////////
///
public:
    // 刷新玩家的最后活动的信息
    void ActiveRefresh();
    bool IsUnactive();

public:
    // 世界聊天还剩余冷却时间
    int GetWorldChatCD();
    void RefreshWorldChatCD();

    //附近聊天
    void RefreshNearbyChatTime();
    bool CanChatNearbyNow();

public:
    // Tick 计算
    void OnTick();

public:
    // 防沉迷状态
    void SetAAStatus(TAAStatus enAAStatus) {m_enAAStatus = enAAStatus;};
    TAAStatus GetAAStatus() {return m_enAAStatus;};

/////////////////////////////////////////////////////////////////////////////////////////
public:
    // 调试使用
    void PrintMyself();

public:
    // 在线状态
    void SetOnline();
    void SetOffline();
    bool IsOnline();

    void SetForbidTime(int iForbidTime)
    {
        m_iForbidTime = iForbidTime;
    }

    int GetForbidTime() const
    {
        return m_iForbidTime;
    }

public:

    bool TryToRefreshActivityDayLimit();

public:
    // 真正的GM帐号, 不包括10000号以下的
    //todo jasonxiong 后续开发GM功能时再重新开发
    bool IsRealGM() { return false; }

/////////////////////////////////////////////////////////////////////////////////////////
// 成员变量
private:
    int m_iSessionID;

    //玩家角色的数据信息
    TROLEINFO m_stRoleInfo;

    //禁止登录时间
    int m_iForbidTime;

    // 最后世界聊天时间
    int m_iLastWorldChatTime;

    // 上次附近聊天时间
    int m_iLastNearbyChatTime;

private:
    // 好友
    //CFriendManager m_stFriendManager;

    // 操作记录
    int m_iLastActiveTime;

    // 记录踢我下线的人
    World_KickRole_Request m_stKicker;

    // Tick记录
    int m_iLastTickTime;
    int m_iLastUpdateDBTickTime;
    int m_iLastUpdateCheckTimeoutTime;
    int m_iLastMinTime;
    int m_iLastTenSecondTime;
        
    // 防沉迷状态
    TAAStatus m_enAAStatus;

public:
    bool IsLock(ENUM_LOCK eLock)
    {
        return m_abLock[eLock];
    }

    void UnLock(ENUM_LOCK eLock)
    {
        m_abLock[eLock] = false;
    }

    void Lock(ENUM_LOCK eLock)
    {
        m_abLock[eLock] = true;
    }

private:
    //行为锁
    //  请求限次，防止恶意请求造成异步事务异常，出现道具复制等问题
    bool m_abLock[MAX_LOCK_NUM];

public:

    //玩家背包管理器
    CRepThingsManager m_stRepThingsManager;

    CRepThingsManager& GetRepThingsManager();
    void UpdateRepThingsToDB(ITEMDBINFO& rstItemDBInfo);
    void InitRepThingsFromDB(const ITEMDBINFO& rstItemDBInfo);

    //玩家的战斗单位管理器
    CFightUnitManager m_stFightUnitManager;

    CFightUnitManager& GetFightUnitManager();
    int UpdateFightUnitToDB(FIGHTDBINFO& stFightDBInfo);
    int InitFightUnitFromDB(const FIGHTDBINFO& rstFightDBInfo);

public:
    void SetLogoutReason(int iReason);
    int GetLogoutReason() {return m_iLogoutReason;}

private:
    int m_iLogoutReason;

public:
    bool IsMsgFreqLimit(const int iMsgID) const;
    void PushMsgID(const int iMsgID, const unsigned char ucSource);
private:
    //消息队列
    MsgInfoQueue m_stMsgInfoQueue;

public:
    unsigned char GetLogoutRequestReason() {return m_ucLogoutReason;}
    void SetLogoutRequestReason(unsigned char ucLogoutReason) {m_ucLogoutReason = ucLogoutReason;}

private:
    //记录下线原因
    unsigned char m_ucLogoutReason;

public:
    void OnTwoSecondTimerUp(time_t tNow);

private:
    int m_iLastTwoSecondTime;
};

#endif
