--[[
function combatCounter(max_number)
  local i = 0
  return function()
    i=i+1
    if max_number then
      if i > max_number then i = 1 end
    end
    return i
  end
end
]]--

local cc = require("combat_const")
local cm = require("combat_module")

local CK = {}
CK.skill_function_map = {}
function CK.register_skill_function(skill_id, func)
  CK.skill_function_map[skill_id] = func
end
function CK.get_skill_function(skill_id)
    return CK.skill_function_map[skill_id]
end

--空SKILL模板
CK.register_skill_function(1000000,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  local target_unit = cm.capi_funcs.get_unit_by_pos(user_id,target_posx,target_posy)
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then --计算伤害时调用
    
  end
  if execute_type == cc.SKILL_SCRIPT_CALL_SKILL_DONE then --技能处理完成后调用
    
  end
  return result
end)

---------------------------------------------------------------------------------------------------------------------------

--仙音 菩提清心 10010201
CK.register_skill_function(10010201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then cm.buff_funcs.clear_control(user_id, target_unit) end
  end
  return 0
end)

--江瑜 立地成佛 10020301
CK.register_skill_function(10020301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local maxhp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HPMAX)
    cm.capi_funcs.set_unit_attr(user_id, caster_unit_id, cc.FIGHT_ATTR_QIFANG, maxhp)--气防设为与最大HP相同
  end
  return 0
end)

--关伟 长虹守护
CK.register_skill_function(10050301, 
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    cm.capi_funcs.add_unit_buff(user_id,caster_unit_id,caster_unit_id,10050302)
  end
  return 0
end)

--方云华 普攻 10060101
CK.register_skill_function(10060101,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then --计算伤害时调用
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      if cm.skill_funcs.is_male(user_id,target_unit) then return 100 end
      return 150 --对女性造成额外50%伤害
    end
  end
  return 0
end)

--齐丽 剪刀 10070101
CK.register_skill_function(10070101,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    local t = math.random(1,3)
    if t==1 then return 50 end
    if t==3 then return 150 end
  end
  return 0
end)

--齐丽 石头 10070201
CK.register_skill_function(10070201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    local t = math.random(1,3)
    if t==1 then return 50 end
    if t==3 then return 150 end
  end
  return 0
end)

--齐丽 布 10070301
CK.register_skill_function(10070301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    local t = math.random(1,3)
    if t==1 then return 50 end
    if t==3 then return 200 end
  else
    cm.level_funcs.set_mark(user_id,caster_unit_id,0) --清空标记
    local chain = math.random(0,2)
    if chain < 1 then --30%连击率
      local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
      if target_unit>=0 then
        cm.capi_funcs.unit_cast_skill(user_id,caster_unit_id,target_posx,target_posy,10070302)
        cm.level_funcs.add_mark(user_id,caster_unit_id,1) --标记+1
      end
    end
  end
  return 0
end)

--齐丽 布-后续 10070302
CK.register_skill_function(10070302,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local t = cm.level_funcs.get_mark(caster_unit_id)
    if t < 3 then --连击最多2次
      local chain = math.random(0,2)
      if chain < 1 then
        local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
        if target_unit>=0 then
          cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, target_posx, target_posy, 10070302)
          cm.level_funcs.add_mark(user_id,caster_unit_id,1) --标记+1
        end
      end
    end
  end
  return 0
end)

--荆棘 狂刀饮血 10120301
CK.register_skill_function(10120301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    local maxhp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HPMAX)
    return 200 -- math.floor(maxhp/10*2)--额外伤害等于技能支付的血量的2倍
  else
    local hp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HP)
    local maxhp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HPMAX)
    local index = 10 - math.floor(10 * hp / maxhp)
    if index>0 then
      cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, caster_unit_id, 10120401+index)
    else
      cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, caster_unit_id, 10120412)
    end
  end
  return 0
end)

--花翁 落花有意 10160201
CK.register_skill_function(10160201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_SKILL_DONE then --技能处理完成后调用，但是不包括闪避的情况
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      if math.random(1,10)> 5 then
        cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10160201)
      end
    end
  end
  return 0
end)

--神医 妙手回春 10180201
CK.register_skill_function(10180201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local maxhp = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_HPMAX)
      return 100 + 100 * (maxhp * 5 / 100) / 25   --25+5%血
    end
  end
  return 0
end)

--神医 换血大法 10180401
CK.register_skill_function(10180401,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id,target_posx,target_posy)
    if target_unit>=0 then
      local hp1 = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HP)
      local maxhp1 = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HPMAX)
      local hp2 = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_HP)
      local maxhp2 = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_HPMAX)
      cm.capi_funcs.set_unit_attr(user_id, caster_unit_id, cc.FIGHT_ATTR_HP, 1 + maxhp1 * hp2 / maxhp2)
      cm.capi_funcs.set_unit_attr(user_id, target_unit, cc.FIGHT_ATTR_HP, 1 + maxhp2 * hp1 / maxhp1)
    end
  end
  return 0
end)

--书生 银钩铁画 10200301
CK.register_skill_function(10200301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
  local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      if cm.skill_funcs.is_male(user_id,target_unit) then
        cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10200303)--铁画 男
      else
        cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10200302)--银钩 女
      end
    end
  end
  return 0
end)

--沈湘云 紧急治疗 10220201
CK.register_skill_function(10220201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local maxhp = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_HPMAX)
      return 100 + 100 * (maxhp * 10 / 100) / 50  --50+10%血
    end
  end
  return 0
end)

--吃 饱以老拳 10240201
CK.register_skill_function(10240201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    local buffs = cm.capi_funcs.get_unit_buff(caster_unit_id)
    local index = 0
    for key in pairs(buffs) do
      if buffs[key] == 10240310 then index = 1 break end
      if buffs[key] == 10240311 then index = 2 break end
      if buffs[key] == 10240312 then index = 3 break end
      if buffs[key] == 10240313 then index = 4 break end
      if buffs[key] == 10240314 then index = 4 break end
    end
    return 100+ index*50
  end
  return 0
end)

--吃 大快朵颐 10240301
CK.register_skill_function(10240301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then --技能处理完成后调用
    local buffs = cm.capi_funcs.get_unit_buff(caster_unit_id)
    local index = 0
    for key in pairs(buffs) do
      if buffs[key] == 10240310 then index = 1 break end
      if buffs[key] == 10240311 then index = 2 break end
      if buffs[key] == 10240312 then index = 3 break end
      if buffs[key] == 10240313 then index = 4 break end
      if buffs[key] == 10240314 then index = 4 break end
    end
    cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, caster_unit_id, 10240310+index)
  end
  return 0
end)

