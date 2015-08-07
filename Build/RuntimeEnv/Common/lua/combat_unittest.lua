local function main()
package.path = 'D:/lua/?.lua';
  require("CppAPI")
  require("combat_main")
  
  print(ExecuteSkillAIScript(1, 1, 10270201, 10, 11, 12, 13, 14, 1, 1))
  
  --[[
  local cl = require("combat_level")
  for key in pairs(cl.level_function_map) do
    ExecuteCombatScript(1, cl.level_function_map[key], 1)
  end
  ]]--
  
  --[[
  local ck = require("combat_skill")
  for key in pairs(ck.skill_function_map) do
    ExecuteSkillScript(1, 1, ck.skill_function_map[key], 1, 1, 1, 1)
    ExecuteSkillScript(1, 1, ck.skill_function_map[key], 1, 1, 1, 2)
  end
  ]]--
  
  --[[
  local cb = require("combat_buff")
  for key in pairs(cb.buff_function_map) do
    ExcuteBuffScript(1, 1, cb.buff_function_map[key], 1, 1, 1)
  end
  ]]--
  
  --[[
  local ca = require("combat_ai")
  for key in pairs(ca.ai_check_map) do
    ca.ai_check_map[key](1, 1)
  end
  ]]--
  
  --[[
  local cv = require("combat_value")
  for key in pairs(cv.skill_value_map) do
    ExecuteSkillAIScript(1, 1, cv.skill_value_map[key], 1, 1, 1, 1, 1, 1, 1)
  end
  ]]--
  
  --[[
  local ci = require("combat_item")
  for key in pairs(ci.item_function_map) do
    ExecuteUseItemScript(1, 1, ci.item_function_map, 1, 1, 1)
  end
  ]]--
  
  --[[
  local ct = require("combat_trap")
  for key in pairs(ct.trap_function_map) do
    ExecuteTrapTriggerScript(1, ct.trap_function_map, 1, 1, 1)
  end
  ]]--
  
end
main()
