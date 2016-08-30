#include "GameProtocol.hpp"
#include "ModuleShortcut.hpp"
#include "ModuleHelper.hpp"

//获取战斗单位的配置
CSFightUnitConfigManager& FightUnitCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetFightUnitConfigManager();
}

//获取战斗关卡配置
CSBattleCrossConfigManager& BattleCrossCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetBattleCrossConfigManager();
}

//获取关卡怪物组配置
CSCrossMonsterConfigManager& CrossMonterCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetCrossMonsterConfigManager();
}

//获取战斗单位AI的配置
CSFightUnitAIConfigManager& FightUnitAICfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetFightUnitAIConfigManager();
}

//获取战斗怪物的配置
CSMonsterConfigManager& MonsterCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetMonsterConfigManager();
}

//获取战场阻挡的配置
CBattlefieldPathConfigManager& BattlefieldPathCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetBattlefieldPathConfigManager();
}

//获取战斗技能的配置
CFightSkillConfigManager& FightSkillCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetFightSkillConfigManager();
}

//获取技能释放距离配置
CSkillRangeConfigManager& SkillRangeCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetSkillRangeConfigManager();
}

//获取技能释放范围配置
CSkillAreaConfigManager& SkillAreaCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetSkillAreaConfigManager();
}

//获取技能修正配置
CSkillScoreConfigManager& SkillScoreCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetSkillScoreConfigManager();
}

//获取技能伤害配置
CSkillDamageConfigManager& SkillDamageCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetSkillDamageConfigManager();
}

//获取战斗Buff配置
CSFightBuffConfigManager& FightBuffCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetFightBuffConfigManager();
}

//获取随机掉落的配置
CSDropRewardConfigManager& DropRewardCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetDropRewardConfigManager();
}

//技能伙伴合击的配置
CSJointAtkConfigManager& JointAtkCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetJointAtkConfigManager();
}

//战场机关的配置
CSTrapConfigManager& TrapCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetTrapConfigManager();
}

//游戏物品的配置
CSGameItemConfigManager& ItemCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetGameItemConfigManager();
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