--吃 大胃之王 10240401
CK.register_skill_function(10240401,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then --技能处理完成后调用
    local buffs = cm.capi_funcs.get_unit_buff(caster_unit_id)
    local index = 0
    for key in pairs(buffs) do
      if buffs[key] == 10240314 then index = 5 break  end
      if buffs[key] == 10240313 then index = 4 break  end
      if buffs[key] == 10240312 then index = 3 break  end
      if buffs[key] == 10240311 then index = 2 break  end
      if buffs[key] == 10240310 then index = 1 break  end
    end
    cm.capi_funcs.del_unit_buff(user_id, caster_unit_id, 10240310)
    cm.capi_funcs.del_unit_buff(user_id, caster_unit_id, 10240311)
    cm.capi_funcs.del_unit_buff(user_id, caster_unit_id, 10240312)
    cm.capi_funcs.del_unit_buff(user_id, caster_unit_id, 10240313)
    cm.capi_funcs.del_unit_buff(user_id, caster_unit_id, 10240314)
    local hp = cm.capi_funcs.get_unit_attr(caster_unit_id, 3)
    local maxhp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HPMAX)
    cm.capi_funcs.set_unit_attr(user_id, caster_unit_id, 3, hp + maxhp * index * 15 / 100) --每层造成15%maxHP的回复
  end
  return 0
end)

--喝 不醉不归 10250401
CK.register_skill_function(10250401,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local mp1 = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_MP)
      local mmp1 = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_MPMAX)
      local mp2 = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_MP)
      local mmp2 = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_MPMAX)
      if mp1/mmp1 > mp2/mmp2 then
        cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10250401) --播放特效
        local hp = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_HP)
        cm.capi_funcs.set_unit_attr(user_id, target_unit, cc.FIGHT_ATTR_HP, hp - hp * 2 * (mp1/mmp1-mp2/mmp2)) --当前HP的2*剩余内力比例之差
      else
        cm.capi_funcs.set_unit_attr(user_id, caster_unit_id, cc.FIGHT_ATTR_MP, mp1*2/3)
        cm.capi_funcs.set_unit_attr(user_id, target_unit, cc.FIGHT_ATTR_MP, mp2*2/3)
      end
    end
  end
  return result
end)

--嫖 污言秽语 10260201
CK.register_skill_function(10260201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then --技能处理完成后调用
    if target_unit>=0 then
      cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 151101)--降低暴击
      if not cm.skill_funcs.is_male(user_id,target_unit) then
        cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 61101)--降低耳功 女
      end
    end
  end
  return 0
end)

--嫖 动手动脚 10260301
CK.register_skill_function(10260301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_SKILL_DONE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      if not cm.skill_funcs.is_male(user_id,target_unit) then
        if math.random(1,2)==1 then
          cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 1102)--禁止移动
        else
          cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 1302)--禁止发招
        end
      end
    end
  end
  return 0
end)

--嫖 见猎心喜 10260401
CK.register_skill_function(10260401,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then --技能处理完成后调用
    local enemy_camp = cm.skill_funcs.get_enemy_camp(user_id,caster_unit_id)
    local t = cm.team_funcs.get_gender_number(user_id, enemy_camp, cc.GENDER_FEMALE)
    if t>0 then
      cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, caster_unit_id, 190100+t)--敌方每名女性提升10%伤害
    end
  end
  return 0
end)

--赌 命运游戏-随机结果 10270401
CK.register_skill_function(10270401,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then --技能处理完成后调用
    local dice = math.random(1,6) --随机加一个BUFF用来表示随机结果
    local buffid = 10270100 + dice
    cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, caster_unit_id, buffid)
    cm.level_funcs.set_mark(user_id, caster_unit_id, 0) --清除乾坤一掷的标记
  end
  return 0
end)

--赌 小赌怡情 10270201
CK.register_skill_function(10270201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then  --伤害处理阶段
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local result = 100
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
      --属性比较
      local attr1 = cm.capi_funcs.get_unit_attr(caster_unit_id, 5+index)
      local attr2 = cm.capi_funcs.get_unit_attr(target_unit, 5+index)
      if attr1> attr2 then result = 120 end
      if attr1< attr2 then result = 80 end
      --距离匹配
      local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
      local distance = math.abs(target_posx-x) + math.abs(target_posy-y)
      if distance==index then result = result + 50 end
      return result
    end
  end
  return 0
end)

--赌 乾坤一掷 10270301
CK.register_skill_function(10270301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then --技能处理完成后调用
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10270302)
  end
  return 0
end)

--赌 乾坤一掷-范围伤害 10270302
CK.register_skill_function(10270302,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then --计算伤害时调用
    local result= 100
    local buffs = cm.capi_funcs.get_unit_buff(caster_unit_id)
    local index = 0 -- 骰数
    for key in pairs(buffs) do
      if buffs[key] == 10270101 then index = 1 break end
      if buffs[key] == 10270102 then index = 2 break end
      if buffs[key] == 10270103 then index = 3 break end
      if buffs[key] == 10270104 then index = 4 break end
      if buffs[key] == 10270105 then index = 5 break end
      if buffs[key] == 10270106 then index = 6 break end
    end
    local t = cm.level_funcs.get_mark(caster_unit_id)--人数
    if index > t then result = 150  end
    if index== t then result = 200  end
    return result
  end
  return 0
end)

--虚真 降魔棍法 10300201
CK.register_skill_function(10300201,
function (user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then --计算伤害时调用
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local good_or_evil = cm.capi_funcs.get_unit_extra_attr(user_id, target_unit, cc.EXTRAATTR_GOOD)
      if good_or_evil<0 then result = 200 end -- 对邪恶方造成2倍伤害
    end
  end
  return result
end)

--虚真 护法金刚 10300301
CK.register_skill_function(10300301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then  --技能处理完成后调用
    local hp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HP)
    cm.capi_funcs.set_unit_attr(user_id, caster_unit_id, cc.FIGHT_ATTR_QIFANG, hp) --气防设为与当前HP相同
  end
  return 0
end)

--史燕 顺手牵羊 10320301
CK.register_skill_function(10320301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if execute_type == cc.SKILL_SCRIPT_CALL_SKILL_DONE then --技能处理完成后调用
    if target_unit > 0 then
      if cm.level_funcs.is_PVP(caster_unit_id, target_unit) then
        --随机偷取目标基础属性
        local index = math.random(6,11)
        local value1 = cm.capi_funcs.get_unit_attr(caster_unit_id, index)
        local value2 = cm.capi_funcs.get_unit_attr(target_unit, index)
        local t = math.max(1,math.floor(value2/10)) --偷取10%当前值，至少1点
        if (value2-t)>0 then
          cm.capi_funcs.set_unit_attr(user_id, caster_unit_id, index, value1+t)
          cm.capi_funcs.set_unit_attr(user_id, target_unit, index, value2-t)
        end
      else
        if cm.skill_funcs.target_has_buff(target_unit, 10320301) then
          --已经失窃的不处理
        else
          cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10320301) --标记为失窃
          cm.capi_funcs.add_drop(user_id, 194, cc.DROP_INSTANTLY)
        end
      end
    end
  end
  return result
end)

--巩光杰 随身补品 10370301
CK.register_skill_function(10370301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then --技能处理完成后调用
    local bufftype = math.random(1,6)
    local buffid = 10000 * bufftype + 101
    cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, caster_unit_id, buffid)
  end
  return result
