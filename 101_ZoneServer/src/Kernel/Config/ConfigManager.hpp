#ifndef __CONIFG_MANAGER_HPP__
#define __CONIFG_MANAGER_HPP__

#include "HandlerFactory.hpp"
#include "LogAdapter.hpp"
#include "TemplateConfigManager.hpp"
#include "QMGMPrivConfigManager.hpp"
#include "BattlefieldPathConfigManager.hpp"

using namespace ServerLib;

// 消息最大CD时间 2000ms
const int MAX_MSG_INTERVAL_TIME = 2000;

// 默认不限制CD
const int DEFAULT_MSG_INTERVAL = 0;

//战斗单位配置管理器
extern const char GAME_FIGHT_UNIT_CONFIG_FILE[];
typedef CTemplateConfigManager<SFightUnitConfig, MAX_FIGHT_UNIT_NUM, GAME_FIGHT_UNIT_CONFIG_FILE> CSFightUnitConfigManager;

//战斗怪物配置管理器
extern const char GAME_FIGHT_MONSTER_CONFIG_FILE[];
typedef CTemplateConfigManager<SMonsterConfig, MAX_MONSTER_CONFIG_NUM, GAME_FIGHT_MONSTER_CONFIG_FILE> CSMonsterConfigManager;

//战斗关卡配置管理器
extern const char GAME_BATTLE_CROSS_CONFIG_FILE[];
typedef CTemplateConfigManager<SBattleCrossConfig, MAX_BATTLE_CROSS_NUM, GAME_BATTLE_CROSS_CONFIG_FILE> CSBattleCrossConfigManager;

//关卡怪物组配置管理器
extern const char GAME_CROSS_MONSTER_CONFIG_FILE[];
typedef CTemplateConfigManager<SCrossMonsterConfig, MAX_CROSS_MONSTER_CONFIG_NUM, GAME_CROSS_MONSTER_CONFIG_FILE> CSCrossMonsterConfigManager;

//战斗技能配置管理
extern const char GAME_FIGHT_SKILL_CONFIG_FILE[];
typedef CTemplateConfigManager<SFightUnitSkillConfig, MAX_FIGHT_SKILL_CONFIG_NUM, GAME_FIGHT_SKILL_CONFIG_FILE> CFightSkillConfigManager;

//技能释放距离配置管理
extern const char GAME_SKILL_RANGE_CONFIG_FILE[];
typedef CTemplateConfigManager<SSkillRangeConfig, MAX_SKILL_RANGE_CONFIG_NUM, GAME_SKILL_RANGE_CONFIG_FILE> CSkillRangeConfigManager;

//技能释放范围配置管理
extern const char GAME_SKILL_AREA_CONFIG_FILE[];
typedef CTemplateConfigManager<SSkillAreaConfig, MAX_SKILL_AREA_CONFIG_NUM, GAME_SKILL_AREA_CONFIG_FILE> CSkillAreaConfigManager;

//技能效果修正配置
extern const char GAME_SKILL_SCORE_CONFIG_FILE[];
typedef CTemplateConfigManager<SSkillScoreConfig, MAX_SKILL_SCORE_CONFIG_NUM, GAME_SKILL_SCORE_CONFIG_FILE> CSkillScoreConfigManager;

//技能伤害效果配置
extern const char GAME_SKILL_DAMAGE_CONFIG_FILE[];
typedef CTemplateConfigManager<SSkillDamageConfig, MAX_SKILL_DAMAGE_CONFIG_NUM, GAME_SKILL_DAMAGE_CONFIG_FILE> CSkillDamageConfigManager;

//技能Buff的配置
extern const char GAME_FIGHT_BUFF_CONFIG_FILE[];
typedef CTemplateConfigManager<SFightBuffConfig, MAX_FIGHT_BUFF_CONFIG_NUM, GAME_FIGHT_BUFF_CONFIG_FILE> CSFightBuffConfigManager;

//随机掉落的配置
extern const char GAME_DROP_REWARD_CONFIG_FILE[];
typedef CTemplateConfigManager<SDropRewardConfig, MAX_DROP_REWARD_CONFIG_NUM, GAME_DROP_REWARD_CONFIG_FILE> CSDropRewardConfigManager;

//战斗单位AI配置
extern const char GAME_FIGHTUNIT_AI_CONFIG_FILE[];
typedef CTemplateConfigManager<SFightUnitAIConfig, MAX_FIGHTUNIT_AI_CONFIG_NUM, GAME_FIGHTUNIT_AI_CONFIG_FILE> CSFightUnitAIConfigManager;

//战斗技能伙伴合击的配置
extern const char GAME_JOINT_ATTACK_CONFIG_FILE[];
typedef CTemplateConfigManager<SJointAttackConfig, MAX_JOINT_ATTACK_CONFIG_NUM, GAME_JOINT_ATTACK_CONFIG_FILE> CSJointAtkConfigManager;

//战斗战场机关的配置
extern const char GAME_COMBAT_TRAP_CONFIG_FILE[];
typedef CTemplateConfigManager<STrapConfig, MAX_COMBAT_TRAP_CONFIG_NUM, GAME_COMBAT_TRAP_CONFIG_FILE> CSTrapConfigManager;

