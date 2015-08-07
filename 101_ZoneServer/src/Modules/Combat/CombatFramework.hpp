#ifndef __COMBAT_FRAMEWORK_HPP__
#define __COMBAT_FRAMEWORK_HPP__

#include "GameProtocol.hpp"

class CGameRoleObj;
class CBattlefieldObj;
class CCombatFramework
{
public:

    static CCombatFramework* Instance();

    ~CCombatFramework();

    //玩家开始战斗的请求
    int DoCombat(CGameRoleObj& stRoleObj, const Zone_DoCombat_Request& rstRequest, bool bNeedResp=true);

    //玩家主角战斗单位移动的请求
    int DoCombatMove(CGameRoleObj& stRoleObj, const Zone_CombatMove_Request& rstRequest);

    //玩家主角战斗单位行动的请求
    int DoCombatAction(CGameRoleObj& stRoleObj, const Zone_CombatAction_Request& rstRequest);

    //玩家主角
    int DoUpdatePartnerAI(CGameRoleObj& stRoleObj, const Zone_UpdatePartnerAI_Request& rstRequest);

    //发起PVP战斗的请求
    int StartPVPCombat(CGameRoleObj& stRoleObj, const Zone_StartPVPCombat_Request& rstRequest);

    //接受PVP战斗的请求
    int AcceptPVPCombat(CGameRoleObj& stRoleObj, const Zone_AcceptPVPCombat_Request& rstRequest);

    //设置战斗阵型的请求
    int SetCombatForm(CGameRoleObj& stRoleObj, const Zone_SetCombatForm_Request& rstRequest);

    //结束玩家战斗
    void FinCombat(CGameRoleObj& stRoleObj);

private:

    CCombatFramework();

    //初始化PVE战斗战场信息
    int InitPveBattlefield(CGameRoleObj& stRoleObj, int iLevelID);

    //初始化PVP战斗战场信息
    int InitPVPBattlefield(CGameRoleObj& stActiveRoleObj, CGameRoleObj& stPassiveRoleObj);

    //创建战场对象，返回CBattlefieldObj的index
    CBattlefieldObj* CreateBattlefieldObj(int& iBattlefieldObjIndex);

    //推送战场初始化信息
    int SendBattlefieldInitNotify(CGameRoleObj& stRoleObj);

    //发送处理成功的返回
    int SendSuccessResponse();

private:

    unsigned int m_uiUin;

    static GameProtocolMsg m_stGameMsg;
};

#endif