end)

--唐冠南 普攻 叠加毒效 10390101
CK.register_skill_function(10390101,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_SKILL_DONE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local buffs = cm.capi_funcs.get_unit_buff(target_unit)
      local index = 0
      for key in pairs(buffs) do
        if buffs[key] == 10390101 then index = 1 break end
        if buffs[key] == 10390102 then index = 2 break end
        if buffs[key] == 10390103 then index = 3 break end
        if buffs[key] == 10390104 then index = 4 break end
      end
      cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10390101+index)
    end
  end
  return 0
end)

--唐冠南 暴雨梨花 叠加毒效 10390201
CK.register_skill_function(10390201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_SKILL_DONE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local buffs = cm.capi_funcs.get_unit_buff(target_unit)
      local index = 0
      for key in pairs(buffs) do
        if buffs[key] == 10390101 then index = 1 break end
        if buffs[key] == 10390102 then index = 2 break end
        if buffs[key] == 10390103 then index = 3 break end
        if buffs[key] == 10390104 then index = 4 break end
      end
      cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10390101+index)
    end
  end
  return 0
end)

--唐冠南 毁尸灭迹 10390401
CK.register_skill_function(10390401,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local buffs = cm.capi_funcs.get_unit_buff(target_unit)
      local index = 0
      for key in pairs(buffs) do
        if buffs[key] == 10390101 then index = 1 cm.capi_funcs.del_unit_buff(user_id, target_unit, 10390101) break end
        if buffs[key] == 10390102 then index = 2 cm.capi_funcs.del_unit_buff(user_id, target_unit, 10390102) break end
        if buffs[key] == 10390103 then index = 3 cm.capi_funcs.del_unit_buff(user_id, target_unit, 10390103) break end
        if buffs[key] == 10390104 then index = 4 cm.capi_funcs.del_unit_buff(user_id, target_unit, 10390104) break end
        if buffs[key] == 10390105 then index = 5 cm.capi_funcs.del_unit_buff(user_id, target_unit, 10390105) break end
      end
      local hp = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_HP)
      local maxhp = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_HPMAX)
      cm.capi_funcs.set_unit_attr(user_id, target_unit, cc.FIGHT_ATTR_HP, hp - maxhp * 10 * index / 100) --每层造成10%maxHP的伤害
    end
  end
  return result
end)

--沈丘山 最后一击 10480401
CK.register_skill_function(10480401,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_SKILL_DONE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      if cm.unit_funcs.hp_less_than_percent(target_unit, 20) then
        cm.capi_funcs.set_unit_attr(user_id, target_unit, cc.FIGHT_ATTR_HP, 0)
      end
    end
  end
  return 0
end)

--谷月轩 逆劲护元 10520401
CK.register_skill_function(10520401,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local hp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HP)
    local maxhp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HPMAX)
    local mp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_MP)
    local maxmp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_MPMAX)
    cm.capi_funcs.set_unit_attr(user_id, caster_unit_id, cc.FIGHT_ATTR_HP, maxhp * mp/maxmp)
    cm.capi_funcs.set_unit_attr(user_id, caster_unit_id, cc.FIGHT_ATTR_MP, maxmp * hp/maxhp)
  end
  return result
end)

--蓝婷 魔蛊噬元 10530401
CK.register_skill_function(10530401,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local enemycamp = cm.skill_funcs.get_enemy_camp(user_id, target_unit)
      local enemy = cm.team_funcs.get_team(user_id, enemycamp)
      for i in pairs(enemy) do
        if enemy[i]>=0 then
          if cm.skill_funcs.target_has_buff(enemy[i], 10530000) then
            local hp1 = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_HP)
            local maxhp = cm.capi_funcs.get_unit_attr(enemy[i], cc.FIGHT_ATTR_HPMAX)
            cm.capi_funcs.set_unit_attr(user_id, target_unit, cc.FIGHT_ATTR_HP, hp1 + 15 * maxhp / 100)
            local hp2 = cm.capi_funcs.get_unit_attr(enemy[i], cc.FIGHT_ATTR_HP)
            cm.capi_funcs.set_unit_attr(user_id, enemy[i], cc.FIGHT_ATTR_HP, hp2 - 15 * maxhp / 100)
            cm.buff_funcs.clear_poison(user_id, target_unit)
          end
        end
      end
    end
  end
  return 0
end)

--香儿 冲冠一怒 10550201
CK.register_skill_function(10550201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then --技能处理完成后调用
  local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      if cm.skill_funcs.is_male(user_id,target_unit) then
        cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10550203)--男
      else
        cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10550202)--女
      end
    end
  end
  return 0
end)

--香儿 魅惑魔音 10550301
CK.register_skill_function(10550301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then --技能处理完成后调用
  local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      if cm.skill_funcs.is_male(user_id,target_unit) then
        cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10550302)--男
      else
        cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10550303)--女
      end
    end
  end
  return 0
end)

--叶孤 普攻-七伤魔拳 10570101
CK.register_skill_function(10570101,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_SKILL_DONE then
    if cm.skill_funcs.target_has_buff(caster_unit_id, 10570402) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, caster_unit_id, 10570403) return 0 end
    if cm.skill_funcs.target_has_buff(caster_unit_id, 10570403) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, caster_unit_id, 10570404) return 0 end
    if cm.skill_funcs.target_has_buff(caster_unit_id, 10570404) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, caster_unit_id, 10570405) return 0 end
    if cm.skill_funcs.target_has_buff(caster_unit_id, 10570405) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, caster_unit_id, 10570406) return 0 end
    if cm.skill_funcs.target_has_buff(caster_unit_id, 10570406) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, caster_unit_id, 10570407) return 0 end
    if cm.skill_funcs.target_has_buff(caster_unit_id, 10570407) then
      local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
      if target_unit>=0 then
        cm.capi_funcs.set_unit_attr(user_id, target_unit, cc.FIGHT_ATTR_HP, 0)
      end
      cm.capi_funcs.set_unit_attr(user_id, caster_unit_id, cc.FIGHT_ATTR_HP, 0)
      return 0
    end
  end
  return 0
end)

--商仲仁 精于算计 10590201
CK.register_skill_function(10590201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_SKILL_DONE then  --技能处理完成后调用
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
      local dx,dy = cm.skill_funcs.get_dx_dy(x, y, target_posx, target_posy)
      if (dx== 1 and dy== 0) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 141102) end --兑泽 闪避-20%
      if (dx== 1 and dy==-1) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 151102) end --坤土 暴击-20%
      if (dx== 1 and dy== 1) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 161102) end --乾天 坚韧-20%
      if (dx==-1 and dy== 0) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 171102) end --震木 精准-20%
      if (dx==-1 and dy==-1) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 181102) end --巽风 格挡-20%
      if (dx==-1 and dy== 1) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 131102) end --艮山 命中-20%
      if (dx== 0 and dy== 1) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 111102) end --坎水 攻击-20%
      if (dx== 0 and dy==-1) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 121102) end --离火 防御-20%
    end
  end
  return 0
end)

