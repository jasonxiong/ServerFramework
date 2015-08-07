
local cc = require("combat_const")
local cm = require("combat_module")
local cl = require("combat_level")
local ck = require("combat_skill")
local cb = require("combat_buff")
local cv = require("combat_value")
local ci = require("combat_item")
local ct = require("combat_trap")

--[[
require("combat_trap")
require("combat_item")
]]--

local function _G_TRACKBACK_(msg)                          -- for CCLuaEngine traceback
  LOG_INFO("----------------------------------------")
  LOG_INFO("LUA ERROR: " .. tostring(msg) .. "\n")
  LOG_INFO(string.format(debug.traceback()))
  LOG_INFO("----------------------------------------")
  return msg
end

--战斗脚本的主入口函数

function ExecuteCombatScript(user_id, level_id, calledID)
  local function myExecuteCombatScript()
    if calledID == cc.COMBAT_START then       --通用初始化
      cm.log_info("<COMBAT PREPARE> level_id: %d hero_id: %d user_id: %d", level_id, cm.team_funcs.get_hero(user_id) or -1, user_id)
      cm.team_funcs.set_enemy_growth(user_id)
      cm.team_funcs.init_ai(user_id)
      cm.team_funcs.set_team_move(user_id, cc.COMBAT_CAMP_ACTIVE)
      --cm.log_info("<COMBAT START> level_id: %d hero_id: %d user_id: %d", level_id, cm.team_funcs.get_hero(user_id), user_id)
      cm.showDate(user_id)
      math.randomseed(os.clock())
    else
      if IS_COMBAT_ROUND_END(user_id) then
        cm.log_info("<COMBAT ROUND> level_id: %d hero_id: %d user_id: %d round: %d", level_id, cm.team_funcs.get_hero(user_id) or -1, user_id, GET_COMBAT_ROUND_NUM(user_id))
        cm.team_funcs.recover_all(user_id, 0, 5) --回合恢复（回蓝5%）
      end
    end
    local result, level_func = 0, cl.get_level_function(level_id)              
    if level_func then result = level_func(user_id,level_id,calledID) end --特定关卡逻辑
    if calledID == cc.COMBAT_START then --初始化完成后计算经验
      if not cm.team_funcs.hero_dead(user_id,1,cc.COMBAT_CAMP_ACTIVE) then --主角存在，否则是观战，无经验掉落
        local hero_id = cm.team_funcs.get_hero(user_id)
        local hero_lv = cm.unit_funcs.get_unit_level(hero_id)
        local enemy_lv = cm.team_funcs.get_enemy_level(user_id)
        local drop_lv = hero_lv
        if hero_lv > enemy_lv then
          drop_lv = math.max(1,math.floor(drop_lv / math.pow(2,hero_lv-enemy_lv)))
        else
          drop_lv = math.min(hero_lv+5,hero_lv+ (enemy_lv-hero_lv))
        end
        local drop_id = 10000+drop_lv
        cm.log_info("<COMBAT EXP> user_id: %d drop_id: %d", user_id, drop_id)
        ADD_EXTRA_COMBAT_DROPID(user_id, drop_id, 0)
      end
    end
    if result==0 then cm.level_funcs.default_win(user_id) end --通用胜负判断
  end
  local good,msg = xpcall(myExecuteCombatScript, _G_TRACKBACK_)
  if not good then error(msg) end
end

--战斗使用技能效果的脚本主入口函数
function ExecuteSkillScript(user_id, level_id, skill_id, cast_unit_id, target_posx, target_posy, execute_type)
  local r={result=0}
  local function myExecuteSkillScript()
    local result, skill_func = 0, ck.get_skill_function(skill_id)
    if skill_func then result = skill_func(user_id, level_id, skill_id, cast_unit_id, target_posx, target_posy, execute_type) end
    r.result = result
  end
  xpcall(myExecuteSkillScript, _G_TRACKBACK_)
  return r.result
end

