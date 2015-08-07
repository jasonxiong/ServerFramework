
local cc = require("combat_const")
local cm = require("combat_module")

local CI = {}
CI.item_function_map = {}
function CI.register_item_function(item_id, func)
  CI.item_function_map[item_id] = func
end
function CI.get_item_function(item_id)
    return CI.item_function_map[item_id]
end

--空item模板
local function item_1000000(userID, level_id, item_id, caster_unit_id, target_posx, target_posy)
  local result = 0
  
  return result
end
CI.register_item_function(1000000, item_1000000)

--杨云 后劲十足
local function drink_yangyun(userID, caster_unit_id)
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
    if buffs[key] == 10690409 then index = 8 break end
  end
  --cm.checkValue(userID, caster_unit_id, -10-index)
  cm.capi_funcs.add_unit_buff(userID, caster_unit_id, caster_unit_id, 10690401+index)
end

--1000016 杜康酒
CI.register_item_function(1000016,
function(userID, level_id, item_id, caster_unit_id, target_posx, target_posy)
  local result = 0
  drink_yangyun(userID, caster_unit_id)
  
  local hp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HP)
  local maxhp = cm.capi_funcs.get_unit_attr(caster_unit_id, cc.FIGHT_ATTR_HPMAX)
  cm.capi_funcs.set_unit_attr(userID, caster_unit_id, cc.FIGHT_ATTR_HP, hp + maxhp / 10)
  
  return result
end)

return CI