--任剑南 普攻-谁与争锋 10600101
CK.register_skill_function(10600101,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_SKILL_DONE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local weapon = cm.capi_funcs.get_atk_type(target_unit)
      if weapon == 2 then --武器类型为剑
        cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10600401)
      end
    end
  end
  return 0
end)

--任剑南 千锤百炼 10600201
CK.register_skill_function(10600201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local weapon = cm.capi_funcs.get_atk_type(target_unit)
      if weapon == 2 then --武器类型为剑
        if cm.skill_funcs.target_has_buff(target_unit, 10600202) then
          cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10600203)
          return 0
        end
        if cm.skill_funcs.target_has_buff(target_unit, 10600203) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10600204) return 0 end
        if cm.skill_funcs.target_has_buff(target_unit, 10600204) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10600205) return 0 end
        if cm.skill_funcs.target_has_buff(target_unit, 10600205) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10600206) return 0 end
        if cm.skill_funcs.target_has_buff(target_unit, 10600206) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10600206) return 0 end
        cm.capi_funcs.add_unit_buff(user_id, target_unit, target_unit, 10600202)
      else
        cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10600201)
      end
    end
  end
  return 0
end)

--任剑南 万剑齐发-谁与争锋 10600301
CK.register_skill_function(10600301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_SKILL_DONE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local weapon = cm.capi_funcs.get_atk_type(target_unit)
      if weapon == 2 then --武器类型为剑
        cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10600401)
      end
    end
  end
  return 0
end)

--傅剑寒 决一雌雄 10640301
CK.register_skill_function(10640301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then --计算伤害时调用
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local hard1 = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_STR)
      local hard2 = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_STR)
      if hard1>hard2 then
        result = 200
      end
    end
  end
  return result
end)

--丁冲 北冥神功 10310301
CK.register_skill_function(10310301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local mp1 = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_MP)
      local maxmp1 = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_MPMAX)
      local mp2 = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_MP)
      local maxmp2 = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_MPMAX)
      local t = maxmp2 * 30 / 100 --吸取30%的蓝
      cm.capi_funcs.set_unit_attr(user_id, caster_unit_id, cc.FIGHT_ATTR_MP, mp1 + t)  
      cm.capi_funcs.set_unit_attr(user_id, target_unit, cc.FIGHT_ATTR_MP, mp2 - t)
    end
  end
  return 0
end)

--纪玟 10420201 快马加鞭
CK.register_skill_function(10420201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local hp = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_HP)
      local maxhp = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_HPMAX)
      local t = hp / maxhp
      local buff_id = 10420201
      if t < 0.8 then buff_id = 10420202 end
      if t < 0.4 then buff_id = 10420203 end
      if target_unit == caster_unit_id then buff_id = buff_id + 1 end
      cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, buff_id)
    end
  end
  return 0
end)

--赛飞鸿 10500401 万夫莫敌
CK.register_skill_function(10500401,
function (user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    local enemyNum = cm.skill_funcs.get_enemy_around(user_id, caster_unit_id, x, y)
    if enemyNum>0 then
      cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, caster_unit_id, 190101 - 1 + enemyNum)
    end
  end
  return 0
end)

--赵惊风 10510401 快捷如风
CK.register_skill_function(10510401,
function (user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    cm.capi_funcs.set_unit_attr(user_id, caster_unit_id, 30, 99999)
  end
  return 0
end)

--杨云 10690301 酒壮人胆
CK.register_skill_function(10690301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local buffs = cm.capi_funcs.get_unit_buff(caster_unit_id)
    local index = 0
    for key in pairs(buffs) do
      if buffs[key] == 10690401 then index = 1 break end
      if buffs[key] == 10690402 then index = 2 break end
      if buffs[key] == 10690403 then index = 3 break end
      if buffs[key] == 10690404 then index = 4 break end
      if buffs[key] == 10690405 then index = 5 break end
      if buffs[key] == 10690406 then index = 6 break end
      if buffs[key] == 10690407 then index = 7 break end
      if buffs[key] == 10690408 then index = 8 break end
      if buffs[key] == 10690409 then index = 9 break end
    end
    if index > 0 then
      cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, caster_unit_id, 10690300+index)
    end
  end
  return 0
end)

--龙王 双龙断脉
CK.register_skill_function(35180201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_id = cm.capi_funcs.get_unit_by_pos(user_id,target_posx,target_posy)
    if target_id>=0 then
      local mp = cm.capi_funcs.get_unit_attr(target_id,cc.FIGHT_ATTR_MP)
      cm.capi_funcs.set_unit_attr(user_id,target_id,cc.FIGHT_ATTR_MP,mp/2)
    end
  end
  return 0
end)






----------------------------------------------------------------------------------------------------------------------------------------
--以下为范围类技能处理
----------------------------------------------------------------------------------------------------------------------------------------

--西门峰 狂风剑法 10030301
CK.register_skill_function(10030301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10030302)
  end
  return 0
end)

--醉仙 以酒为剑 10210301
CK.register_skill_function(10210301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10210302)
  end
  return 0
end)

--醉仙 以酒为箭
CK.register_skill_function(10210302,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then --计算伤害时调用
    local buffs = cm.capi_funcs.get_unit_buff(caster_unit_id)
    local index = 0
    for key in pairs(buffs) do
      if buffs[key] == 10210214 then index = 5 break end
      if buffs[key] == 10210213 then index = 4 break end
      if buffs[key] == 10210212 then index = 3 break end
      if buffs[key] == 10210211 then index = 2 break end
      if buffs[key] == 10210210 then index = 1 break end
    end
    result = 100 + index * 30 --每层酒意增加30%伤害
  end
  return result
end)

--沈湘芸 火云剑法 10220301
CK.register_skill_function(10220301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10220302)
  end
  return 0
end)

--腾蛇 毒云剑法 35160201
CK.register_skill_function(35160201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 35160202)
  end
  return 0
end)

--喝 敬酒不吃 10250301
CK.register_skill_function(10250301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10250302)
  end
  return 0
end)

--雷震天 飞天震地 10490201
CK.register_skill_function(10490201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    local face = math.random(1,4)
    local mark = cm.level_funcs.get_mark(caster_unit_id)
    --cm.log_info("<skill_10490301> unit mark: %d", mark)
    if mark > 0 then
      if mark == 4 then
        face = 1
      else
        face = mark + 1
      end
    end
    cm.level_funcs.set_mark(user_id,caster_unit_id,face)
    local xoff,yoff = cm.skill_funcs.get_face_off(face,1)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x+xoff, y+yoff, 10490202)
  end
  return 0
end)

--香儿 沉鱼落雁 10550401
CK.register_skill_function(10550401,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10550402)
  end
  return 0
end)

