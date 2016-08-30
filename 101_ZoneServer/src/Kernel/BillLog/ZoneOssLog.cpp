#include <time.h>

#include "GameProtocol.hpp"
#include "TimeUtility.hpp"
#include "ZoneOssLog.hpp"
#include "GameRole.hpp"
#include "ConfigHelper.hpp"
#include "StringUtility.hpp"
#include "LogAdapter.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//运营OSS日志接口

#define GAME_OSSLOG_CONFIG_FILE "../conf/OssLogConfig.xml"

using namespace ServerLib;

//玩家登录的日志
void CZoneOssLog::TraceLogin(CGameRoleObj& stRoleObj)
{
	CGameSessionObj* pstSessionObj = CModuleHelper::GetSessionManager()->FindSessionByRoleID(stRoleObj.GetRoleID());
	if(!pstSessionObj)
	{
		return;
	}

    // uin | OSS_LOG_TYPE_LOGIN | time | LoginTime | LoginIP 
    TRACEBILL("%u|%d|%d|%d|%s\n", stRoleObj.GetUin(), OSS_LOG_TYPE_LOGIN, CTimeUtility::GetNowTime(), 
              stRoleObj.GetLoginTime(), pstSessionObj->GetClientIP());

	return;
}

//玩家登出的日志
void CZoneOssLog::TraceLogout(CGameRoleObj& stRoleObj)
{
	CGameSessionObj* pstSessionObj = CModuleHelper::GetSessionManager()->FindSessionByRoleID(stRoleObj.GetRoleID());
	if(!pstSessionObj)
	{
		return;
	}

    // uin | OSS_LOG_TYPE_LOGOUT | time | LogoutTime | LoginIP 
    TRACEBILL("%u|%d|%d|%d|%s\n", stRoleObj.GetUin(), OSS_LOG_TYPE_LOGOUT, CTimeUtility::GetNowTime(), 
              stRoleObj.GetLogoutTime(), pstSessionObj->GetClientIP());

	return;
}

//记录战斗的日志
void CZoneOssLog::TraceRoleCombat(unsigned uin, int iCrossID, int iStartTime, bool bIsSelfWin)
{
	//uin | OSS_LOG_TYPE_DOCOMBAT | time | cross | start_time | is_self_win
	TRACEBILL("%u|%d|%d|%d|%d|%d\n", uin, OSS_LOG_TYPE_DOCOMBAT, CTimeUtility::GetNowTime(), 
			  iCrossID, iStartTime, bIsSelfWin);

	return;
}

//玩家NPC交易日志
void CZoneOssLog::TraceItemTrade(unsigned uin, int iOpType, int iResChange, int iItemID, int iItemNum)
{
	//uin | OSS_LOG_TYPE_ITEMTRADE | time | optype | res_change | itemid | itemnum
	TRACEBILL("%u|%d|%d|%d|%d|%d|%d\n", uin, OSS_LOG_TYPE_ITEMTRADE, CTimeUtility::GetNowTime(), 
			  iOpType, iResChange, iItemID, iItemNum);

	return;
}

//玩家学习生活技能配方的日志
void CZoneOssLog::TraceAddLifeSkill(unsigned uin, int iSkillType, int iSkillID, int iAddTime)
{
	//uin | OSS_LOG_TYPE_ADDLIFESKILL | time | skill_type | skill_id | add_time
	TRACEBILL("%u|%d|%d|%d|%d|%d\n", uin, OSS_LOG_TYPE_ADDLIFESKILL, CTimeUtility::GetNowTime(), 
			  iSkillType, iSkillID, iAddTime);

	return;
}

//玩家开始小游戏的日志
void CZoneOssLog::TraceBeginMiniGame(unsigned uin, int iMiniGameType)
{
	//uin | OSS_LOG_TYPE_BEGINMINIGAME | time | minigame_type
	TRACEBILL("%u|%d|%d|%d\n", uin, OSS_LOG_TYPE_BEGINMINIGAME, CTimeUtility::GetNowTime(), iMiniGameType);

	return;
}

//玩家结束小游戏的日志
void CZoneOssLog::TraceEndMiniGame(unsigned uin, int iMiniGameType)
{
	//uin | OSS_LOG_TYPE_BEGINMINIGAME | time | minigame_type
	TRACEBILL("%u|%d|%d|%d\n", uin, OSS_LOG_TYPE_ENDMINIGAME, CTimeUtility::GetNowTime(), iMiniGameType);

	return;
}

//玩家小伙伴冲层的日志
void CZoneOssLog::TraceUnitFeed(unsigned uin, int iUnitConfigID, int iNewLevel)
{
	//uin | OSS_LOG_TYPE_UNITFEED | time | configid | newlevel
	TRACEBILL("%u|%d|%d|%d|%d\n", uin, OSS_LOG_TYPE_UNITFEED, CTimeUtility::GetNowTime(), iUnitConfigID, iNewLevel);

	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int CZoneOssLog::Initialize()
{
	//todo jasonxiong5
	//初始化Kingnet UdpLog
	//return _analyzer.Init(GAME_OSSLOG_CONFIG_FILE);
	return 0;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
