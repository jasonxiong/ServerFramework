
local cc = require("combat_const")
local cm = require("combat_module")
local ca = require("combat_ai")

local CB = {}
CB.buff_function_map = {}
function CB.register_buff_function(buff_id, func)
  CB.buff_function_map[buff_id] = func
end
function CB.get_buff_function(buff_id)
    return CB.buff_function_map[buff_id]
end

--空BUFF模板
CB.register_buff_function(1000000,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  return 0
end)

--沈湘云 紧急治疗10220201
CB.register_buff_function(10220201,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  local hp = cm.capi_funcs.get_unit_attr(carrier_unit_id, cc.FIGHT_ATTR_HP)
  local maxhp = cm.capi_funcs.get_unit_attr(carrier_unit_id, cc.FIGHT_ATTR_HPMAX)
  cm.capi_funcs.set_unit_attr(user_id, carrier_unit_id, cc.FIGHT_ATTR_HP, hp + 50 + maxhp * 10 / 100) --50 + 10%HP
  
  local bufftype = math.random(1,6)
  local buffid = 10000 * bufftype + math.random(0,1)*1000 + 101
  cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, carrier_unit_id, buffid)
  return 0
end)

--神医 妙手回春 10180201
CB.register_buff_function(10180201,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  local hp = cm.capi_funcs.get_unit_attr(carrier_unit_id, cc.FIGHT_ATTR_HP)
  local maxhp = cm.capi_funcs.get_unit_attr(carrier_unit_id, cc.FIGHT_ATTR_HPMAX)
  cm.capi_funcs.set_unit_attr(user_id, carrier_unit_id, cc.FIGHT_ATTR_HP, hp + 25 + maxhp * 5 / 100) --25 + 5%HP
  return 0
end)

--关伟 坚守阵地
CB.register_buff_function(10050401,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  local hp = cm.capi_funcs.get_unit_attr(carrier_unit_id, cc.FIGHT_ATTR_HP)
  local maxhp = cm.capi_funcs.get_unit_attr(carrier_unit_id, cc.FIGHT_ATTR_HPMAX)
  local index = 10 - math.floor(10 * hp / maxhp)
  if index>0 then
    cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, carrier_unit_id, 10050401+index)
  else
    cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, carrier_unit_id, 10050412)
  end
  return 0
end)

--荆棘 魔剑噬心
CB.register_buff_function(10120401,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  local hp = cm.capi_funcs.get_unit_attr(carrier_unit_id, cc.FIGHT_ATTR_HP)
  local maxhp = cm.capi_funcs.get_unit_attr(carrier_unit_id, cc.FIGHT_ATTR_HPMAX)
  local index = 10 - math.floor(10 * hp / maxhp)
  if index>0 then
    cm.capi_funcs.add_unit_buff(user_id, carrier_unit_id, carrier_unit_id, 10120401+index)
  else
    cm.capi_funcs.add_unit_buff(user_id, carrier_unit_id, carrier_unit_id, 10120412)
  end
  return 0
end)

--丹青 诗情画意 添加BUFF
CB.register_buff_function(10150401,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, carrier_unit_id, 10150402)
  return 0
end)

--丹青 诗情画意 受到伤害删除BUFF
CB.register_buff_function(10150402,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  cm.capi_funcs.del_unit_buff(user_id, carrier_unit_id, 10150401)
  return 0
end)

--西门峰 削金破腕 10030101
local function buff_10030101(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  local r = math.random(1,10)
  if r > 5 then
    cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, carrier_unit_id, 10310201)--缴械
  end
  return 0
end
CB.register_buff_function(10030101, buff_10030101)

--江瑜 立地成佛 移动打断 10020301
CB.register_buff_function(10020301,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  cm.capi_funcs.set_unit_attr(user_id, carrier_unit_id, cc.FIGHT_ATTR_QIFANG, 0) --移动后将气防值设为0点
  cm.capi_funcs.del_unit_buff(user_id, carrier_unit_id, 10020301)
  return 0
end)