--高胜 人为棋阵 10560401
CK.register_skill_function(10560401,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    local x_off,y_off = cm.skill_funcs.get_face_pos(user_id, caster_unit_id, 1)
    
    local noTarget = true
    for i=1,3 do
      local target = cm.capi_funcs.get_unit_by_pos(user_id, x+x_off*i, y+y_off*i)
      if target>=0 then
        noTarget = false
        if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target) then --车
          local x1,y1 = cm.capi_funcs.get_unit_pos(target)
          cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x1, y1, 10560403)
        end
        local target2 = cm.capi_funcs.get_unit_by_pos(user_id, x+x_off*i*2, y+y_off*i*2)
        if target2>0 then
          if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target2) then --炮
            local x1,y1 = cm.capi_funcs.get_unit_pos(target2)
            cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x1, y1, 10560404)
          end
        end
        break
      end
    end
    if noTarget then
      cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, target_posx, target_posy, 10560402)
    end
  end
  return 0
end)

--丁冲 狂风快剑 10310201
CK.register_skill_function(10310201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10310202)
  end
  return 0
end)

--杨柳山庄护法 锥形群攻 34270201
CK.register_skill_function(34270201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 34270202)
  end
  return 0
end)


----------------------------------------------------------------------------------------------------------------------------------------
--以下为位移类技能处理
----------------------------------------------------------------------------------------------------------------------------------------
--  COMBAT_UNIT_MOVE_NORMAL = 1   --普通移动，正常寻路的
--  COMBAT_UNIT_MOVE_TELEPORT = 2 --传送，可以是任意位置
--  COMBAT_UNIT_MOVE_JUMP = 3   --跳跃移动
--  COMBAT_UNIT_MOVE_RUSH = 4   --冲锋移动
local FPS15_0 = 0
local FPS15_1 = 1
local FPS15_2 = 2
local FPS15_3 = 3
local FPS15_4 = 4
local FPS15_5 = 5
local FPS15_6 = 6
local FPS15_7 = 7
local FPS15_8 = 8
local FPS15_9 = 9
local FPS15_10 = 10
local FPS15_11 = 11
local FPS15_12 = 12
local FPS15_13 = 13
local FPS15_14 = 14
local FPS15_15 = 15
local FPS15_16 = 16

--西门峰 一剑穿心 10030201
CK.register_skill_function(10030201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1004, target_posx, target_posy, cc.MOVE_JUMP, FPS15_1)
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10030202)
  end
  return 0
end)

--方云华 梯云纵 10060201 落地动作
CK.register_skill_function(10060201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1004, target_posx, target_posy, cc.MOVE_JUMP, FPS15_1)
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10060202)
  end
  return 0
end)

--花翁 高歌猛进
CK.register_skill_function(10160301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1003, target_posx, target_posy, cc.MOVE_JUMP, FPS15_1)
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10160302)
  end
  return 0
end)

--血蟾 魔蟾探舌 35200201
CK.register_skill_function(35200201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit >=0 and cm.skill_funcs.is_enemy(user_id,caster_unit_id,target_unit) then
      local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
      local dx,dy = cm.skill_funcs.get_dx_dy(x, y, target_posx, target_posy)
      local step_off = math.abs(target_posx-x) + math.abs(target_posy-y)
      
      for i=0,(step_off-1) do
        local t_x = x-(step_off-i)*dx
        local t_y = y-(step_off-i)*dy
        if cm.level_funcs.pos_can_stand(user_id,t_x,t_y) then
          JUMP_COMBAT_UNIT(user_id, target_unit, (1000+step_off*2-i), t_x, t_y, cc.MOVE_JUMP, FPS15_16)
          cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 35200202)
          break
        end
      end
    end
  end
  return 0
end)

--醉仙 醉翁之意 10210201
CK.register_skill_function(10210201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then --必然命中技能或闪避后
    --添加buff
    local buffs = cm.capi_funcs.get_unit_buff(caster_unit_id)
    local index = 0
    for key in pairs(buffs) do
      if buffs[key] == 10210210 then index = 1 break end
      if buffs[key] == 10210211 then index = 2 break end
      if buffs[key] == 10210212 then index = 3 break end
      if buffs[key] == 10210213 then index = 4 break end
      if buffs[key] == 10210214 then index = 4 break end
    end
    cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, caster_unit_id, 10210210 + index)
    
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    local x_off,y_off = cm.skill_funcs.get_face_pos(user_id, caster_unit_id, 1)
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1001, x+x_off, y+y_off, cc.MOVE_RUSH, -1)--小跳
    x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    local target = cm.capi_funcs.get_unit_by_pos(user_id, x+x_off, y+y_off)
    if target>0 then
      if cm.skill_funcs.is_enemy(user_id,caster_unit_id,target) then
        cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x+x_off, y+y_off, 10210101)--连普攻
      end
    end
  end
  return 0
end)

--史燕 飞燕惊空 10320201
CK.register_skill_function(10320201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1004, target_posx, target_posy, cc.MOVE_RUSH, FPS15_1)
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10320202)
  end
  return 0
end)

--巩光杰 飞镰拘魂 10370201
CK.register_skill_function(10370201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then--技能处理完成后调用
    local target_unit = -1
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    local dx,dy = cm.skill_funcs.get_dx_dy(x, y, target_posx, target_posy)
    for i=1,5 do
      target_unit = cm.capi_funcs.get_unit_by_pos(user_id, x+i*dx, y+i*dy)--找到该方向上最近的单位
      if target_unit>=0 then
        if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target_unit) then
          cm.level_funcs.set_mark(user_id,caster_unit_id,i) -- 设置标记
        end
        local x,y = cm.capi_funcs.get_unit_pos(target_unit)
        cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10370202)
        return 0
      end
    end
  end
  return result
end)

--巩光杰 飞镰拘魂 10370202
CK.register_skill_function(10370202,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local t = cm.level_funcs.get_mark(caster_unit_id) --距离标记 获得
      if t>0 then
        local maxhp = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_HPMAX)
        result = 10*(maxhp*t*5/100) --每距离1格造成5%总血量的伤害
        cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10370201)
      else
        result = 1
      end
    end
    cm.level_funcs.set_mark(user_id,caster_unit_id,0) -- 清除标记
  end
  return result
end)

--沈丘山 金刀驭空 10480201
CK.register_skill_function(10480201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1003, target_posx, target_posy, cc.MOVE_RUSH, FPS15_1)
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10480202)
  end
  return 0
end)

--雷震天 人锤合一 10490301
CK.register_skill_function(10490301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1002, target_posx, target_posy, cc.MOVE_RUSH, FPS15_0)
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10490302)
  end
  return 0
end)

--银钩 地躺突袭 31300201
CK.register_skill_function(31300201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1002, target_posx, target_posy, cc.MOVE_RUSH, FPS15_1)
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 31300202)
  end
  return 0
end)

--赛飞鸿 进退自如 10500201
CK.register_skill_function(10500201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local f = cm.capi_funcs.get_unit_face(caster_unit_id)
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1111, target_posx, target_posy, cc.MOVE_RUSH, FPS15_0)
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    if f ==1 then y=y-1 end
    if f ==2 then x=x+1 end
    if f ==3 then y=y+1 end
    if f ==4 then x=x-1 end
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10500202)
  end
  return 0
end)

