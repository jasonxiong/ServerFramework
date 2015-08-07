
local cc = require("combat_const")
local cm = require("combat_module")

local CT = {}
CT.trap_function_map = {}
function CT.register_trap_function(trap_id, func)
  CT.trap_function_map[trap_id] = func
end
function CT.get_trap_function(trap_id)
    return CT.trap_function_map[trap_id]
end

--空trap模板
CT.register_trap_function(1000000,
function(userID, trigger_unit_id, trap_obj_id, trap_config_id, trigger_type)
  local result = 0
  DELETE_COMBAT_TRAP(userID, trap_obj_id)
  return result
end)

--捕兽夹
CT.register_trap_function(1,
function(userID, trigger_unit_id, trap_obj_id, trap_config_id, trigger_type)
  local result = 0
    local hp = GET_COMBAT_UNIT_ATTR(trigger_unit_id, cc.FIGHT_ATTR_HP)
    SET_COMBAT_UNIT_ATTR(userID, trigger_unit_id, cc.FIGHT_ATTR_HP, hp*(1-1/3)) -- 扣除1/3当前生命
    DELETE_COMBAT_TRAP(userID, trap_obj_id)
    
    ADD_UNIT_BUFF(userID, trigger_unit_id, trigger_unit_id, 1102) --禁止移动
  return result
end)

--HP机关测试
CT.register_trap_function(2,
function(userID, trigger_unit_id, trap_obj_id, trap_config_id, trigger_type)
  local result = 0
    local hp = GET_COMBAT_UNIT_ATTR(trigger_unit_id, cc.FIGHT_ATTR_HP)
    SET_COMBAT_UNIT_ATTR(userID, trigger_unit_id, cc.FIGHT_ATTR_HP, hp*(1-1/3)) -- 扣除1/3当前生命
    DELETE_COMBAT_TRAP(userID, trap_obj_id)
    
  return result
end)


return CT