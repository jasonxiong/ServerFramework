
local cc = require("combat_const")
local cm = require("combat_module")

local CV = {}

CV.skill_value_map = {}
function CV.register_skill_value(skill_id, func)
  CV.skill_value_map[skill_id] = func
end
function CV.get_skill_value(skill_id)
  return CV.skill_value_map[skill_id]
end

---------------------------------------------------------------------------------------
--以下为模板
---------------------------------------------------------------------------------------

--命中率
local function get_hit_rate(user_id, caster_unit_id, target_unit_id, attack_face, check_face)
  local hit = GET_UNIT_SCORE(caster_unit_id, cc.SCORE_NORMAL_HIT)
  local avo = GET_UNIT_SCORE(target_unit_id, cc.SCORE_NORMAL_AVO)
  if check_face then  --面向判断
    if cm.skill_funcs.is_side_attack(attack_face,target_unit_id) then avo = avo * 0.5 end
    if cm.skill_funcs.is_back_attack(attack_face,target_unit_id) then avo = avo * 0.8 end
  end
  return hit / (hit + avo)
end
--精准率（1-格挡率）
local function get_acc_rate(user_id, caster_unit_id, target_unit_id)
  local acc = GET_UNIT_SCORE(caster_unit_id, cc.SCORE_NORMAL_ACC)
  local par = GET_UNIT_SCORE(target_unit_id, cc.SCORE_NORMAL_PAR)
  return acc / (acc + par)
end
--暴击率
local function get_cri_rate(user_id, caster_unit_id, target_unit_id)
  local cri = GET_UNIT_SCORE(caster_unit_id, cc.SCORE_NORMAL_CRI)
  local tou = GET_UNIT_SCORE(target_unit_id, cc.SCORE_NORMAL_TOU)
  return cri / (cri + tou)
end
--伤害率（1-减免率）
local function get_atk_rate(user_id, caster_unit_id, target_unit_id)
  local atk = GET_UNIT_SCORE(caster_unit_id, cc.SCORE_NORMAL_ATK)
  local def = GET_UNIT_SCORE(target_unit_id, cc.SCORE_NORMAL_DEF)
  return atk / (atk + def)
end
--普攻伤害评估
local function get_normal_dmg(user_id, caster_unit_id, target_unit_id)
  local base_dmg = 10
  local att_type = cm.capi_funcs.get_atk_type(caster_unit_id)
  local dmg_attr = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_STR + cc.dmg_score[att_type]-1)
  return base_dmg + dmg_attr*150/100  --未考虑range因素
end
--伤害预估
local function dmg_estimate(user_id, caster_unit_id, target_unit_id, target_range_param, attack_face)
  local dmg = get_normal_dmg(user_id, caster_unit_id, target_unit_id)
  local hitrr = get_hit_rate(user_id, caster_unit_id, target_unit_id, attack_face, true)   --命中率
  hitrr = hitrr * target_range_param / 10000
  --print(hitrr)
  local accrr = get_acc_rate(user_id, caster_unit_id, target_unit_id)      --精准率（1-格挡率）
  --print(accrr)
  local crirr = get_cri_rate(user_id, caster_unit_id, target_unit_id)      --暴击率
  --print(crirr)
  local atkrr = get_atk_rate(user_id, caster_unit_id, target_unit_id)      --伤害率（1-减免率）
  --print(atkrr)
  local dmg = dmg * hitrr * ((1-crirr) + crirr * 1.2) * (accrr + (1-accrr) * 0.8) * atkrr
  return math.max(1,dmg)
end
--是否夹击
local function has_mirror_attack(user_id, caster_unit_id, target_unit_id, move_posx, move_posy, target_posx, target_posy)
  --LOG_INFO("mirror attack check-------------------------------------")
  --LOG_INFO("caster_unit_id : " .. caster_unit_id)
  --LOG_INFO("target_unit_id : " .. target_unit_id)
  
  local mirror_x = target_posx + (target_posx - move_posx)
  local mirror_y = target_posy + (target_posy - move_posy)
  local mirror_unit = cm.capi_funcs.get_unit_by_pos(user_id, mirror_x, mirror_y)
  if mirror_unit>=0 then
    --LOG_INFO("mirror_unit_id : " .. mirror_unit)
    if not cm.skill_funcs.is_enemy(user_id,caster_unit_id,mirror_unit) then
      local normal_attack_range = cm.skill_funcs.get_normal_attack_range(user_id, mirror_unit)
      local within_range = GET_TARGET_ATTACK_DISTANCE(mirror_unit, target_unit_id, normal_attack_range)
      if within_range > 0 then return 2 end
    end
  end
  return 0