--赵惊风 刀刃旋风 10510301
CK.register_skill_function(10510301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local d = cm.skill_funcs.get_rush_dis(user_id, caster_unit_id, target_posx, target_posy, 3)
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    if d > 0 then
      cm.level_funcs.set_mark(user_id, caster_unit_id, d-1)
      local x_off,y_off = cm.skill_funcs.get_face_pos(user_id, caster_unit_id, 1)
      JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1001, x+x_off, y+y_off, cc.MOVE_RUSH, FPS15_0)
      x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
      cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10510302)
    else
      cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10510303)
    end
  end
  return 0
end)

--赵惊风 刀刃旋风-后续 10510302
CK.register_skill_function(10510302,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    local x_off,y_off = cm.skill_funcs.get_face_pos(user_id, caster_unit_id, 1)
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1001, x+x_off, y+y_off, cc.MOVE_RUSH, FPS15_0)
    
    local xx,yy = cm.capi_funcs.get_unit_pos(caster_unit_id)
    if xx==x and yy==y then --已经卡住
      cm.level_funcs.set_mark(user_id, caster_unit_id, 0)
      cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10510303)
    else
      x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
      if math.random(1,3) >= 1 then
        local d = cm.level_funcs.get_mark(caster_unit_id)
        d = d - 1
        cm.level_funcs.set_mark(user_id, caster_unit_id, d)
        if d > 0 then
          cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10510302)
        else
          cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10510303)
        end
      else
        cm.level_funcs.set_mark(user_id, caster_unit_id, 0)
        cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10510303)
      end
    end
  end
  return 0
end)

--贾云长 日月大刀 10040201
CK.register_skill_function(10040201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
  cm.checkValue(user_id,caster_unit_id,-1)
    if math.random(1,2) == 1 then
  cm.checkValue(user_id,caster_unit_id,-2)
      cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, target_posx, target_posy, 10040202)
    else
  cm.checkValue(user_id,caster_unit_id,-3)
      cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, target_posx, target_posy, 10040203)
    end
  end
  return 0
end)

--叶孤 追魂夺命 10570201
CK.register_skill_function(10570201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_SKILL_DONE then
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    local dx,dy = cm.skill_funcs.get_dx_dy(x, y, target_posx, target_posy)
    for i=1,9 do
      local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, x+i*dx, y+i*dy)--找到该方向上最近的单位
      if ((x+i*dx) == target_posx) and ((y+i*dy) == target_posy) then
        if target_unit>=0 then
          JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1004, target_posx, target_posy, cc.MOVE_RUSH, FPS15_1)
          x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
          cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10570202)
        end
      else
        if target_unit>=0 then
          return 0
        end
      end
    end
  end
  return 0
end)

--萧遥 飞龙在天 10580201
CK.register_skill_function(10580201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1004, target_posx, target_posy, cc.MOVE_RUSH, FPS15_1)
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10580202)
  end
  return 0
end)

--萧遥 神龙摆尾 10580301
CK.register_skill_function(10580301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local f = cm.capi_funcs.get_unit_face(caster_unit_id)
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1111, target_posx, target_posy, cc.MOVE_RUSH, FPS15_2)
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    if f ==1 then y=y-1 end
    if f ==2 then x=x+1 end
    if f ==3 then y=y+1 end
    if f ==4 then x=x-1 end
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10580302)
  end
  return 0
end)

--万凯  螳螂捕蝉（反击）10680402
CK.register_skill_function(10680402,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local f = cm.capi_funcs.get_unit_face(caster_unit_id)
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1111, target_posx, target_posy, cc.MOVE_RUSH, FPS15_1)
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    if f ==1 then y=y-1 end
    if f ==2 then x=x+1 end
    if f ==3 then y=y+1 end
    if f ==4 then x=x-1 end
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 10680403)
  end
  return 0
end)

--纪玟 鞭长可及 10420301
CK.register_skill_function(10420301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then--技能处理完成后调用
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, target_posx, target_posy, 10420302)
    end
  end
  return result
end)

--纪玟 鞭长可及 10420302
CK.register_skill_function(10420302,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      if cm.skill_funcs.is_enemy(user_id, caster_unit_id, target_unit) then
        result = 100
      else
        result = 1
      end
    end
  end
  return result
end)


----------------------------------------------------------------------------------------------------------------------------------------
--以下为主角技能相关
----------------------------------------------------------------------------------------------------------------------------------------

--主角 普攻 附带效果判定
local function check_normal_attack(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type == cc.SKILL_SCRIPT_CALL_SKILL_DONE then
    if cm.skill_funcs.target_has_buff(caster_unit_id,1004020) then  --命中后判定是否毒性攻击
      local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
      if target_unit>=0 then
        cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10530201) --潜龙毒影
      end
    end
  end
end

----------------------------------------------------------------------------------------------------------------------------------------
--以下为主角技能 刀系
----------------------------------------------------------------------------------------------------------------------------------------

--主角 刀 普攻 1000001
CK.register_skill_function(1000001,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  cm.level_funcs.set_mark(user_id, caster_unit_id, 0)
  check_normal_attack(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  return 0
end)

--主角 刀 逍遥刀法 1000101
CK.register_skill_function(1000101,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    result = 100 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, skill_id)
  else
    local chain = cm.level_funcs.get_mark(caster_unit_id)
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      if chain == 0 then
        local t = math.random(0,100)
        if t>50 then  --50%几率
          cm.level_funcs.set_mark(user_id, caster_unit_id, 1) --连击1次
          cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, target_posx, target_posy, 1000101)
        end
      else
        if chain > 1 then
          cm.level_funcs.set_mark(user_id, caster_unit_id, chain -1)
          cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, target_posx, target_posy, 1000101)
        else
          cm.level_funcs.set_mark(user_id, caster_unit_id, 0)
        end
      end
    else
      cm.level_funcs.set_mark(user_id, caster_unit_id, 0)
    end
  end
  return result
end)

--主角 刀 金刀驭空 1000151
CK.register_skill_function(1000151,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1003, target_posx, target_posy, cc.MOVE_RUSH, FPS15_11)
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 1000152)
  end
  return 0
end)

--主角 刀 金刀驭空 1000152
CK.register_skill_function(1000152,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    result = 100 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, 1000151)
  end
  return result
end)

--主角 刀 八方风雨 1000201
CK.register_skill_function(1000201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    result = 100 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, skill_id)
  end
  return result
end)

--主角 刀 破防绝刀 1000301
CK.register_skill_function(1000301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    result = 100 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, skill_id)
  end
  return result
end)

--主角 刀 春秋刀法 1000401
CK.register_skill_function(1000401,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    result = 100 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, skill_id)
  end
  return result
end)

--主角 刀 刀光剑影 1000501
CK.register_skill_function(1000501,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    result = 100 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, skill_id)
  end
  return result
end)

--主角 刀 日月大刀 1000601
CK.register_skill_function(1000601,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    result = 100 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, skill_id)
  end
  return result
end)