--战斗BUFF效果的脚本主入口函数
function ExcuteBuffScript(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  local r={result=0}
  local function myExcuteBuffScript()
    local result, buff_func = 0, cb.get_buff_function(buff_config_id)
    if buff_func then result = buff_func(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id) end
    r.result = result
  end
  xpcall(myExcuteBuffScript, _G_TRACKBACK_)
  return r.result
end

--战斗AI技能价值计算的脚本主入口函数
function ExecuteSkillAIScript(user_id, level_id, skill_id, caster_unit_id, move_x, move_y, target_x, target_y, target_range, damage_range)
  local r={result=0}
  local function myExecuteSkillAIScript()
    local result, ai_func = 0, cv.get_skill_value(skill_id)
    if ai_func then
      --cm.log_info("<COMBAT AI FUNC> skill id: %d", skill_id)
      result = ai_func(user_id, level_id, skill_id, caster_unit_id, move_x, move_y, target_x, target_y, target_range, damage_range)
    else
      --cm.log_info("<COMBAT AI NORMAL ATK> unit_id: %d x: %d y: %d", caster_unit_id, target_x, target_y)
      local x,y = cm.capi_funcs.get_unit_pos(cm.team_funcs.get_hero(user_id))
      if target_x==x and target_y==y then
        --cm.log_info("<COMBAT AI NORMAL ATK> unit_id: %d x: %d y: %d", caster_unit_id, move_x, move_y)
      end
      result = cv.normal_attack_value(user_id, level_id, skill_id, caster_unit_id, move_x, move_y, target_x, target_y, target_range, damage_range)
    end
    --if result>0 then cm.log_info("<ExecuteSkillAIScript> result: %d", result) end
    r.result = result
  end
  xpcall(myExecuteSkillAIScript, _G_TRACKBACK_)
  return r.result
end

--战斗使用道具物品的脚本主入口函数
function ExecuteUseItemScript(user_id, level_id, item_id, caster_unit_id, target_x, target_y)
  local r={result=0}
  local function myExecuteUseItemScript()
    if cm.skill_funcs.target_has_buff(caster_unit_id, 10370401) then --巩光杰 精通药性
      local bufftype = math.random(1,6)
      local buffid = 10000 * bufftype + 101
      ADD_UNIT_BUFF(user_id, caster_unit_id, caster_unit_id, buffid)
    end
    local result, item_func = 0, ci.get_item_function(item_id)
    if item_func then result = item_func(user_id, level_id, item_id, caster_unit_id, target_x, target_y) end
    r.result = result
  end
  xpcall(myExecuteUseItemScript,_G_TRACKBACK_)
  return r.result
end

--战斗机关触发处理的主入口函数
function ExecuteTrapTriggerScript(user_id, trigger_unit_id, trap_obj_id, trap_config_id, trigger_type) --trigger_type 触发类型
  local r={result=0}
  local function myExecuteTrapTriggerScript() --trigger_type 触发类型
    local result, trap_func = 0, ct.get_trap_function(trap_config_id)
    if trap_func then
        result = trap_func(user_id, trigger_unit_id, trap_obj_id, trap_config_id, trigger_type)
    else  --默认处理
      ADD_UNIT_BUFF(user_id, trigger_unit_id, trigger_unit_id, 10170401)    --禁止移动  移形换位
      if trigger_type==1 then DELETE_COMBAT_TRAP(user_id, trap_obj_id) end  --陷阱
    end
    r.result = result
  end
  xpcall(myExecuteTrapTriggerScript,_G_TRACKBACK_)
  return r.result
end

--计算赠送礼物好感度加成的脚本主入口函数

function ExecuteGiftScript(user_id, unit_id, unit_gender)
  local r={result=0}
  local function myExecuteGiftScript() --unit_id 为玩家角色表配置ID，因此其它接口无效。只能自己在cc里加数据
    local multi = 100
    local YY,MM,DD,TT = cm.level_funcs.get_role_date(user_id)
    
    local hobby = cc.hero_hobby[unit_id]
    if hobby then
      local adj = hobby[(MM-1)*3+DD]
      multi = multi * adj  --根据送礼日期修正送礼好感度加成
    end
    --cm.log_info("<GIFT SEND> YY: %d MM: %d DD: %d  day_index: %d multi: %d", YY, MM, DD, (MM-1)*3+DD, multi)
    r.result = multi
  end
  xpcall(myExecuteGiftScript,_G_TRACKBACK_)
  return r.result
end


--[[

]]--