end
--BLANK SKILL TARGET-SINGLE
local function skill_single(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    local hp = cm.capi_funcs.get_unit_attr(target,cc.FIGHT_ATTR_HP) --目标血越少，价值越高
    local maxhp = cm.capi_funcs.get_unit_attr(target,cc.FIGHT_ATTR_HPMAX) 
    d = d * maxhp/hp
    --d = 100 *d / hp --hp超过3000会有问题
    d = d * (1 + has_mirror_attack(user_id, caster_unit_id, target, move_posx, move_posy, target_posx, target_posy)) --夹击优先
    --cm.log_info("<COMBAT SKILL SINGLE> target: %d dmg_estimate: %f", target, d)
    return d
  end
  return 0
end
--BLANK SKILL TARGET-AROUND
local function skill_around(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_AROUND, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
  for key in pairs(targets) do
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end

--普攻价值
function CV.normal_attack_value(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  return skill_single(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
end

--锥形伤害
local function triangle_dmg(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  local count = 0
  if target >= 0 then
    local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
    local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_TRIANG, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
    for key in pairs(targets) do
      local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
      result = result + d
      count = count + 1
    end
  end
  if count <2 then return 0 end
  return result
end

--单体治疗
local function single_heal(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local unit = -1
  if (move_posx==target_posx and move_posy==target_posy) then --对自身施法
    unit = caster_unit_id
  else
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    if (target_posx==x and target_posy==y) then return 0 end  --对自身原来位置施法
  end
  --移动后对其他单位施法
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >=0 then
    if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then return 0 end
    unit = target
  end
  if unit >=0 then
    local hp = cm.capi_funcs.get_unit_attr(caster_unit_id, 3)
    local maxhp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HPMAX)
    if hp/maxhp <0.8 then result =  maxhp/hp end
  end
  return result
end

---------------------------------------------------------------------------------------
--以下为具体角色技能
---------------------------------------------------------------------------------------

--仙音 10010201 菩提清心
CV.register_skill_value(10010201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local unit = -1
  if (move_posx==target_posx and move_posy==target_posy) then --对自身施法
    unit = caster_unit_id
  else
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    if (target_posx==x and target_posy==y) then return 0 end  --对自身原来位置施法
  end
  --移动后对其他单位施法
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >=0 then
    if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then return 0 end
    unit = target
  end
  if unit >=0 then
    if cm.buff_funcs.has_control_buff(unit) then
      result = result + 100
    end
  end
  return result
end)

--仙音 10010301 空山鸣涧
CV.register_skill_value(10010301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = 1
  local targets = GET_AREA_TARGET_UNIT(user_id, move_posx, move_posy, cc.AREA_AROUND, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
  local count = 0
  for key in pairs(targets) do
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    count = count + 1
    result = result + d
  end
  if count <2 then return 0 end
  return result
end)

--江瑜 10020201 放下屠刀
CV.register_skill_value(10020201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_SINGLE, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
  for key in pairs(targets) do
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    d = d + cm.capi_funcs.get_unit_attr(targets[key], 12) --攻击值高的优先
    result = result + d
  end
  return result
end)

--江瑜 10020301 立地成佛
CV.register_skill_value(10020301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  if cm.capi_funcs.get_unit_attr(caster_unit_id, 21) >0 then return 0 end
  local result = 1
  if cm.capi_funcs.get_unit_attr(caster_unit_id, 3) / cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HPMAX) > 0.8 then
    --往人群里冲开盾
    local targets = GET_AREA_TARGET_UNIT(user_id, move_posx, move_posy, cc.AREA_AROUND, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), 0)
    for key in pairs(targets) do
      result = result + 1
    end
  end
  return result
end)

--西门峰 10030201 一剑穿心
CV.register_skill_value(10030201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local x_off,y_off = cm.skill_funcs.get_face_off(attack_face, 1)
  
  if cm.skill_funcs.is_jump_down(user_id, move_posx, move_posy, target_posx, target_posy, 4) then
    return 0
  end
  local d,x,y = cm.skill_funcs.get_jump_dis(user_id, move_posx, move_posy, target_posx, target_posy, 4)
  if d > 1 then --至少跳2格，才能穿刺1格
    for i = 0, d-2 do
      local targets = GET_AREA_TARGET_UNIT(user_id, target_posx + x_off * i, target_posy + y_off * i, cc.AREA_SINGLE, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
      for key in pairs(targets) do
        local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
        result = result + d
      end
    end
  end
  return result
end)

--西门峰 10030301 狂风剑法
CV.register_skill_value(10030301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_TRIANG, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id),attack_face)
  local count = 0
  for key in pairs(targets) do
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    count = count + 1
    result = result + d
  end
  if count <2 then return 0 end
  return result
end)

--贾云长 10040201 日月大刀
CV.register_skill_value(10040201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_SINGLE, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
  for key in pairs(targets) do
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--贾云长 10040401 惊艳一刀
CV.register_skill_value(10040401,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  --checkValue(user_id, caster_unit_id, -1)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_STRAIT, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id),attack_face)
  for key in pairs(targets) do
    --checkValue(user_id, targets[key], -2)
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--关伟 10050201 春秋刀法
CV.register_skill_value(10050201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_SINGLE, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
  for key in pairs(targets) do
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    --可考虑有同类BUFF时 d*0.x
    result = result + d
  end
  return result
end)

--关伟 10050301 长虹守护
CV.register_skill_value(10050301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  if cm.skill_funcs.target_has_buff(caster_unit_id, 10050302) then return 0 end  --同时只守护1人
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_SINGLE, GET_COMBAT_UNIT_CAMP(user_id, caster_unit_id), attack_face)
  for key in pairs(targets) do
    if caster_unit_id ~= targets[key] then --不守护自己
      if (not cm.skill_funcs.target_has_buff(targets[key], 10050301)) then --不重复守护（如果有2个关伟上场）
        if cm.capi_funcs.get_unit_attr(caster_unit_id, 3) / cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HPMAX) > 0.5 then --自身血量50%以上
          local t = cm.capi_funcs.get_unit_attr(targets[key], 3) / cm.capi_funcs.get_unit_attr(targets[key], cc.FIGHT_ATTR_HPMAX)
          if t < 0.8 then --目标血量80%以下
            result = result + 100 / t
          end
        end
      end
    end
  end
  return result
end)

--方云华 10060101 普攻
CV.register_skill_value(10060101,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_SINGLE, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id),attack_face)
  for key in pairs(targets) do
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    if cm.skill_funcs.is_male(user_id, targets[key]) then d = d * 0.8 end
    result = result + d
  end
  return result