--主角 刀 最后一击 1000701
CK.register_skill_function(1000701,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      if cm.unit_funcs.hp_less_than_percent(target_unit, 20) then
        cm.capi_funcs.set_unit_attr(user_id, target_unit, 3, 0)
      else
        result = 100 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, skill_id)
      end
    end
  end
  return result
end)

----------------------------------------------------------------------------------------------------------------------------------------
--以下为主角技能 剑系
----------------------------------------------------------------------------------------------------------------------------------------

--主角 剑 普攻 2000001
CK.register_skill_function(2000001,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  cm.level_funcs.set_mark(user_id, caster_unit_id, 0)
  check_normal_attack(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  return 0
end)

--主角 剑 逍遥剑法 2000101
CK.register_skill_function(2000101,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    result = 100 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, skill_id)
  else
    local chain = cm.level_funcs.get_mark(caster_unit_id)
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      if chain == 0 then
        local t = math.random(0,100)
        if t>50 then  --50%几率
          cm.level_funcs.set_mark(user_id, caster_unit_id, 1) --连击1次
          cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, target_posx, target_posy, 2000101)
        end
      else
        if chain > 1 then
          cm.level_funcs.set_mark(user_id, caster_unit_id, chain -1)
          cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, target_posx, target_posy, 2000101)
        else
          cm.level_funcs.set_mark(user_id, caster_unit_id, 0)
        end
      end
    else
      cm.level_funcs.set_mark(user_id, caster_unit_id, 0)
    end
  end
  return result
end)

--主角 剑 一剑穿心 2000151
CK.register_skill_function(2000151,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1004, target_posx, target_posy, 3, FPS15_13)
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 2000152)
  end
  return 0
end)

--主角 剑 一剑穿心 2000152
CK.register_skill_function(2000152,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    result = 100 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, 2000151)
  end
  return result
end)

--主角 剑 火云剑法 2000201
CK.register_skill_function(2000201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 2000202)
  end
  return 0
end)

--主角 剑 魔剑噬心 2000301
CK.register_skill_function(2000301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local hp = cm.capi_funcs.get_unit_attr(target_unit, 4)
      local maxhp = cm.capi_funcs.get_unit_attr(target_unit, cc.FIGHT_ATTR_HPMAX)
      local t = hp / maxhp
      local buff_id = 10120402
      if t < 0.9 then buff_id = 10120403 end
      if t < 0.8 then buff_id = 10120404 end
      if t < 0.7 then buff_id = 10120405 end
      if t < 0.6 then buff_id = 10120406 end
      if t < 0.5 then buff_id = 10120407 end
      if t < 0.4 then buff_id = 10120408 end
      if t < 0.3 then buff_id = 10120409 end
      if t < 0.2 then buff_id = 10120410 end
      if t < 0.1 then buff_id = 10120411 end
      cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, buff_id)
    end
  end
  return 0
end)

--主角 剑 千锤百炼 2000501
CK.register_skill_function(2000501,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local weapon = cm.capi_funcs.get_atk_type(target_unit)
      if weapon == 2 then --武器类型为剑
        if cm.skill_funcs.target_has_buff(target_unit, 10600202) then
          cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10600203)
          return 0
        end
        if cm.skill_funcs.target_has_buff(target_unit, 10600203) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10600204) return 0 end
        if cm.skill_funcs.target_has_buff(target_unit, 10600204) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10600205) return 0 end
        if cm.skill_funcs.target_has_buff(target_unit, 10600205) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10600206) return 0 end
        if cm.skill_funcs.target_has_buff(target_unit, 10600206) then cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10600206) return 0 end
        cm.capi_funcs.add_unit_buff(user_id, target_unit, target_unit, 10600202)
      else
        cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10600201)
      end
    end
  end
  return 0
end)

--主角 剑 决一雌雄 2000701
CK.register_skill_function(2000701,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then --计算伤害时调用
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local hard1 = cm.capi_funcs.get_unit_attr(caster_unit_id, 6)
      local hard2 = cm.capi_funcs.get_unit_attr(target_unit, 6)
      if hard1>hard2 then
        result = 200
      else
        result = 100
      end
    end
    result = result + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, skill_id)
  end
  return result 
end)

----------------------------------------------------------------------------------------------------------------------------------------
--以下为主角技能 棍系
----------------------------------------------------------------------------------------------------------------------------------------

--主角 棍 普攻 3000001
CK.register_skill_function(3000001,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  cm.level_funcs.set_mark(user_id, caster_unit_id, 0)
  check_normal_attack(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  return 0
end)

--主角 棍 逍遥棍法 3000101
CK.register_skill_function(3000101,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    result = 100 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, skill_id)
  else
    local chain = cm.level_funcs.get_mark(caster_unit_id)
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      if chain == 0 then
        local t = math.random(0,100)
        if t>50 then  --50%几率
          cm.level_funcs.set_mark(user_id, caster_unit_id, 1) --连击1次
          cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, target_posx, target_posy, 3000101)
        end
      else
        if chain > 1 then
          cm.level_funcs.set_mark(user_id, caster_unit_id, chain -1)
          cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, target_posx, target_posy, 3000101)
        else
          cm.level_funcs.set_mark(user_id, caster_unit_id, 0)
        end
      end
    else
      cm.level_funcs.set_mark(user_id, caster_unit_id, 0)
    end
  end
  return result
end)

--主角 棍 高歌猛进 3000151
CK.register_skill_function(3000151,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  --checkValue(user_id, caster_unit_id, -10000)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1003, target_posx, target_posy, cc.MOVE_JUMP, FPS15_15)
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 3000152)
  end
  return 0
end)

--主角 棍 高歌猛进 3000152
CK.register_skill_function(3000152,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    result = 100 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, 3000151)
  end
  return result
end)

--主角 棍 群魔辟易 3000201
CK.register_skill_function(3000201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    result = 100 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, skill_id)
  end
  return result
end)

--主角 棍 降魔棍法 3000301
CK.register_skill_function(3000301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    result = 100 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, skill_id)
  end
  return result
end)

--主角 棍 护法金刚 3000401
CK.register_skill_function(3000401,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > 1 then  --技能处理完成后调用
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      local hp = cm.capi_funcs.get_unit_attr(target_unit, 3)
      cm.capi_funcs.set_unit_attr(user_id, target_unit, 21, hp) --气防设为与当前HP相同
    end
  end
  return 0
end)

--主角 棍 千钧重担 3000701
CK.register_skill_function(3000701,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 3000702)
  end
  return 0
end)

----------------------------------------------------------------------------------------------------------------------------------------
--以下为主角技能 拳系
----------------------------------------------------------------------------------------------------------------------------------------

--主角 拳 普攻 4000001
CK.register_skill_function(4000001,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  cm.level_funcs.set_mark(user_id, caster_unit_id, 0)
  check_normal_attack(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  
  --野球诀 连击判定
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    if cm.skill_funcs.target_has_buff(caster_unit_id,1002210) then
      local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
      if target_unit>=0 then
        local t = math.random(0,100)
        if t>50 then  --50%几率
          cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, target_posx, target_posy, 4000001)
        end
      end
    end
  end
  
  return 0
end)

