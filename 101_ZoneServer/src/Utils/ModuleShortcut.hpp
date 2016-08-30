#ifndef __EASY_USE_HPP
#define __EASY_USE_HPP

#include "ConfigManager.hpp"
#include "LogAdapter.hpp"

using namespace ServerLib;

//这些函数返回值务必用引用或者指针 或者起const引用，指针
//否则会有很大性能消耗

//获取战斗单位的配置
CSFightUnitConfigManager& FightUnitCfgMgr();

//获取战斗关卡配置
CSBattleCrossConfigManager& BattleCrossCfgMgr();

//获取关卡怪物组配置
CSCrossMonsterConfigManager& CrossMonterCfgMgr();

//获取战斗单位AI的配置
CSFightUnitAIConfigManager& FightUnitAICfgMgr();

//获取战斗怪物的配置
CSMonsterConfigManager& MonsterCfgMgr();

//获取战场阻挡的配置
CBattlefieldPathConfigManager& BattlefieldPathCfgMgr();

//获取战斗技能的配置
CFightSkillConfigManager& FightSkillCfgMgr();

//获取技能释放距离配置
CSkillRangeConfigManager& SkillRangeCfgMgr();

//获取技能释放范围配置
CSkillAreaConfigManager& SkillAreaCfgMgr();

//获取技能修正配置
CSkillScoreConfigManager& SkillScoreCfgMgr();

//获取技能伤害配置
CSkillDamageConfigManager& SkillDamageCfgMgr();

//获取战斗Buff配置
CSFightBuffConfigManager& FightBuffCfgMgr();

//获取随机掉落的配置
CSDropRewardConfigManager& DropRewardCfgMgr();

//技能伙伴合击的配置
CSJointAtkConfigManager& JointAtkCfgMgr();

//战场机关的配置
CSTrapConfigManager& TrapCfgMgr();

//游戏物品的配置
CSGameItemConfigManager& ItemCfgMgr();

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