--方云华 栽赃嫁祸 10060301
CB.register_buff_function(10060301,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  cm.capi_funcs.add_unit_buff(user_id, carrier_unit_id, caster_unit_id, 10060302) -- carrier_unit_id 代替 caster_unit_id 承受伤害
  return 0
end)

--方云华 逍遥法外 10060302
CB.register_buff_function(10060302,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  cm.capi_funcs.del_unit_buff(user_id, caster_unit_id, 10060301)
  cm.capi_funcs.del_unit_buff(user_id, carrier_unit_id, 10060302)
  return 0
end)

--赌 乾坤一掷 标记 10270301
CB.register_buff_function(10270301,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  cm.level_funcs.add_mark(user_id, caster_unit_id, 1)
  return 0
end)

--丁冲 独孤残式 10310101
CB.register_buff_function(10310101,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, carrier_unit_id, 10310201) -- 缴械
  return 0
end)

--香儿 沉鱼落雁 10550401
CB.register_buff_function(10550401,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  local f1 = cm.capi_funcs.get_unit_face(carrier_unit_id)
  local f2 = cm.capi_funcs.get_atk_face(carrier_unit_id, caster_unit_id)
  if f1==f2 then
    cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, carrier_unit_id, 1102)--无法移动
  end
  return 0
end)

--橘叟 移形换位 敌意禁足 10170401
CB.register_buff_function(10170401,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  if cm.skill_funcs.is_enemy(user_id, carrier_unit_id, caster_unit_id) then
    cm.capi_funcs.add_unit_buff(user_id, caster_unit_id, carrier_unit_id, 1102)
  end
  return 0
end)

--任清璇 宁神幽香 10630401
CB.register_buff_function(10630401,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  cm.buff_funcs.clear_bad_buff(user_id, carrier_unit_id)
  return 0
end)

--道具 解毒
CB.register_buff_function(280002,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  cm.buff_funcs.clear_poison(user_id, carrier_unit_id)
  return 0
end)


----------------------------------------------------------------------------------------------------------------------------------------
--以下为心法处理
----------------------------------------------------------------------------------------------------------------------------------------

--气防
CB.register_buff_function(1002000,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  local maxhp = cm.capi_funcs.get_unit_attr(carrier_unit_id, cc.FIGHT_ATTR_HPMAX)
  cm.capi_funcs.set_unit_attr(user_id, carrier_unit_id, 21, maxhp*0.1) --获得最大生命10%的气防
  return 0
end)

--气防
CB.register_buff_function(1004051,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  local maxhp = cm.capi_funcs.get_unit_attr(carrier_unit_id, cc.FIGHT_ATTR_HPMAX)
  cm.capi_funcs.set_unit_attr(user_id, carrier_unit_id, 21, maxhp*0.1) --获得最大生命10%的气防
  return 0
end)

----------------------------------------------------------------------------------------------------------------------------------------
--以下为轮到行动处理 用于AI或剧情
----------------------------------------------------------------------------------------------------------------------------------------
CB.register_buff_function(1000,
function(user_id, level_id, buff_config_id, carrier_unit_id, caster_unit_id, trigger_unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  local unit_base_id = GET_COMBAT_UNIT_EXTRAATTR(user_id, carrier_unit_id, 3)
  if unit_base_id == 1 then --主角
    if level_id == 2000003 then --小山剧情
      if turn == 1 then
        cm.level_funcs.show_movie(user_id,6013)--夹击后对话
      end
    end
  else
    if turn == 1 then
      if GET_COMBAT_STATUS(user_id, 7) > 0 then return 0 end  --状态7设为1时不播放开场对白
    end
    local ai_check = ca.get_ai_check(unit_base_id) --以单位的配置表id为AI_CHECK的key
    if ai_check then ai_check(user_id, carrier_unit_id) end
  end
  return 0
end)


return CB