end)

--方云华 10060201 梯云纵跃
CV.register_skill_value(10060201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  if move_posx ~= target_posx or move_posy ~= target_posy then  --避免原地跳跃
    result = 100
    if cm.skill_funcs.is_jump_down(user_id, move_posx, move_posy, target_posx, target_posy, 4) then --会跳崖
      return 0
    end
    local d,x,y = cm.skill_funcs.get_jump_dis(user_id, move_posx, move_posy, target_posx, target_posy, 4)
    local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
    
    local targets = GET_AREA_TARGET_UNIT(user_id, x, y, 109, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
    local v = -1
    if cm.skill_funcs.target_has_buff(caster_unit_id, 10060302) then v = 1 end --带栽赃效果时冲进敌群
    for key in pairs(targets) do
      result = result + v
    end
    local result = result + math.random(1,10) --随机
    --checkValue(user_id, caster_unit_id, - result/10)
  end
  return result
end)

--方云华 10060301 栽赃嫁祸
CV.register_skill_value(10060301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  if cm.skill_funcs.target_has_buff(caster_unit_id, 10060302) then return 0 end  --同时只栽赃1人
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target)  then
    local d = cm.capi_funcs.get_unit_attr(target, cc.FIGHT_ATTR_HPMAX) / cm.capi_funcs.get_unit_attr(target, 3) --血越低，越优先
    if cm.skill_funcs.is_male(user_id, target) then d = d * 2 end --优先栽赃男性
    result = result + d
  end
  return result
end)

--齐丽 10070101 剪刀脚
CV.register_skill_value(10070101,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_SINGLE, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id),attack_face)
  for key in pairs(targets) do
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    d = d * (1 + has_mirror_attack(user_id, caster_unit_id, targets[key], move_posx, move_posy, target_posx, target_posy)) --夹击优先
    if GET_LAST_ATTACK_UNIT(caster_unit_id)==targets[key] then  --追击优先
      d = d * 1.2
    else
      d = d * 0.5 * cm.capi_funcs.get_unit_attr(targets[key], cc.FIGHT_ATTR_HPMAX) / cm.capi_funcs.get_unit_attr(targets[key], 3)  --低血优先
    end
    result = result + d
  end
  return result
end)