--主角 拳 逍遥拳法 4000101
CK.register_skill_function(4000101,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    result = 100 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, skill_id)
  else
    local chain = cm.level_funcs.get_mark(caster_unit_id)
    local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
    if target_unit>=0 then
      if chain == 0 then
        local t = math.random(0,100)
        if t>50 then  --50%几率
          cm.level_funcs.set_mark(user_id, caster_unit_id, 1) --连击1次
          cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, target_posx, target_posy, 4000101)
        end
      else
        if chain > 1 then
          cm.level_funcs.set_mark(user_id, caster_unit_id, chain -1)
          cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, target_posx, target_posy, 4000101)
        else
          cm.level_funcs.set_mark(user_id, caster_unit_id, 0)
        end
      end
    else
      cm.level_funcs.set_mark(user_id, caster_unit_id, 0)
    end
  end
  return result
end)

--主角 拳 醉翁之意 4000151
CK.register_skill_function(4000151,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then --必然命中技能或闪避后
    --添加buff
    local buffs = cm.capi_funcs.get_unit_buff(caster_unit_id)
    local index = 0
    for key in pairs(buffs) do
      if buffs[key] == 10210210 then index = 1 break end
      if buffs[key] == 10210211 then index = 2 break end
      if buffs[key] == 10210212 then index = 3 break end
      if buffs[key] == 10210213 then index = 4 break end
      if buffs[key] == 10210214 then index = 4 break end
    end
    cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, caster_unit_id, 10210210 + index)
    
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    local x_off,y_off = cm.skill_funcs.get_face_pos(user_id, caster_unit_id, 1)
    JUMP_COMBAT_UNIT(user_id, caster_unit_id, 1001, x+x_off, y+y_off, cc.MOVE_RUSH, 13)--小跳
    x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    local target = cm.capi_funcs.get_unit_by_pos(user_id, x+x_off, y+y_off)
    if target>0 then
      if cm.skill_funcs.is_enemy(user_id,caster_unit_id,target) then
        cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x+x_off, y+y_off, 4000152)--连普攻
      end
    end
  end
  return 0
end)

--主角 拳 醉翁之意 4000152
CK.register_skill_function(4000152,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then
    result = 100 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, 4000151)
  end
  return result
end)

--主角 拳 以酒为剑 4000201
CK.register_skill_function(4000201,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local x,y = cm.capi_funcs.get_unit_pos(caster_unit_id)
    cm.capi_funcs.unit_cast_skill(user_id, caster_unit_id, x, y, 4000202)
  end
  return 0
end)

--主角 拳 以酒为箭
CK.register_skill_function(4000202,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then --计算伤害时调用
    local buffs = cm.capi_funcs.get_unit_buff(caster_unit_id)
    local index = 0
    for key in pairs(buffs) do
      if buffs[key] == 10210214 then index = 5 break end
      if buffs[key] == 10210213 then index = 4 break end
      if buffs[key] == 10210212 then index = 3 break end
      if buffs[key] == 10210211 then index = 2 break end
      if buffs[key] == 10210210 then index = 1 break end
    end
    result = 100 + index * 30 + GET_ROLEUNIT_SKILL_LEVEL(user_id, caster_unit_id, 4000201) --每层酒意增加30%伤害
  end
  return result
end)

--主角 拳 动手动脚 4000301
CK.register_skill_function(4000301,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if execute_type == cc.SKILL_SCRIPT_CALL_SKILL_DONE then --技能处理完成后调用
    if target_unit>=0 then
      if not cm.skill_funcs.is_male(user_id,target_unit) then
        if math.random(1,2)==1 then
          cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 1102)--禁止移动
        else
          cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 1302)--禁止发招
        end
      end
      return 0
    end
  end
  return result
end)

--主角 拳 见猎心喜 4000401
CK.register_skill_function(4000401,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then --技能处理完成后调用
    local enemycamp = cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id)
    local t = cm.team_funcs.get_gender_number(user_id, enemycamp, 100)
    if t>0 then
      cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, caster_unit_id, 190100+t)--敌方每名女性提升10%伤害
    end
  end
  return 0
end)

--主角 拳 顺手牵羊 4000501
CK.register_skill_function(4000501,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  local target_unit = cm.capi_funcs.get_unit_by_pos(user_id, target_posx, target_posy)
  if execute_type == cc.SKILL_SCRIPT_CALL_SKILL_DONE then --技能处理完成后调用
    if target_unit > 0 then
      cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, target_unit, 10320301) --标记为失窃
      if cm.level_funcs.is_PVP(caster_unit_id, target_unit) then
        --随机偷取目标基础属性
        local index = math.random(6,11)
        local value1 = cm.capi_funcs.get_unit_attr(caster_unit_id, index)
        local value2 = cm.capi_funcs.get_unit_attr(target_unit, index)
        local t = math.max(1,math.floor(value2/10)) --偷取10%当前值，至少1点
        if (value2-t)>0 then
          cm.capi_funcs.set_unit_attr(user_id, caster_unit_id, index, value1+t)
          cm.capi_funcs.set_unit_attr(user_id, target_unit, index, value2-t)
        end
      end
      if not skill_funcs.target_has_buff(target_unit, 10320301) then
        cm.capi_funcs.add_drop(user_id, 194, cc.DROP_INSTANTLY) --立即获得掉落
      end
    end
  end
  return result
end)

--主角 拳 乾坤一掷 4000601
CK.register_skill_function(4000601,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result= 100
  if execute_type == cc.SKILL_SCRIPT_CALL_DAMAGE then --计算伤害时调用
    local attack_face = cm.capi_funcs.get_unit_face(caster_unit_id)
    local targets = GET_AREA_TARGET_UNIT(user_id, target_posx, target_posy, cc.AREA_AROUND, cm.skill_funcs.get_enemy_camp(user_id, caster_unit_id), attack_face)
    local d = 20  --每个范围内的失窃单位提供额外20%伤害
    for key in pairs(targets) do
      if cm.skill_funcs.target_has_buff(targets[key], 10320301) then
        result = result + d
      end
    end
  end
  return result
end)

--主角 拳 逆劲护元 4000701
CK.register_skill_function(4000701,
function(user_id, level_id, skill_id, caster_unit_id, target_posx, target_posy, execute_type)
  local result = 0
  if execute_type > cc.SKILL_SCRIPT_CALL_DAMAGE then
    local hp = cm.capi_funcs.get_unit_attr(caster_unit_id, 3)
    local maxhp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HPMAX)
    local mp = cm.capi_funcs.get_unit_attr(caster_unit_id, 4)
    local maxmp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_MPMAX)
    cm.capi_funcs.set_unit_attr(user_id, caster_unit_id, 3, maxhp * mp/maxmp)
    cm.capi_funcs.set_unit_attr(user_id, caster_unit_id, 4, maxmp * hp/maxhp)
  end
  return result
end)



--------------------------------------------------------------------------------------------------
--以下为剧情战斗技能
--------------------------------------------------------------------------------------------------






return CK