//游戏玩家物品的配置
extern const char GAME_USER_ITEM_CONFIG_FILE[];
typedef CTemplateConfigManager<SGameItemConfig, MAX_GAME_ITEM_CONFIG_NUM, GAME_USER_ITEM_CONFIG_FILE> CSGameItemConfigManager;

class CConfigManager
{
public:
    int Initialize(bool bResumeMode);

    //GM权限配置
    CQMGMPrivConfigManager& GetGMPrivConfigManager() { return m_stQMGMPrivConfigManager; };

    const int GetMsgInterval(const unsigned int uiMsgID)const;
    int LoadMsgInterval();
    int LoadMsgDiableList();

    int GetServerStartTime(int& iServerStartTime);

    //战斗单位配置管理器
    CSFightUnitConfigManager& GetFightUnitConfigManager() { return m_stSFightUnitConfigManager; };

    //战斗关卡配置管理器
    CSBattleCrossConfigManager& GetBattleCrossConfigManager() { return m_stSBattleCrossConfigManager; };

    //关卡怪物配置管理器
    CSCrossMonsterConfigManager& GetCrossMonsterConfigManager() { return m_stCrossMonsterConfigManager; };

    //战斗单位AI配置管理器
    CSFightUnitAIConfigManager& GetFightUnitAIConfigManager() { return m_stSFightUnitAIConfigManager; };

    //战斗怪物配置管理器
    CSMonsterConfigManager& GetMonsterConfigManager() { return m_stSMonsterConfigManager; };

    //战场地图信息配置管理器
    CBattlefieldPathConfigManager& GetBattlefieldPathConfigManager() { return m_stBattlefieldPathConfigManager; };

    //战斗技能配置管理器
    CFightSkillConfigManager& GetFightSkillConfigManager() { return m_stFightSkillConfigManager; };

    //技能释放距离管理器
    CSkillRangeConfigManager& GetSkillRangeConfigManager() { return m_stSkillRangeConfigManager; };

    //技能释放范围管理器
    CSkillAreaConfigManager& GetSkillAreaConfigManager() { return m_stSkillAreaConfigManager; };

    //技能效果修正管理器
    CSkillScoreConfigManager& GetSkillScoreConfigManager() { return m_stSkillScoreConfigManager; };

    //技能伤害配置管理器
    CSkillDamageConfigManager& GetSkillDamageConfigManager() { return m_stSkillDamageConfigManager; };

    //战斗Buff配置管理器
    CSFightBuffConfigManager& GetFightBuffConfigManager() { return m_stFightBuffConfigManager; };

    //随机奖励配置管理器
    CSDropRewardConfigManager& GetDropRewardConfigManager() { return m_stDropRewardConfigManager; };

    //战斗技能伙伴合击配置管理器
    CSJointAtkConfigManager& GetJointAtkConfigManager() { return m_stJointAtkConfigManager; };

    //战斗战场机关配置管理器
    CSTrapConfigManager& GetTrapConfigManager() { return m_stTrapConfigManager; };

    //游戏物品配置管理器
    CSGameItemConfigManager& GetGameItemConfigManager() { return m_stGameItemConfigManager; };

private:
    int PrintConfigMemoryStatics();

private:

    //GM工具权限配置管理器
    CQMGMPrivConfigManager m_stQMGMPrivConfigManager;

    //战斗单位配置管理器
    CSFightUnitConfigManager m_stSFightUnitConfigManager;

    //战斗关卡配置管理器
    CSBattleCrossConfigManager m_stSBattleCrossConfigManager;

    //关卡怪物组配置管理器
    CSCrossMonsterConfigManager m_stCrossMonsterConfigManager;

    //战斗单位AI配置管理器
    CSFightUnitAIConfigManager  m_stSFightUnitAIConfigManager;

    //战斗怪物配置管理器
    CSMonsterConfigManager  m_stSMonsterConfigManager;

    //战场地图信息配置管理器
    CBattlefieldPathConfigManager m_stBattlefieldPathConfigManager;

    //战斗技能配置管理器
    CFightSkillConfigManager m_stFightSkillConfigManager;

    //战斗技能释放距离配置管理器
    CSkillRangeConfigManager m_stSkillRangeConfigManager;

    //战斗技能释放范围配置管理器
    CSkillAreaConfigManager m_stSkillAreaConfigManager;

    //战斗技能效果修正配置管理器
    CSkillScoreConfigManager m_stSkillScoreConfigManager;

    //战斗技能伤害配置管理器
    CSkillDamageConfigManager m_stSkillDamageConfigManager;

    //战斗Buff配置管理器
    CSFightBuffConfigManager m_stFightBuffConfigManager;

    //战斗奖励配置管理器
    CSDropRewardConfigManager m_stDropRewardConfigManager;

    //技能伙伴合击配置管理器
    CSJointAtkConfigManager m_stJointAtkConfigManager;

    //战场关卡配置管理器
    CSTrapConfigManager m_stTrapConfigManager;

    //游戏物品配置管理器
    CSGameItemConfigManager m_stGameItemConfigManager;
};

#endif