--齐丽 10070301 耳光布
CV.register_skill_value(10070301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_SINGLE, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
  for key in pairs(targets) do
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--荆棘 10120201 刀光剑影
CV.register_skill_value(10120201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_SINGLE, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
  for key in pairs(targets) do
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--荆棘 10120301 狂刀饮血
CV.register_skill_value(10120301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    local count = 0
  if target >= 0 then
    local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
    local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, 112, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
    for key in pairs(targets) do
      local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
      count = count + 1
      result = result + d
    end
  end
  if count <2 then return 0 end
  return result
end)

--丹青 10150201 泼墨如雨
CV.register_skill_value(10150201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  local count = 0
  if target >= 0 then
    local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
    local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, 109, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
    for key in pairs(targets) do
      local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
      count = count + 1
      result = result + d
    end
  end
  if count <2 then return 0 end
  return result
end)

--丹青 10150401 诗情画意
CV.register_skill_value(10150401,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_SINGLE, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
  for key in pairs(targets) do
    local d = 1
    if cm.skill_funcs.target_has_buff(targets[key], 10150401) then d = 0 end
    result = result + d
  end
  return result
end)

--花翁 10160201 落花有意
CV.register_skill_value(10160201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_SINGLE, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
  for key in pairs(targets) do
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--花翁 10160301 高歌猛进
CV.register_skill_value(10160301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 100
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local x_off,y_off = cm.skill_funcs.get_face_off(attack_face, 1)
  for i = 0,2 do
    local targets = GET_AREA_TARGET_UNIT(user_id, target_posx + x_off * i, target_posy + y_off * i, 109, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
    for key in pairs(targets) do
      local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
      result = result + d
    end
  end
  return result
end)

--橘叟 10170201 满天花雨
CV.register_skill_value(10170201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    local count = 0
  if target >= 0 then
    local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
    local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_ACROSS, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
    for key in pairs(targets) do
      local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
      count = count + 1
      result = result + d
    end
  end
  if count <2 then return 0 end
  return result
end)

--橘叟 10170301 连珠飞子
CV.register_skill_value(10170301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_SINGLE, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
  for key in pairs(targets) do
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--橘叟 10170401 移形换位
CV.register_skill_value(10170401,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 then
    if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then result = result + 1 end
  end
  return result
end)

--神医 10180301 火云神掌
CV.register_skill_value(10180301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--神医 10180401 换血大法
CV.register_skill_value(10180401,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local hp1 = cm.capi_funcs.get_unit_attr(caster_unit_id, 3)
    local maxhp1 = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HPMAX)
    local hp2 = cm.capi_funcs.get_unit_attr(target, 3)
    local maxhp2 = cm.capi_funcs.get_unit_attr(target, cc.FIGHT_ATTR_HPMAX)
    if hp1/maxhp1 < 0.5 then
      result = 100 * hp2 / maxhp2
    end
  end
  return result
end)

--怪医 10190201 狂化邪毒
CV.register_skill_value(10190201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--怪医 10190301 七步追魂
CV.register_skill_value(10190301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    if (not cm.skill_funcs.target_has_buff(target, 10190301)) then
      --checkValue(user_id, target, -1)
      local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
      result = result + d
    end
  end
  return result
end)

--书生 10200201 铁扇封穴
CV.register_skill_value(10200201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--书生 10200301 银钩铁画
CV.register_skill_value(10200301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local unit = -1
  if (move_posx==target_posx and move_posy==target_posy) then --对自身施法
    unit = caster_unit_id
  else
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    if (target_posx==x and target_posy==y) then return 0 end  --对自身原来位置施法
  end
  --移动后对其他单位施法
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >=0 then
    if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then return 0 end
    unit = target
  end
  if unit >=0 then
    local d = 10
    if cm.skill_funcs.target_has_buff(unit, 10200302) then d = 0 end --银钩
    if cm.skill_funcs.target_has_buff(unit, 10200303) then d = 0 end --铁画
    result = result + d
  end
  return result
end)

--醉仙 10210201 醉翁之意
CV.register_skill_value(10210201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 100
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local x_off,y_off = cm.skill_funcs.get_face_off(attack_face, 1)
  for i = 0,1 do
    local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx + x_off * i, target_posy + y_off * i)
    if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
      local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
      result = result + d
    end
  end
  return result
end)

--醉仙 10210301 以酒为箭
CV.register_skill_value(10210301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 then
    local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
    local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_TRIANG, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
    for key in pairs(targets) do
      local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
      result = result + d
    end
  end
  return result
end)

--沈湘芸 10220201 紧急治疗
CV.register_skill_value(10220201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  return single_heal(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
end)

--杨柳山庄护法 36640201 紧急治疗
CV.register_skill_value(36640201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  return single_heal(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
end)

--沈湘芸 10220301 火云剑法
CV.register_skill_value(10220301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  return triangle_dmg(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
end)

--杨柳山庄护法 群攻 34270201
CV.register_skill_value(34270201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  return triangle_dmg(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
end)

--吃 饱以老拳 10240201
CV.register_skill_value(10240201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--吃 大快朵颐 10240301
CV.register_skill_value(10240301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  return 1
end)

--吃 大胃之王 10240401
CV.register_skill_value(10240401,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local hp = cm.capi_funcs.get_unit_attr(caster_unit_id, 3)
  local maxhp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HPMAX)
  if hp/maxhp>0.5 then return 0 end
  if cm.skill_funcs.target_has_buff(caster_unit_id, 10240310) then return 10 end
  if cm.skill_funcs.target_has_buff(caster_unit_id, 10240311) then return 10 end
  if cm.skill_funcs.target_has_buff(caster_unit_id, 10240312) then return 10 end
  if cm.skill_funcs.target_has_buff(caster_unit_id, 10240313) then return 10 end
  if cm.skill_funcs.target_has_buff(caster_unit_id, 10240314) then return 10 end
  return 0
end)

--喝 10250201 自罚一杯
CV.register_skill_value(10250201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local hp = cm.capi_funcs.get_unit_attr(caster_unit_id, 3)
  local maxhp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HPMAX)
  if hp/maxhp<0.8 then return 0 end
  local mp = cm.capi_funcs.get_unit_attr(caster_unit_id, 4)
  local maxmp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_MPMAX)
  if hp/maxhp>0.8 then return 0 end
  return 10
end)

--喝 10250301 敬酒不吃
CV.register_skill_value(10250301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  local count = 0
  if target >= 0 then
    local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
    local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, 109, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
    if #targets < 2 then return 0 end  --至少同时攻击三个单位
    for key in pairs(targets) do
      local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
      result = result + d
      count = count + 1
    end
  end
  if count<2 then return 0 end
  return result
end)

--喝 10250401 不醉不归
CV.register_skill_value(10250401,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local mp1 = cm.capi_funcs.get_unit_attr(caster_unit_id, 4)
    local mmp1 = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_MPMAX)
    local mp2 = cm.capi_funcs.get_unit_attr(target, 4)
    local mmp2 = cm.capi_funcs.get_unit_attr(target, cc.FIGHT_ATTR_MPMAX)
    if mp1/mmp1 > mp2/mmp2 then
      local hp = cm.capi_funcs.get_unit_attr(target, 3)
      if hp * 2 * (mp1/mmp1-mp2/mmp2) > hp*1.0 then result = 100 end  --能击杀的情况下才用此技能
    end
  end
  return result
end)

--嫖 10260201 污言秽语
CV.register_skill_value(10260201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    if cm.skill_funcs.is_male(user_id, target) then d = d * 0.5 end
    result = result + d
  end
  return result
end)

--嫖 10260301 动手动脚
CV.register_skill_value(10260301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    if cm.skill_funcs.is_male(user_id, target) then d = d * 0.5 end
    result = result + d
  end
  return result
end)

--赌 10270201 小赌怡情
CV.register_skill_value(10270201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    result = 10 + d
    local buffs = cm.capi_funcs.get_unit_buff(caster_unit_id)
    local index = 0
    for key in pairs(buffs) do
      if buffs[key] == 10270101 then index = 1 break end
      if buffs[key] == 10270102 then index = 2 break end
      if buffs[key] == 10270103 then index = 3 break end
      if buffs[key] == 10270104 then index = 4 break end
      if buffs[key] == 10270105 then index = 5 break end
      if buffs[key] == 10270106 then index = 6 break end
    end
    local distance = math.abs(target_posx-move_posx) + math.abs(target_posy-move_posy)
    if distance==index then result = result * 2 end
  end
  return result
end)

--赌 10270301 乾坤一掷
CV.register_skill_value(10270301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = 1
  local targets = GET_AREA_TARGET_UNIT(user_id, move_posx, move_posy, cc.AREA_AROUND, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
  local n = 0
  for key in pairs(targets) do
    n = n + 1
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  if n < 2 then return 0 end
  
  local buffs = GET_COMBAT_UNIT_BUFF(caster_unit_id)
  local index = 0 -- 骰数
  for key in pairs(buffs) do
    if buffs[key] == 10270101 then index = 1 break end
    if buffs[key] == 10270102 then index = 2 break end
    if buffs[key] == 10270103 then index = 3 break end
    if buffs[key] == 10270104 then index = 4 break end
    if buffs[key] == 10270105 then index = 5 break end
    if buffs[key] == 10270106 then index = 6 break end
  end
  
  if index ~= n then result = result * 0 end  --如果不触发特效，则不会发动此技能
  return result
end)

--虚真 10300201 降魔棍法
CV.register_skill_value(10300201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--虚真 10300301 护法金刚
CV.register_skill_value(10300301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  if cm.capi_funcs.get_unit_attr(caster_unit_id, 21) > 0 then return 0 end
  return 10
end)

--史燕 10320201 飞燕惊空
CV.register_skill_value(10320201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local x_off,y_off = cm.skill_funcs.get_face_off(attack_face, 1)
  for i = 0,3 do
    local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx + x_off * i, target_posy + y_off * i)
    if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
      local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
      result = result + d
      break
    end
  end
  return result
end)

--史燕 10320301 顺手牵羊
CV.register_skill_value(10320301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    if cm.skill_funcs.target_has_buff(target, 10320301) then return 0 end  --已有失窃标记
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--夏侯非 10330201 回旋刀法
CV.register_skill_value(10330201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = 1
  local targets = GET_AREA_TARGET_UNIT(user_id, move_posx, move_posy, cc.AREA_AROUND, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
  local count = 0
  for key in pairs(targets) do
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    count = count + 1
    result = result + d
  end
  if count < 2 then return 0 end
  return result
end)

--夏侯非 10330301 破防绝刀
CV.register_skill_value(10330301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--巩光杰 10370201 飞镰拘魂
CV.register_skill_value(10370201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local x_off,y_off = cm.skill_funcs.get_face_off(attack_face, 1)
  for i = 0,5 do
    local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx + x_off * i, target_posy + y_off * i)
    if target >= 0 then
      if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
        local maxhp = cm.capi_funcs.get_unit_attr(target, cc.FIGHT_ATTR_HPMAX)
        local d = 10*(maxhp*(i+1)*5/100) --每距离1格造成5%总血量的伤害
        result = result + d
      else
        if target ~= caster_unit_id then
          if result==0 then return 0 end --路径上第一个是己方单位
        end
      end
      break
    end
  end
  return result
end)

--巩光杰 10370301 随身补品
CV.register_skill_value(10370301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  return 1
end)

--唐冠南 10390101 普攻
CV.register_skill_value(10390101,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_SINGLE, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id),attack_face)
  for key in pairs(targets) do
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    d = d * (1 + has_mirror_attack(user_id, caster_unit_id, targets[key], move_posx, move_posy, target_posx, target_posy)) --夹击优先
    if GET_LAST_ATTACK_UNIT(caster_unit_id)==targets[key] then  --追击优先
      d = d * 1.2
    else
      d = d * 0.5 * cm.capi_funcs.get_unit_attr(targets[key], cc.FIGHT_ATTR_HPMAX) / cm.capi_funcs.get_unit_attr(targets[key], 3)  --低血优先
    end
    local buffs = cm.capi_funcs.get_unit_buff(targets[key])
    local index = 0
    for key in pairs(buffs) do
      if buffs[key] == 10390101 then index = 1 break end
      if buffs[key] == 10390102 then index = 2 break end
      if buffs[key] == 10390103 then index = 3 break end
      if buffs[key] == 10390104 then index = 4 break end
      if buffs[key] == 10390105 then index = 5 break end
    end
    result = result + d * (1 + index)
  end
  return result
end)

--唐冠南 10390201 暴雨梨花
CV.register_skill_value(10390201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    local count = 0
  if target >= 0 then
    local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
    local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_FLOWER, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
    for key in pairs(targets) do
      local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
      result = result + d
      count = count + 1
    end
  end
  if count < 2 then return 0 end
  return result
end)

--唐冠南 10390401 毁尸灭迹
CV.register_skill_value(10390401,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
      local buffs = cm.capi_funcs.get_unit_buff(target)
      local index = 0
      for key in pairs(buffs) do
        if buffs[key] == 10390101 then index = 1 break end
        if buffs[key] == 10390102 then index = 2 break end
        if buffs[key] == 10390103 then index = 3 break end
        if buffs[key] == 10390104 then index = 4 break end
        if buffs[key] == 10390105 then index = 5 break end
      end
      local hp = cm.capi_funcs.get_unit_attr(target, cc.FIGHT_ATTR_HP)
      local maxhp = cm.capi_funcs.get_unit_attr(target, cc.FIGHT_ATTR_HPMAX)
      local d = maxhp * 10 * index / 100  --每层造成10%maxHP的伤害
      result = result + d
  end
  return result
end)

--纪玟 快马加鞭 10420201
CV.register_skill_value(10420201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local unit = -1
  if (move_posx==target_posx and move_posy==target_posy) then --对自身施法
    unit = caster_unit_id
  else
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    if (target_posx==x and target_posy==y) then return 0 end  --对自身原来位置施法
  end
  --移动后对其他单位施法
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >=0 then
    if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then return 0 end
    unit = target
  end
  if unit >=0 then
    local d = 10
    if cm.skill_funcs.target_has_buff(unit, 10420201) then d = 0 end
    if cm.skill_funcs.target_has_buff(unit, 10420202) then d = 0 end
    if cm.skill_funcs.target_has_buff(unit, 10420203) then d = 0 end
    result = result + d
  end
  return result
end)

--纪玟 鞭长可及 10420301
CV.register_skill_value(10420301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >=0 then
    result = 10
    if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
      local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
      result = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    end
  end
  return result
end)

--古实 10450201 寓守于攻
CV.register_skill_value(10450201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  if cm.skill_funcs.target_has_buff(caster_unit_id, 10450201) then return 0 end
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--古实 10450301 自愈疗伤
CV.register_skill_value(10450301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local hp = cm.capi_funcs.get_unit_attr(caster_unit_id, 3)
  local maxhp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HPMAX)
  if hp/maxhp <0.8 then result =  maxhp/hp end
  return result
end)

--沈丘山 10480201 金刀驭空
CV.register_skill_value(10480201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local x_off,y_off = cm.skill_funcs.get_face_off(attack_face, 1)
  for i = 1,3 do
    local target = cm.capi_funcs.get_unit_by_pos(user_id, move_posx + x_off * i, move_posy + y_off * i)
    if target >= 0 then
      if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
        local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
        result = result + d
      else
        if target ~= caster_unit_id then
          if result==0 then return 0 end --路径上第一个是己方单位
        end
      end
      break
    end
  end
  return result
end)

--沈丘山 10480401 最后一击
CV.register_skill_value(10480401,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local hp = cm.capi_funcs.get_unit_attr(target, 3)
    local maxhp = cm.capi_funcs.get_unit_attr(target, cc.FIGHT_ATTR_HPMAX)
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    if hp / maxhp < 0.3 then d = d * 10 end
    result = result + d
  end
  return result
end)

--谷月轩 10520201 正气凛人
CV.register_skill_value(10520201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    if cm.skill_funcs.target_has_buff(target, 10520201) then
      return 0
    else
      local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
      result = result + d
      --如果目标没有伤害提升的BUFF则只有一定几率发动该技能
      if not cm.buff_funcs.has_dmgUP_buff(target) then
        if math.random(1,10)<5 then
          result = 0
        end
      end
    end
  end
  return result
end)

--谷月轩 10520301 逍遥身法
CV.register_skill_value(10520301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 100
  if cm.skill_funcs.target_has_buff(caster_unit_id, 10520301) then return 0 end
  result = result - cm.skill_funcs.get_enemy_around(user_id, caster_unit_id, move_posx, move_posy)  --朝人少的位置移动
  return result
end)

--谷月轩 10520401 逆劲护元
CV.register_skill_value(10520401,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local hp = cm.capi_funcs.get_unit_attr(caster_unit_id, 3)
  local maxhp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HPMAX)
  local mp = cm.capi_funcs.get_unit_attr(caster_unit_id, 4)
  local maxmp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_MPMAX)
  if hp/maxhp <0.5 and hp/maxhp < mp/maxmp then
    result = 10
  end
  return result
end)

--蓝婷 10530201 潜龙毒影
CV.register_skill_value(10530201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    if cm.skill_funcs.target_has_buff(caster_unit_id, 10530201) then d = d / 2 end
    result = result + d
  end
  return result
end)

--蓝婷 10530301 附体之毒
CV.register_skill_value(10530301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local unit = -1
  if (move_posx==target_posx and move_posy==target_posy) then --对自身施法
    unit = caster_unit_id
  else
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    if (target_posx==x and target_posy==y) then return 0 end  --对自身原来位置施法
  end
  --移动后对其他单位施法
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >=0 then
    if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then return 0 end
    unit = target
  end
  if unit >=0 then
    if cm.skill_funcs.target_has_buff(unit, 10530301) then return 0 end
    result = 10
  end
  return result
end)

--蓝婷 10530401 魔蛊噬元
CV.register_skill_value(10530401,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local unit = -1
  if (move_posx==target_posx and move_posy==target_posy) then --对自身施法
    unit = caster_unit_id
  else
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    if (target_posx==x and target_posy==y) then return 0 end  --对自身原来位置施法
  end
  --移动后对其他单位施法
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >=0 then
    if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then return 0 end
    unit = target
  end
  if unit >=0 then
    local count = cm.skill_funcs.count_team_has_buff(user_id, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), 10530000)
    local hp = cm.capi_funcs.get_unit_attr(unit, 3)
    local maxhp = cm.capi_funcs.get_unit_attr(unit, cc.FIGHT_ATTR_HPMAX)
    local d = count * maxhp / hp
    result = result + d
  end
  return result
end)

--香儿 10550201 冲冠一怒
CV.register_skill_value(10550201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local unit = -1
  if (move_posx==target_posx and move_posy==target_posy) then --对自身施法
    unit = caster_unit_id
  else
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    if (target_posx==x and target_posy==y) then return 0 end  --对自身原来位置施法
  end
  --移动后对其他单位施法
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >=0 then
    if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then return 0 end
    unit = target
  end
  if unit >=0 then
    local d = 10
    if cm.skill_funcs.target_has_buff(unit, 10550202) then d = 0 end
    if cm.skill_funcs.target_has_buff(unit, 10550203) then d = 0 end
    result = result + d
  end
  return result
end)

--香儿 10550301 魅惑魔音
CV.register_skill_value(10550301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    if cm.skill_funcs.target_has_buff(target, 10550302) then return 0 end
    if cm.skill_funcs.target_has_buff(target, 10550303) then return 0 end
    result = 10
  end
  return result
end)

--高胜 仙人指路 10560201
CV.register_skill_value(10560201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local unit = -1
  if (move_posx==target_posx and move_posy==target_posy) then --对自身施法
    unit = caster_unit_id
  else
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    if (target_posx==x and target_posy==y) then return 0 end  --对自身原来位置施法
  end
  
  --移动后对其他单位施法
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >=0 then
    if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then return 0 end
    unit = target
  end
  if unit >=0 then
    local d = 10
    if cm.skill_funcs.target_has_buff(unit, 10560201) then d = 0 end
    result = result + d
  end
  return result
end)

--高胜 剑啸昆仑 10560301
CV.register_skill_value(10560301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--高胜 人为棋阵 10560401
CV.register_skill_value(10560401,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--叶孤 10570201 追魂夺命
CV.register_skill_value(10570201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--叶孤 10570301 罗汉护体
CV.register_skill_value(10570301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  if cm.skill_funcs.target_has_buff(caster_unit_id, 10570301) then return 0 end
  return 10
end)

--叶孤 10570401 七伤魔拳
CV.register_skill_value(10570401,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  if cm.skill_funcs.target_has_buff(caster_unit_id, 10570401) then return 0 end
  return 10
end)

--萧遥 10580201 飞龙在天
CV.register_skill_value(10580201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local x_off,y_off = cm.skill_funcs.get_face_off(attack_face, 1)
  for i = 0,3 do
    local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx + x_off * i, target_posy + y_off * i)
    if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
      local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
      result = result + d
      break
    end
  end
  return result
end)

--萧遥 10580301 神龙摆尾
CV.register_skill_value(10580301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--商仲仁 10590201 精于算计
CV.register_skill_value(10590201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--商仲仁 10590301 八方风雨
CV.register_skill_value(10590301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = 1
  local targets = GET_AREA_TARGET_UNIT(user_id, move_posx, move_posy, cc.AREA_AROUND, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
  local count = 0
  for key in pairs(targets) do
    local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
    count = count + 1
  end
  if count<2 then return 0 end
  return result
end)

--任剑南 10600201 千锤百炼
CV.register_skill_value(10600201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local unit = -1
  if (move_posx==target_posx and move_posy==target_posy) then --对自身施法
    unit = caster_unit_id
  else
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    if (target_posx==x and target_posy==y) then return 0 end  --对自身原来位置施法
  end
  --移动后对其他单位施法
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >=0 then
    if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then return 0 end
    unit = target
  end
  if unit >=0 then
    local d = 10
    if cm.skill_funcs.target_has_buff(unit, 10600201) then d = 0 end
    if cm.skill_funcs.target_has_buff(unit, 10600206) then d = 0 end
    local weapon = cm.capi_funcs.get_atk_type(unit)
    if weapon == 2 then d = d * 2 end --武器类型为剑
    result = result + d
  end
  return result
end)

--任剑南 10600301 万剑齐发
CV.register_skill_value(10600301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    local count = 0
  if target >= 0 then
    local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
    local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, 109, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
    for key in pairs(targets) do
      local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
      result = result + d
      count = count + 1
    end
  end
  if count < 2 then return 0 end
  return result
end)

--任清璇 10630201 花不堪折
CV.register_skill_value(10630201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    if cm.skill_funcs.target_has_buff(target, 10630201) then return 0 end
    result = 10
  end
  return result
end)

--任清璇 10630401 宁神幽香
CV.register_skill_value(10630401,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local unit = -1
  if (move_posx==target_posx and move_posy==target_posy) then --对自身施法
    unit = caster_unit_id
  else
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    if (target_posx==x and target_posy==y) then return 0 end  --对自身原来位置施法
  end
  --移动后对其他单位施法
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >=0 then
    if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then return 0 end
    unit = target
  end
  if unit >=0 then
    if cm.buff_funcs.has_bad_buff(unit) then
      result = result + 100
    end
  end
  return result
end)

--傅剑寒 10640201 项庄舞剑 
CV.register_skill_value(10640201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 then
    local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
    local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, 110, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
    for key in pairs(targets) do
      local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
      result = result + d
    end
  end
  return result
end)

--傅剑寒 10640301 决一雌雄 
CV.register_skill_value(10640301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    local hard1 = cm.capi_funcs.get_unit_attr(caster_unit_id, 6)
    local hard2 = cm.capi_funcs.get_unit_attr(target, 6)
    if hard1>hard2 then d = d * 2
    end
    result = result + d
  end
  return result
end)

--傅剑寒 10640401 破釜沉舟 
CV.register_skill_value(10640401,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  if cm.skill_funcs.target_has_buff(caster_unit_id, 10640401) then return 0 end
  return 10
end)

--万凯 10680201 挥臂猛击
CV.register_skill_value(10680201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  --checkValue(user_id, caster_unit_id, -1)
  local result = 0
  local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then
    local d = dmg_estimate(user_id, caster_unit_id, target, target_range_param, attack_face) * damage_range_param / 10000
    result = result + d
  end
  return result
end)

--万凯 10680301 兽血沸腾
CV.register_skill_value(10680301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  if cm.skill_funcs.target_has_buff(caster_unit_id, 10680301) then return 0 end
  return 10
end)

--丁冲 10310201 狂风快剑
CV.register_skill_value(10310201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local target = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target >= 0 then
    --checkValue(user_id, target, -1)
    local attack_face = cm.skill_funcs.get_attack_face_by_pos(move_posx, move_posy, target_posx, target_posy)
    local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_TRIANG, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
    for key in pairs(targets) do
      local d = dmg_estimate(user_id, caster_unit_id, targets[key], target_range_param, attack_face) * damage_range_param / 10000
      result = result + d
    end
  end
  return result
end)

--丁冲 10310301 北冥神功
CV.register_skill_value(10310301,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = 0
  local mp1 = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_MP)
  local maxmp1 = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_MPMAX)
  if mp1/maxmp1>=0.8 then return 0 end
  local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if target_unit >= 0 and cm.skill_funcs.is_enemy(user_id, caster_unit_id, target_unit) then
    local mp2 = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_MP)
    local maxmp2 = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_MPMAX)
    if mp2/maxmp2 >=0.2 then result = mp2/maxmp2 end
  end
  return result
end)



---------------------------------------------------------------------------------------
--以下为剧情战怪物技能
---------------------------------------------------------------------------------------

--叶枫 20170101 普攻
CV.register_skill_value(20170101,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = skill_single(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  
  if level_id==2000017 then --level:瓮中捉鳖
    local ai_check = cm.level_funcs.get_mark(caster_unit_id)
    if ai_check == 3 then --优先攻击10,0位置
      if target_posx==10 and target_posy == 0 then
        return result * 100
      else
        return 0
      end
    end  
    
    if ai_check == 4 then --优先攻击10,1位置
      if target_posx==10 and target_posy == 1 then
        return result * 100
      else
        return 0
      end
    end
    
  end
  return result
end)

--叶枫 20170201 普攻
CV.register_skill_value(20170201,
function(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  local result = skill_single(user_id, level_id, skill_id, caster_unit_id, move_posx, move_posy, target_posx, target_posy, target_range_param, damage_range_param)
  
  if level_id==2000017 then --level:瓮中捉鳖
    local ai_check = cm.level_funcs.get_mark(caster_unit_id)
    if ai_check == 3 then --优先攻击10,0位置
      if target_posx==10 and target_posy == 0 then
        return result * 100
      else
        return 0
      end
    end  
    
    if ai_check == 4 then --优先攻击10,1位置
      if target_posx==10 and target_posy == 1 then
        return result * 100
      else
        return 0
      end
    end
    
  end
  return result
end)




return CV














