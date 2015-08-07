
local cc = require("combat_const")
local cm = require("combat_module")

local CL = {}
CL.level_function_map = {}
function CL.register_level_function(level_id, func)       --注册关卡脚本功能函数
  CL.level_function_map[level_id] = func
end
function CL.get_level_function(level_id)                  --获取关卡脚本功能函数
  return CL.level_function_map[level_id]
end

--空关卡模板
local function level_1000000(user_id, level_id, first)
  if first == cc.COMBAT_START then
    --战前初始化
  elseif IS_COMBAT_ROUND_END(user_id) then
    if GET_COMBAT_ROUND_NUM(user_id) == 5 then
      --第五回合
    end
  else
    --单位行动间歇
  end
  return 0
end
CL.register_level_function(1000000,level_1000000)

--剧情战斗：新手村村长
local function level_3000208(user_id, level_id, first)
  local ending = false
  if first==1 then
    cm.level_funcs.show_movie(user_id, 6001)
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)
  elseif IS_COMBAT_ROUND_END(user_id) then
    local turn = GET_COMBAT_ROUND_NUM(user_id)
    if turn == 1 then cm.level_funcs.show_movie(user_id, 6002) end
    if turn == 3 then ending = true end
  else
    if cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_enemy(user_id), 60) then ending = true end
  end
  if ending then
    cm.level_funcs.show_movie(user_id, 6003)
    SET_CAMP_ACTIVE_WIN(user_id, 1)
    return 1
  end
  return 0
end
CL.register_level_function(3000208,level_3000208)

--剧情战斗：野猪(开始要添加小山)
local function level_2000002(user_id, level_id, first)
  if first==1 then --战斗刚开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101047, 31, 10, cc.FACE_LEFT_UP) --添加小山
  end
  return 0
end
CL.register_level_function(2000002,level_2000002)

--剧情战斗：和熊(开始要添加小山)
local function level_2000003(user_id,level_id,first)
  if first==1 then --战斗刚开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101047, 19, -4, cc.FACE_LEFT_UP) --添加小山
    cm.level_funcs.show_movie(user_id, 6012)
  end
  return 0
end
CL.register_level_function(2000003,level_2000003)

--剧情战斗：劫案现场（关伟、关长虹 主角肯定1人）
local function level_3000218(user_id,level_id,first)
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101005, 135, -17, cc.FACE_RIGHT_DOWN)--关伟加入
    
    --补充怪物
    local unit_id = cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, 2200004, 137, -19, cc.FACE_LEFT_DOWN)
    CHANGE_COMBAT_NPC_AI(user_id, unit_id, cc.AI_MODE_SKILL_1)
    unit_id = cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, 2200004, 137, -15, cc.FACE_RIGHT_UP)
    CHANGE_COMBAT_NPC_AI(user_id, unit_id, cc.AI_MODE_SKILL_1)
    
  else
    --主角死亡则战败
    if cm.team_funcs.hero_dead(user_id,1,cc.COMBAT_CAMP_ACTIVE) then return cm.level_funcs.battle_lose(user_id) end
    --关伟死亡则战败
    if cm.team_funcs.hero_dead(user_id,2101005,cc.COMBAT_CAMP_ACTIVE) then return cm.level_funcs.battle_lose(user_id) end
    --关长虹加入判定
    if cm.team_funcs.hero_dead(user_id, 2300123, cc.COMBAT_CAMP_ACTIVE) then --关长虹尚未加入
      
      local t = false
      --if cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_hero(user_id), 40) then t = true end  --主角
      --if cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_enemy(user_id), 50) then t = true end --敌人：赵师傅
      if cm.unit_funcs.hp_less_than_percent(GET_COMBAT_SIDE(user_id, cc.COMBAT_CAMP_ACTIVE, 1), 60) then t = true end --关伟
      if cm.capi_funcs.get_round_end(user_id)==2 then t=true end
      
      if t then --关长虹加入
        cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2300123, 132, -18, cc.FACE_RIGHT_DOWN)
        cm.level_funcs.show_movie(user_id, 6008) --关长虹说话
      end
    end
  end
  return 0
end
CL.register_level_function(3000218,level_3000218)

--拜师考验 1200052
local function level_1200052(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2) --清空配角
    cm.team_funcs.set_weak(user_id,cc.COMBAT_CAMP_PASSIVE,50) --谷月轩变弱
  else
    if cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_enemy(user_id), 30) then --谷月轩血量低于40%即胜利
      SET_CAMP_ACTIVE_WIN(user_id, cc.RESULT_WIN)
      result = 1
    end
  end
  return result
end
CL.register_level_function(1200052,level_1200052)

--逍遥谷后山 毒蛇
local function level_2000020(user_id,level_id,first)
  if first==1 then --战斗开始 追加单位
    cm.level_funcs.put_monster_around(user_id, 2200020, cm.team_funcs.get_team_number(user_id, cc.COMBAT_CAMP_ACTIVE)+1)
    --cm.showDate(user_id)
    local YY,MM,DD,TT = cm.level_funcs.get_role_date(user_id)
    if YY > 1 or (MM > 9 and DD > 1) then
      ADD_EXTRA_COMBAT_DROPID(user_id, 196, 0)  --在特定剧情时间后才有蛇血掉落
    end
  end
  return 0
end
CL.register_level_function(2000020,level_2000020)

--逍遥谷后山 大蛇（大体积单位）
local function level_2000021(user_id,level_id,first)
  local result = 0
  local poisoned = false
  if first==1 then --战斗开始
    cm.level_funcs.show_movie(user_id, 6010)
  else
    if cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_enemy(user_id), 20) then poisoned = true end
    if cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_hero(user_id), 20) then poisoned = true end
  end
  if poisoned then --poisoned then
    cm.level_funcs.show_movie(user_id, 6011)
    SET_CAMP_ACTIVE_WIN(user_id, 0)
    result = 1
  end
  return result
end
CL.register_level_function(2000021,level_2000021)

 --无暇子 1000010
local function level_1000010(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2) --清空配角
  else
    if cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_enemy(user_id), 60) then --无暇子血量低于60%即胜利
      SET_CAMP_ACTIVE_WIN(user_id, cc.RESULT_WIN)
      result = 1
    end
  end
  return result
end
CL.register_level_function(1000010,level_1000010)

--主角+虚真 2100005
local function level_2100005(user_id,level_id,first)
  if first==1 then --战斗刚开始
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101030, 16, 2, cc.FACE_LEFT_UP) --添加虚真
  end
  return 0
end
CL.register_level_function(2100005,level_2100005)

--只有虚真 2200005
local function level_2200005(user_id,level_id,first)
  if first==1 then --战斗刚开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 1) --清空主角队伍
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101030, 16, 2, cc.FACE_LEFT_UP)--添加虚真
  end
  return 0
end
CL.register_level_function(2200005,level_2200005)

--剧情战斗：花翁
local function level_1000016(user_id,level_id,first)
  local result = 0
  if first==1 then
  else
    if cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_hero(user_id), 60) then --主角血量少于一定比例后失败
      SET_CAMP_ACTIVE_WIN(user_id, cc.RESULT_LOSE)
      result = 1
    end
  end
  return result
end
CL.register_level_function(1000016,level_1000016)

--剧情战斗：酒仙
local function level_1000021(user_id,level_id,first)
  local result = 0
  if first==1 then
  else
    if cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_hero(user_id), 60) then --主角血量少于一定比例后失败
      SET_CAMP_ACTIVE_WIN(user_id, cc.RESULT_LOSE)
      result = 1
    end
  end
  return result
end
CL.register_level_function(1000021,level_1000021)

--剧情战斗：叶枫（开始加古月轩、史纲）
local function level_2000017(user_id,level_id,first)
  if first==1 then
    local unit1 = cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101052, 11, 6, cc.FACE_RIGHT_DOWN)--加古月轩
    local unit2 = cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101061, 15, 6, cc.FACE_LEFT_UP)--加史纲
    
    SET_COMBAT_UNIT_ATTR(user_id, unit1, cc.FIGHT_ATTR_MOVE, 20) --移动修正
    SET_COMBAT_UNIT_ATTR(user_id, unit2, cc.FIGHT_ATTR_MOVE, 20) --移动修正
    
  else
    --判断是否为叶枫切换逃跑AI的标记
    local unit_id = cm.team_funcs.get_enemy(user_id)
    if cm.level_funcs.get_mark(unit_id) == 0 then
      if cm.unit_funcs.hp_less_than_percent(unit_id, 90) then
        cm.level_funcs.set_mark(user_id,unit_id,1)
      end
    end
    
    --逃跑成功判断与处理
    if cm.level_funcs.check_pos(unit_id,10,0) then
      cm.level_funcs.battle_lose(user_id)
    end
    
  end
  return 0
end
CL.register_level_function(2000017,level_2000017)

--剧情战斗：三次洛阳嫖 1100026
local function level_1100026(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 1)   --清空主角队伍
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101052, 15, 2, cc.FACE_LEFT_UP)  --谷月轩 2110052
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101012, 17, 2, cc.FACE_LEFT_UP)  --荆棘 2110012
  end
  return 0
end
CL.register_level_function(1100026,level_1100026)

--剧情战斗：三次洛阳假战报A   #0# 1000025
local function level_1000025(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 1)   --清空主角队伍
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101033, 14, 0, cc.FACE_LEFT_UP)--夏侯非
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101003, 15, 1, cc.FACE_LEFT_UP)--西门峰
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101045, 15, -1, cc.FACE_LEFT_UP)--古实
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101005, 16, -2, cc.FACE_LEFT_UP)--关伟
    
    local unit_id = cm.team_funcs.get_unit_id_by_config(user_id, 2101024, cc.COMBAT_CAMP_PASSIVE)
    if unit_id then
      cm.log_info("<COMBAT UNIT> unit_id : %d",unit_id)
      cm.capi_funcs.set_unit_ai(user_id,unit_id,2)
    end
  end
  return 0
end
CL.register_level_function(1000025,level_1000025)

--剧情战斗：三次洛阳假战报B   1000027
local function level_1000027(user_id,level_id,first)
  SET_COMBAT_STATUS(user_id, 7, 1)  --不喊话
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)   --清空配角
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101052, 15, -1, cc.FACE_LEFT_UP) --谷月轩 2110052
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101012, 15, 1, cc.FACE_LEFT_UP)    --荆棘 2110012
    
    local unit_id = cm.team_funcs.get_unit_id_by_config(user_id, 2101024, cc.COMBAT_CAMP_PASSIVE)
    if unit_id then
      cm.log_info("<COMBAT UNIT> unit_id : %d",unit_id)
      cm.capi_funcs.set_unit_ai(user_id,unit_id,2)
    end
  end
  return 0
end
CL.register_level_function(1000027,level_1000027)

--剧情战斗：三次洛阳假战报C   1000023
local function level_1000023(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 1)   --清空主角队伍
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101009, 13, -2, cc.FACE_LEFT_UP) --江天雄
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101061, 14, -1, cc.FACE_LEFT_UP) --史纲
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101044, 14, 1, cc.FACE_LEFT_UP)    --卓人清
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2300123, 13, 2, cc.FACE_LEFT_UP)    --关长虹
  else
    if cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_enemy(user_id), 40) then
      SET_CAMP_ACTIVE_WIN(user_id, cc.RESULT_WIN)
      result = 1
    end
  end
  return result
end
CL.register_level_function(1000023,level_1000023)

--剧情战斗：少年英雄会
local function young_heros(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)   --清空配角
  else
    if cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_enemy(user_id), 10) then --血量少于10%后播放一段对话，战斗胜利
      SET_CAMP_ACTIVE_WIN(user_id, cc.RESULT_WIN)
      result = 1
    end
  end
  return result
end
CL.register_level_function(1000048,young_heros) --沈丘山
CL.register_level_function(1100003,young_heros) --西门峰
CL.register_level_function(1000005,young_heros) --关伟
CL.register_level_function(1000030,young_heros) --虚真
CL.register_level_function(1000002,young_heros) --江瑜
CL.register_level_function(1000007,young_heros) --齐丽
CL.register_level_function(1000033,young_heros) --夏侯非
CL.register_level_function(1000022,young_heros) --沈湘芸
CL.register_level_function(1000032,young_heros) --史燕
CL.register_level_function(1000037,young_heros) --巩光杰

--剧情战斗：少年英雄会--沈丘山VS西门峰 决战 #0# 1100048
local function level_1100048(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 1)
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101048, 20, 6, cc.FACE_RIGHT_DOWN)
  end
  return result
end
CL.register_level_function(1100048,level_1100048)

--剧情战斗：少年英雄会之后 洛阳 天剑门--西门峰
local function level_1200003(user_id,level_id,first)
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)
  end
  return 0
end
CL.register_level_function(1200003,level_1200003)

--书信任务：龙井村 &关伟
local function level_1100005(user_id,level_id,first)
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101005, 25, 15, cc.FACE_RIGHT_DOWN)
  end
  return 0
end
CL.register_level_function(1100005,level_1100005)

--书信任务：猎户村 &关伟
local function level_1200005(user_id,level_id,first)
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101005, 26, 4, cc.FACE_LEFT_UP)
  end
  return 0
end
CL.register_level_function(1200005,level_1200005)

--书信任务：成都野外 &关伟
local function level_1300005(user_id,level_id,first)
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101005, 13, 8, cc.FACE_RIGHT_UP)
  end
  return 0
end
CL.register_level_function(1300005,level_1300005)

--书信任务：铸剑山庄 &任剑南
local function level_1100060(user_id,level_id,first)
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101060, 20, 6, cc.FACE_RIGHT_UP)
  end
  return 0
end
CL.register_level_function(1100060,level_1100060)

--书信任务：华山论剑
local function level_1100062(user_id,level_id,first)
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)
  end
  return 0
end
CL.register_level_function(1100062,level_1100062)


--剧情战斗：铸剑山庄--高胜 VS 西门峰  --高胜必负
local function level_1000056(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 1)
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101003, 19, 7, cc.FACE_RIGHT_UP)   --西门峰
    cm.unit_funcs.set_weak(user_id, cm.team_funcs.get_enemy(user_id), 50)
  else
    if IS_COMBAT_ROUND_END(user_id) and GET_COMBAT_ROUND_NUM(user_id) == 3 then
      SET_CAMP_ACTIVE_WIN(user_id, cc.RESULT_WIN) --第3回合结束，判定胜利
      result = 1
    end
  end
  return result
end
CL.register_level_function(1000056,level_1000056)

--剧情战斗：铸剑山庄--丁冲 VS 西门峰  --西门峰必负
local function level_1000031(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 1)
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101031, 18, 6, cc.FACE_LEFT_DOWN)  --丁冲
    cm.unit_funcs.set_weak(user_id, cm.team_funcs.get_enemy(user_id), 50)
  else
    if IS_COMBAT_ROUND_END(user_id) and GET_COMBAT_ROUND_NUM(user_id) == 3 then
      SET_CAMP_ACTIVE_WIN(user_id, cc.RESULT_WIN) --第3回合结束，判定胜利
      result = 1
    end
  end
  return result
end
CL.register_level_function(1000031,level_1000031)

--剧情战斗：铸剑山庄--偷剑贼
local function level_3400603(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101048, 19, 7, cc.FACE_RIGHT_UP) --沈丘山
  end
  return result
end
CL.register_level_function(3400603,level_3400603)

--剧情战斗：京城东厂 主角&小山 VS 黑衣人*4   
local function level_3100603(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101048, 57, 38, cc.FACE_RIGHT_UP)  --小山
  end
  return result
end
CL.register_level_function(3100603,level_3100603)

--剧情战斗：京城东厂 主角&小山 VS 各种东厂守卫   
local function level_2000018(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101048, 57, 38, cc.FACE_RIGHT_UP)
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2300603, 59, 38, cc.FACE_RIGHT_UP)  --神秘人
  end
  return result
end
CL.register_level_function(2000018,level_2000018)

--剧情战斗：主角&荆棘 VS 游进&东厂守卫*3   
local function level_2100010(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101012, 118, 102, cc.FACE_LEFT_DOWN) --荆棘
  end
  return result
end
CL.register_level_function(2100010,level_2100010)

--剧情战斗：一次成都--吃
local function level_1100024(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101048, 34, 24, cc.FACE_LEFT_UP)--加沈丘山
  else
    if cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_enemy(user_id), 50) then --血量少于50%后战斗胜利
      SET_CAMP_ACTIVE_WIN(user_id, cc.RESULT_WIN)
      result = 1
    end
  end
  --------------------------------------------------------------------------------
  return result
end
CL.register_level_function(1100024,level_1100024)

--剧情战斗：青霞子VS紫阳子（AI战斗，青霞子中毒状态，被紫阳子2次攻击杀死）（118） #0# 3000166
local function level_3000166(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 1)
    local unit_id = cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2300527, 36, 3, cc.FACE_LEFT_DOWN)--青霞子
    if unit_id >=0 then
      cm.unit_funcs.set_weak(user_id, unit_id, 50) --青霞子，伤害为50，闪避、暴击、格挡为0
      ADD_UNIT_BUFF(user_id, unit_id, unit_id, 3) --中毒的效果表现 每回合扣除10%生命，最多持续5回合
    end
  end
  return 0
end
CL.register_level_function(3000166,level_3000166)

--剧情战斗：主角&青霞子VS 紫阳子&摩呼罗迦
local function level_3100166(user_id,level_id,first)
  if first==1 then --战斗开始
    --cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)
    cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_ACTIVE, 2300527, 45, -24, cc.FACE_LEFT_UP)--青霞子
  end
  return 0
end
CL.register_level_function(3100166,level_3100166)

--剧情战斗：主角&青霞子VS 黑衣人*2
local function level_3300603(user_id,level_id,first)
  if first==1 then --战斗开始
    --cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE,2)
    cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_ACTIVE, 2300527, 37, 4, cc.FACE_LEFT_UP)--青霞子
  end
  return 0
end
CL.register_level_function(3300603,level_3300603)

--剧情战斗：主角 VS 紫阳子
local function level_3200166(user_id,level_id,first)
  if first==1 then --战斗开始
  
  end
  return 0
end
CL.register_level_function(3200166,level_3200166)

--剧情战斗：主角&荆棘  VS  西门峰&方云华&商鹤鸣&任剑南（116）
local function level_1000043(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101012, 27, -12, cc.FACE_LEFT_UP)--荆棘
  end
  return 0
end
CL.register_level_function(1000043,level_1000043)

--剧情战斗：主角&荆棘  VS  西门玄&方云华&郝一天&任剑南（117）
local function level_3000138(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101012, 40, 0, cc.FACE_LEFT_UP)--荆棘
  end
  return 0
end
CL.register_level_function(3000138,level_3000138)

--剧情战斗：主角&小山 VS黑衣人
local function level_3500603(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101048, 22, -7, cc.FACE_RIGHT_UP)--小山
  end
  return 0
end
CL.register_level_function(3500603,level_3500603)


--剧情战斗：主角VS 黑风寨小兵*2（120）
local function level_2300005(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)
  end
  return 0
end
CL.register_level_function(2300005,level_2300005)

--剧情战斗：谷月轩 VS 荆棘 
local function level_1200012(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE,1)
    cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_ACTIVE, 2101052, 12, 6, cc.FACE_RIGHT_UP)--谷月轩
    cm.unit_funcs.set_weak(user_id, cm.team_funcs.get_enemy(user_id), 100)
  end
  
  if cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_hero(user_id),30) or cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_enemy(user_id),30) then
    SET_CAMP_ACTIVE_WIN(user_id, cc.RESULT_WIN)
    result = 1
  end
  
  return result
end
CL.register_level_function(1200012,level_1200012)

--剧情战斗：谷月轩 VS 荆棘 
local function level_1300012(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE,1)
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101015, 12, 3, cc.FACE_RIGHT_UP)--丹青
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101020, 11, 2, cc.FACE_RIGHT_UP)--书生
    cm.unit_funcs.set_weak(user_id, cm.team_funcs.get_enemy(user_id), 100)
  end
  return result
end
CL.register_level_function(1300012,level_1300012)

--剧情战斗：主角&谷月轩 VS 李三
local function level_2000013(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101052, 73, -17, cc.FACE_RIGHT_UP) --谷月轩
  end
  return 0
end
CL.register_level_function(2000013,level_2000013)

--剧情战斗：主角&小山 VS 算命先生&黑风寨小兵*3 
local function level_3000216(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE,2)
    cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_ACTIVE, 2101048, 38, 13, cc.FACE_RIGHT_DOWN) --小山
    
  else
    if IS_COMBAT_ROUND_END(user_id) then
      if GET_COMBAT_ROUND_NUM(user_id)==10 then
        cm.level_funcs.battle_win(user_id)
      else
        local team = cm.team_funcs.get_team(user_id, cc.COMBAT_CAMP_PASSIVE)
        local deadenemycount = 4 - #team 
        if deadenemycount>1 then
          cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, 2310218, 44, 15, cc.FACE_LEFT_UP)--黑风寨小兵
          cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, 2310218, 44, 13, cc.FACE_LEFT_UP)--黑风寨小兵
        end
      end
    end
  end
  return 0
end
CL.register_level_function(3000216,level_3000216)

--剧情战斗：主角&书生 VS 柳若柏&2个杨柳山庄守卫 （战斗到一半香儿入场帮忙战斗）3000429
local function level_3000429(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE,2)
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101020, 40, 13, cc.FACE_RIGHT_UP)--书生
  else
    if IS_COMBAT_ROUND_END(user_id) and GET_COMBAT_ROUND_NUM(user_id)==3 then
      cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101055, 40, 13, cc.FACE_RIGHT_UP)--香儿
    end
  end
  return 0
end
CL.register_level_function(3000429,level_3000429)

--剧情战斗：主角VS荆棘 1400012
local function level_1400012(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE,2)
  end
  if cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_hero(user_id),50) or cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_enemy(user_id),50) then
    SET_CAMP_ACTIVE_WIN(user_id, cc.RESULT_WIN)
    result = 1
  end
  return result
end
CL.register_level_function(1400012,level_1400012)



--剧情战斗 灵隐寺下4场
local function level_2000011(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101020, 12, 5, cc.FACE_RIGHT_UP) --书生
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101015, 13, 4, cc.FACE_RIGHT_UP) --丹青
  end
  return result
end
CL.register_level_function(2000011,level_2000011)
local function level_2000012(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_ACTIVE, 2101020, 16, 2, cc.FACE_LEFT_UP) --书生
    cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_ACTIVE, 2101015, 16, 4, cc.FACE_LEFT_UP) --丹青
  end
  return result
end
CL.register_level_function(2000012,level_2000012)
local function level_2000014(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_ACTIVE, 2101020, 13, 1, cc.FACE_LEFT_UP) --书生
    cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_ACTIVE, 2101015, 14, 4, cc.FACE_LEFT_UP) --丹青
  end
  return result
end
CL.register_level_function(2000014,level_2000014)
local function level_2000016(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_ACTIVE, 2101020, 16, 3, cc.FACE_LEFT_UP) --书生
    cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_ACTIVE, 2101015, 15, 2, cc.FACE_LEFT_UP) --丹青
  end
  return result
end
CL.register_level_function(2000016,level_2000016)

--明雷战斗 杨柳山庄 --2300427
local function level_3900427(user_id,level_id,first)
  if first == 1 then
    cm.team_funcs.kill_units(user_id,cc.COMBAT_CAMP_PASSIVE,1)
    cm.level_funcs.put_monster_around(user_id, 2300427, cm.team_funcs.get_team_number(user_id, cc.COMBAT_CAMP_ACTIVE)+1)
  end
  return 0
end
CL.register_level_function(3900427,level_3900427)
CL.register_level_function(3910427,level_3900427)

--明雷战斗 杨柳山庄 --2300664
local function level_3900644(user_id,level_id,first)
  if first == 1 then
    cm.team_funcs.kill_units(user_id,cc.COMBAT_CAMP_PASSIVE,1)
    cm.level_funcs.put_monster_around(user_id, 2300664, cm.team_funcs.get_team_number(user_id, cc.COMBAT_CAMP_ACTIVE)+1)
  end
  return 0
end
CL.register_level_function(3900644,level_3900644)
CL.register_level_function(3910644,level_3900644)
CL.register_level_function(3920644,level_3900644)
CL.register_level_function(3930644,level_3900644)

--明雷战斗 杨柳山庄 --2300665
local function level_3900645(user_id,level_id,first)  --该场怪物需要变强
  if first == 1 then
    cm.team_funcs.kill_units(user_id,cc.COMBAT_CAMP_PASSIVE,1)
    cm.level_funcs.put_monster_around(user_id, 2300665, cm.team_funcs.get_team_number(user_id, cc.COMBAT_CAMP_ACTIVE)+1)
    
    cm.team_funcs.set_wake(user_id,cc.COMBAT_CAMP_PASSIVE,400)
  end
  return 0
end
CL.register_level_function(3900645,level_3900645)
CL.register_level_function(3910645,level_3900645)
CL.register_level_function(3920645,level_3900645)
CL.register_level_function(3930645,level_3900645)

--可群攻的两组怪
local function level_3600427(user_id,level_id,first)
  if first == 1 then
    local enemy = cm.team_funcs.get_team(user_id,cc.COMBAT_CAMP_PASSIVE)
    for key in pairs(enemy) do
      cm.capi_funcs.set_unit_ai(user_id,enemy[key],2) --护卫可放技能
    end
  end
  return 0
end
CL.register_level_function(3600427,level_3600427)
local function level_3610427(user_id,level_id,first)
  if first == 1 then
    local enemy = cm.team_funcs.get_team(user_id,cc.COMBAT_CAMP_PASSIVE)
    for key in pairs(enemy) do
      cm.capi_funcs.set_unit_ai(user_id,enemy[key],2) --护卫可放技能
    end
  end
  return 0
end
CL.register_level_function(3610427,level_3610427)

--杨柳山庄 柳若枫+2护法
local function level_3100429(user_id,level_id,first)
  if first == 1 then
    local enemy = cm.team_funcs.get_team(user_id,cc.COMBAT_CAMP_PASSIVE)
    for key in pairs(enemy) do
      cm.capi_funcs.set_unit_ai(user_id,enemy[key],2) --护卫可放技能
    end
  end
  return 0
end
CL.register_level_function(3100429,level_3100429)

--少林寺 对战喇嘛第一场 打不过 3000336
local function level_3000336(user_id,level_id,first)
  local result = 0
  if first==1 then
    cm.team_funcs.set_weak(user_id, cc.COMBAT_CAMP_ACTIVE, 10)  --己方队伍伤害10点
  else
    --主角死亡即失败
    if cm.team_funcs.hero_dead(user_id,1,cc.COMBAT_CAMP_ACTIVE) then
      return cm.level_funcs.battle_lose(user_id)
    end
    
    --主角半血，无因巴拉巴拉
    local hero_id = cm.team_funcs.get_hero(user_id)
    cm.checkValue(user_id,hero_id,-1)
    if cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_hero(user_id), 50) then
      if cm.level_funcs.get_mark(hero_id)==0 then
        --场上加无因，无因说话，无因退出，取消己方变弱限制
        local unit = cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_ACTIVE, 2300338, 51, -7, cc.FACE_LEFT_DOWN)
        cm.level_funcs.show_movie(user_id,6020)
        KILL_COMBAT_UNIT(user_id, unit)
        cm.team_funcs.reset_power(user_id,cc.COMBAT_CAMP_ACTIVE)
        cm.level_funcs.set_mark(user_id,hero_id,1)
      end
    end
  end
  return result
end
CL.register_level_function(3000336,level_3000336)

--剧情战斗 黑风寨 进入 焦大 2300232
local function level_2300232(user_id,level_id,first)
  if first == 1 then
    cm.capi_funcs.set_unit_attr(user_id,cm.team_funcs.get_hero(user_id),cc.FIGHT_ATTR_MOVE,30)
  end
  return 0
end
CL.register_level_function(2300232,level_2300232)
--剧情战斗 黑风寨 进入 焦小 2300233
local function level_2300233(user_id,level_id,first)
  if first == 1 then
    
  end
  return 0
end
CL.register_level_function(2300233,level_2300233)

--剧情战斗 黑风寨 进入 双刀贼 2210004 改为拳头贼
local function level_2210004(user_id,level_id,first)
  if first == 1 then cm.level_funcs.put_monster_around(user_id, 2310218, cm.team_funcs.get_team_number(user_id, cc.COMBAT_CAMP_ACTIVE)+1) end
  return 0
end
CL.register_level_function(2210004,level_2210004)
CL.register_level_function(2211004,level_2210004)
CL.register_level_function(2212004,level_2210004)
CL.register_level_function(2213004,level_2210004)
CL.register_level_function(2214004,level_2210004)

--剧情战斗 黑风寨 进入 刀盾贼 2220005
local function level_2220005(user_id,level_id,first)
  if first == 1 then cm.level_funcs.put_monster_around(user_id, 2220005, cm.team_funcs.get_team_number(user_id, cc.COMBAT_CAMP_ACTIVE)+1) end
  return 0
end
CL.register_level_function(2220005,level_2220005)
CL.register_level_function(2221005,level_2220005)
CL.register_level_function(2222005,level_2220005)

--剧情战斗 黑风寨 进入 面具贼 2310218
local function level_2310218(user_id,level_id,first)
  if first == 1 then cm.level_funcs.put_monster_around(user_id, 2310218, cm.team_funcs.get_team_number(user_id, cc.COMBAT_CAMP_ACTIVE)+1) end
  return 0
end
CL.register_level_function(2310218,level_2310218)
CL.register_level_function(2311218,level_2310218)
CL.register_level_function(2312218,level_2310218)


--剧情战斗 主角、古月轩和小山对玄冥子、荆棘 2101011
local function level_2101011(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2) --清空主角队伍
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101052, 19, 7, cc.FACE_RIGHT_DOWN)  --谷月轩
    cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_ACTIVE, 2101048, 18, 6, cc.FACE_RIGHT_DOWN) --沈丘山
    
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_ACTIVE, 2000070, 19, 9, cc.FACE_RIGHT_DOWN) --无因
  end
  return 0
end
CL.register_level_function(2101011,level_2101011)

local function checkOnBridge(user_id, unit_id)
  local x,y = GET_COMBAT_UNIT_POSITION(unit_id)
  if x > 63 then
    return true
  end
  return false
end
local function anyoneOnBridge(user_id)
    for i=0,9 do
    local unit_id = GET_COMBAT_SIDE(user_id, cc.COMBAT_CAMP_ACTIVE , i)
    if unit_id >= 0 then
      if checkOnBridge(user_id, unit_id) then
        return true
      end
    end
    end
  return false  
end
local function checkPassBridge(user_id) --主角是否通过吊桥
  local unit_id = cm.team_funcs.get_hero(user_id)
  local x,y = GET_COMBAT_UNIT_POSITION(unit_id)
  if x > 81 then
    return true
  end
  return false
end
local function moreEnemy(user_id)
  if anyoneOnBridge(user_id) then
    --吊桥补充怪
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, 2220005, 70, 8, 4)--黑风寨小兵
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, 2220005, 73, 8, 4)--黑风寨小兵
    --吊桥补充怪
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, 2310218, 76, 8, 4)--黑风寨小兵
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, 2310218, 79, 8, 4)--黑风寨小兵
  else
    --门口补充怪
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, 2310218, 41, 8, 3)--黑风寨小兵
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, 2310218, 44, 8, 3)--黑风寨小兵
    --吊桥补充怪
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, 2220005, 49, 7, 4)--黑风寨小兵
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, 2220005, 49, 10, 4)--黑风寨小兵
  end
end

--剧情战斗 黑风寨 脱离
local function level_2500005(user_id,level_id,first)
  local result = -1 --返回0会检测敌方全灭胜利
  --立即触发类条件
  if checkPassBridge(user_id) then  --主角通过吊桥
    cm.level_funcs.show_movie(user_id, 6019)
    SET_CAMP_ACTIVE_WIN(user_id, cc.RESULT_WIN)
    return 1
  end
  if cm.team_funcs.hero_dead(user_id, 1, cc.COMBAT_CAMP_ACTIVE) then  --主角死亡
    SET_CAMP_ACTIVE_WIN(user_id, cc.RESULT_LOSE)
    return 1
  end
  --回合触发类条件
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE,2)
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101052, 41, -1, cc.FACE_RIGHT_DOWN)  --谷月轩 1
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101014, 39, -1, cc.FACE_RIGHT_DOWN)  --三师兄 2
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101012, 42, -2, cc.FACE_RIGHT_DOWN)  --荆棘    3
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, 2220005, 41, 8, cc.FACE_LEFT_UP)   --黑风寨小兵
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, 2220005, 44, 8, cc.FACE_LEFT_UP)   --黑风寨小兵
    cm.level_funcs.show_movie(user_id, 6014)
  elseif IS_COMBAT_ROUND_END(user_id) then --回合结束（下回开始）
    --SET_COMBAT_UNIT_ATTR(user_id, cm.team_funcs.get_hero(user_id), cc.FIGHT_ATTR_MOVE, 200) --测试用，主角移动步数提升
    
    local turn = GET_COMBAT_ROUND_NUM(user_id)
    local team = cm.team_funcs.get_team(user_id, cc.COMBAT_CAMP_PASSIVE)
    local enemycount = #team
    if turn > 2 and enemycount < 3 then
      moreEnemy(user_id)
    end
    if turn==4 then
      cm.level_funcs.show_movie(user_id, 6015)
    end
    if turn==9 then
      cm.level_funcs.show_movie(user_id, 6016)
    end
    if turn==14 then
      cm.level_funcs.show_movie(user_id, 6017)
    end
    if turn==20 then
      cm.level_funcs.show_movie(user_id, 6018)
      SET_CAMP_ACTIVE_WIN(user_id, cc.RESULT_LOSE)--第20回合结束，判定失败
      return 1
    end
  end
  return result
end
CL.register_level_function(2500005,level_2500005)  --  #0# 2500005

--剧情战斗：福州 3100704 小山VS 熊天霸
local function level_3100704(user_id,level_id,first)
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 1)   --清空主角队伍
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101048, 22, -6, cc.FACE_RIGHT_UP)--沈丘山
    cm.unit_funcs.set_weak(user_id,cm.team_funcs.get_enemy(user_id),100)--弱化熊天霸
  end
  return 0
end
CL.register_level_function(3100704,level_3100704)

--剧情战斗：修罗宫 2300516 主角和小山VS 销魂和腾蛇
local function level_2300516(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)   --清空主角队伍（主角除外）
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101048, 13, 6, cc.FACE_RIGHT_UP)--沈丘山
  end
  return 0
end
CL.register_level_function(2300516,level_2300516)

--剧情战斗：修罗宫 2300517 小山VS 销魂
local function level_2300517(user_id,level_id,first)
  SET_COMBAT_STATUS(user_id, 7, 1)  --不喊话
  if first==1 then
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 1)   --清空主角队伍
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101048, 13, 6, cc.FACE_RIGHT_UP)--沈丘山
  end
  return 0
end
CL.register_level_function(2300517,level_2300517)

--剧情战斗：修罗宫 2101063 主角和小山VS 任清璇
local function level_2101063(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)   --清空主角队伍（主角除外）
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101048, 13, 6, cc.FACE_RIGHT_UP)--沈丘山
  end
  return 0
end
CL.register_level_function(2101063,level_2101063)

--剧情战斗：峨眉主殿 主角和小山VS 吃喝嫖赌 1100025
local function level_1100025(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)   --清空主角队伍（主角除外）
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101048, 19, 7, cc.FACE_RIGHT_UP)--沈丘山
  end
  return 0
end
CL.register_level_function(1100025,level_1100025)

--剧情战斗：鞑靼 1100056 主角和小山VS 高胜
local function level_1100056(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)   --清空主角队伍（主角除外）
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101048, 86, 17, cc.FACE_RIGHT_DOWN)--沈丘山
  end
  return 0
end
CL.register_level_function(1100056,level_1100056)

--剧情战斗：洛阳 1000004 主角、傅剑寒vs五岳四龙
local function level_1000004(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)   --清空主角队伍（主角除外）
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101064, 15, 2, cc.FACE_LEFT_UP)--傅剑寒
  end
  return 0
end
CL.register_level_function(1000004,level_1000004)

--剧情战斗：京城擂台 2100011 方云华
local function level_2100011(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 1)   --清空主角队伍
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101006, 101, 29, cc.FACE_LEFT_UP)--方云华
  end
  return 0
end
CL.register_level_function(2100011,level_2100011)

--剧情战斗：京城擂台 2200011 商仲仁
local function level_2200011(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 1)   --清空主角队伍
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101059, 101, 29, cc.FACE_LEFT_UP)--商仲仁
  end
  return 0
end
CL.register_level_function(2200011,level_2200011)

--剧情战斗：京城擂台 2300011 沈丘山
local function level_2200011(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 1)   --清空主角队伍
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101048, 101, 29, cc.FACE_LEFT_UP)--沈丘山
    
    --需要设置沈丘山必胜
    
  end
  return 0
end
CL.register_level_function(2300011,level_2300011)

--剧情战斗：京城擂台 1200048 沈丘山
local function level_2200011(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)   --清空主角队伍，主角除外
  else
    if cm.unit_funcs.hp_less_than_percent(cm.team_funcs.get_enemy(user_id),20) then --hp小于20%
      return cm.level_funcs.battle_win(user_id)
    end
  end
  return 0
end
CL.register_level_function(1200048,level_2300011)

--剧情战斗：福州街巷  2110010 主角、谷月轩、史刚vs游进、李三（3000322）、仇霸
local function level_2110010(user_id,level_id,first)
  if first==1 then --战斗开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_ACTIVE, 2)   --清空主角队伍
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101052, 66, 49, cc.FACE_LEFT_UP)--谷月轩
    cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_ACTIVE, 2101061, 67, 50, cc.FACE_LEFT_UP)--史刚
  end
  return 0
end
CL.register_level_function(2110010,level_2110010)

--剧情战斗：意念战斗 大师兄 1500052
CL.register_level_function(1500052,
function(user_id,level_id,first)
  if first == cc.COMBAT_START then
    cm.team_funcs.kill_units(user_id,cc.COMBAT_CAMP_ACTIVE,2)
    local power = cm.unit_funcs.get_unit_level(cm.team_funcs.get_hero(user_id))
    power = math.max(math.floor(power * 0.85),20)
    cm.level_funcs.set_team_equal(user_id,power)
    --cm.log_info("<COMBAT CHECK> level_id: %d mark: %d", level_id, 1)
  end
  return 0
end)

--剧情战斗：意念战斗 二师兄 1500012
CL.register_level_function(1500012,
function(user_id,level_id,first)
  if first == cc.COMBAT_START then
    cm.team_funcs.kill_units(user_id,cc.COMBAT_CAMP_ACTIVE,2)
    local power = cm.unit_funcs.get_unit_level(cm.team_funcs.get_hero(user_id))
    power = math.max(math.floor(power * 0.8),20)
    cm.level_funcs.set_team_equal(user_id,power)
  end
  return 0
end)

--剧情战斗：意念战斗 三师兄 1500014
CL.register_level_function(1500014,
function(user_id,level_id,first)
  if first == cc.COMBAT_START then
    cm.team_funcs.kill_units(user_id,cc.COMBAT_CAMP_ACTIVE,2)
    local power = cm.unit_funcs.get_unit_level(cm.team_funcs.get_hero(user_id))
    power = math.max(math.floor(power * 0.75),20)
    cm.level_funcs.set_team_equal(user_id,power)
  end
  return 0
end)

--剧情战斗：少林寺铜人
local function level_func_2300785(user_id,level_id,first)
  if first == cc.COMBAT_START then
    cm.level_funcs.put_monster_around(user_id, 2300787, cm.team_funcs.get_team_number(user_id, cc.COMBAT_CAMP_ACTIVE)+1)
  end
  return 0
end
CL.register_level_function(2310785,level_func_2300785)
CL.register_level_function(2320785,level_func_2300785)
CL.register_level_function(2330785,level_func_2300785)
CL.register_level_function(2340785,level_func_2300785)
CL.register_level_function(2350785,level_func_2300785)
CL.register_level_function(2360785,level_func_2300785)

--剧情战斗：少林寺铜人
local function level_func_2300787(user_id,level_id,first)
  if first == cc.COMBAT_START then
    cm.level_funcs.put_monster_around(user_id, 2300787, cm.team_funcs.get_team_number(user_id, cc.COMBAT_CAMP_ACTIVE)+1)
  end
  return 0
end
CL.register_level_function(2310787,level_func_2300787)
CL.register_level_function(2320787,level_func_2300787)
CL.register_level_function(2330787,level_func_2300787)
CL.register_level_function(2340787,level_func_2300787)

--剧情战斗：少林寺铜人 2300786  怪物无法移动
CL.register_level_function(2300786,
function(user_id,level_id,first)
  if first == cc.COMBAT_START then
    local team = cm.team_funcs.get_team(user_id,cc.COMBAT_CAMP_PASSIVE)
    for key in pairs(team) do
      --cm.checkValue(user_id,team[key],-1)
      cm.capi_funcs.add_unit_buff(user_id,team[key],team[key],1199)
      --cm.capi_funcs.set_unit_attr(user_id,team[key],cc.FIGHT_ATTR_MOVE,0)
    end
  end
  return 0
end)

--剧情战斗：少林寺铜人 2300784  不断刷怪
CL.register_level_function(2300784,
function(user_id,level_id,first)
  if first == cc.COMBAT_START then
    --cm.level_funcs.set_team_equal(user_id,100)  --测试用，强度拉平
  else
    local boss = cm.team_funcs.get_unit_id_by_config(user_id,2300784,cc.COMBAT_CAMP_PASSIVE)
    if boss then
      local mark = cm.level_funcs.get_mark(boss)
      --cm.log_info("<COMBAT BOSS> unit_id:%d mark:%d",boss,mark)
      if mark==0 and cm.unit_funcs.hp_less_than_percent(boss,90) then
        cm.level_funcs.set_mark(user_id,boss,1)
        --cm.log_info("<COMBAT MARK> mark:0 hp:90%")
        --补充怪
        cm.level_funcs.put_monster_around(user_id, 2300787, 1)
      end
      if mark==1 and cm.unit_funcs.hp_less_than_percent(boss,70) then
        cm.level_funcs.set_mark(user_id,boss,2)
        --cm.log_info("<COMBAT MARK> mark:0 hp:70%")
        --补充怪
        cm.level_funcs.put_monster_around(user_id, 2300787, 1)
        cm.level_funcs.put_monster_around(user_id, 2300785, 1)
      end
      if mark==2 and cm.unit_funcs.hp_less_than_percent(boss,60) then
        cm.level_funcs.set_mark(user_id,boss,3)
        --cm.log_info("<COMBAT MARK> mark:0 hp:60%")
        --补充怪
        cm.level_funcs.put_monster_around(user_id, 2300787, 1)
        cm.level_funcs.put_monster_around(user_id, 2300785, 1)
        cm.level_funcs.put_monster_around(user_id, 2300786, 1)
      end
      if mark==3 and cm.unit_funcs.hp_less_than_percent(boss,50) then
        cm.level_funcs.set_mark(user_id,boss,4)
        --cm.log_info("<COMBAT MARK> mark:0 hp:50%")
        --补充怪
        cm.level_funcs.put_monster_around(user_id, 2300787, 1)
        cm.level_funcs.put_monster_around(user_id, 2300785, 2)
        cm.level_funcs.put_monster_around(user_id, 2300786, 1)
      end
      if mark==4 and cm.unit_funcs.hp_less_than_percent(boss,30) then
        cm.level_funcs.set_mark(user_id,boss,5)
        --cm.log_info("<COMBAT MARK> mark:0 hp:30%")
        --补充怪
        cm.level_funcs.put_monster_around(user_id, 2300787, 1)
        cm.level_funcs.put_monster_around(user_id, 2300785, 2)
        cm.level_funcs.put_monster_around(user_id, 2300786, 2)
      end
    else
      return cm.level_funcs.battle_win(user_id)
    end
  end
  return 0
end)

--武当关卡-普通房
local function level_func_2300343(user_id,level_id,first)
  if first == cc.COMBAT_START then
    cm.level_funcs.put_monster_around(user_id, 2300343, cm.team_funcs.get_team_number(user_id, cc.COMBAT_CAMP_ACTIVE)+1)
  end
  return 0
end
CL.register_level_function(2300343,level_func_2300343)
CL.register_level_function(2310343,level_func_2300343)
CL.register_level_function(2320343,level_func_2300343)
CL.register_level_function(2330343,level_func_2300343)
CL.register_level_function(2340343,level_func_2300343)

--武当关卡-宝藏房
local function level_func_2300985(user_id,level_id,first)
  if first == cc.COMBAT_START then
    cm.level_funcs.put_monster_around(user_id, 2300985, cm.team_funcs.get_team_number(user_id, cc.COMBAT_CAMP_ACTIVE)+1)
  end
  return 0
end
CL.register_level_function(2300985,level_func_2300985)
CL.register_level_function(2310985,level_func_2300985)
CL.register_level_function(2320985,level_func_2300985)
CL.register_level_function(2330985,level_func_2300985)
CL.register_level_function(2340985,level_func_2300985)
CL.register_level_function(2350985,level_func_2300985)

----------------------------------------------------------------------------------------------------------------------------------------
--特殊关卡
----------------------------------------------------------------------------------------------------------------------------------------
--世界地图
local function addRandomMonster(user_id, unitLevel, x,y,face)
  local mon_id = 100*unitLevel + math.random(1,6)*10
  cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, mon_id, x, y, face)
end

local function level_1010(user_id,level_id,first)
  if first==1 then
    --清空配置表怪物
    cm.team_funcs.kill_units(user_id,cc.COMBAT_CAMP_PASSIVE,1)
    --获取主角等级
    local unit_level = 10
    --至少1个敌人
    addRandomMonster(user_id, unit_level, 11, 1, 2)
    --补充敌人
    local teammember = cm.team_funcs.get_team_number(user_id, cc.COMBAT_CAMP_ACTIVE)
    if teammember > 1 then addRandomMonster(user_id, unit_level, 10, 2, 2) end
    if teammember > 2 then addRandomMonster(user_id, unit_level, 10, 0, 2) end
    if teammember > 3 then addRandomMonster(user_id, unit_level, 9, -1, 2) end
    if teammember > 4 then addRandomMonster(user_id, unit_level, 8, -2, 2) end
  end
  return 0
end
CL.register_level_function(1010,level_1010)

local function level_1020(user_id,level_id,first)
  if first==1 then
    --补充敌人
    local teammember = cm.team_funcs.get_team_number(user_id, cc.COMBAT_CAMP_ACTIVE)
    if teammember > 1 then cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2200009, 10, 2, 2) end
    if teammember > 2 then cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2200009, 10, 0, 2) end
    if teammember > 3 then cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2200009, 9, -1, 2) end
    if teammember > 4 then cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2200009, 8, -2, 2) end
    --修正强度
    cm.team_funcs.adjust_monster_power(user_id,35,100,5)
  end
  return 0
end
CL.register_level_function(1020,level_1020)
local function level_1030(user_id,level_id,first)
  if first==1 then
    --补充敌人
    local teammember = cm.team_funcs.get_team_number(user_id, cc.COMBAT_CAMP_ACTIVE)
    if teammember > 1 then cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2310603, 10, 2, 2) end
    if teammember > 2 then cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2310603, 10, 0, 2) end
    if teammember > 3 then cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2310603, 9, -1, 2) end
    if teammember > 4 then cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2310603, 8, -2, 2) end
    --修正强度
    cm.team_funcs.adjust_monster_power(user_id,40,100,5)
  end
  return 0
end
CL.register_level_function(1030,level_1030)

--PVP地图PVE战斗
local function add_random_npc(user_id, side, x, y, d)
  local count = 1
  while count < cc.npcUnitsNum do
    local config_id = cc.npcUnits[math.random(1, cc.npcUnitsNum)]
    if cm.team_funcs.hero_dead(user_id, config_id, cc.COMBAT_CAMP_PASSIVE) then --尚未添加
      local unit_id = cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, config_id, x, y, d)
      if unit_id >= 0 then return end
    end
  end
end
local function level_101(user_id,level_id,first)
  if first==1 then --战斗刚开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_PASSIVE, 1)
    add_random_npc(user_id, cc.COMBAT_CAMP_PASSIVE, 16, 14, 4)
    add_random_npc(user_id, cc.COMBAT_CAMP_PASSIVE, 16, 15, 4)
    add_random_npc(user_id, cc.COMBAT_CAMP_PASSIVE, 14, 15, 4)
    add_random_npc(user_id, cc.COMBAT_CAMP_PASSIVE, 14, 16, 4)
    add_random_npc(user_id, cc.COMBAT_CAMP_PASSIVE, 12, 16, 4)
  end
  return 0
end
--CL.register_level_function(101, level_101)

---钓鱼关卡
local function addRandomFish(user_id, x,y,face)
  local mon_id = 2300623 + math.random(0,2)
  cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, mon_id, x, y, face)
end
--洛阳
local function level_100001(user_id,level_id,first)
  if first==1 then --战斗刚开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_PASSIVE, 1)
    addRandomFish(user_id, 11,1,2)
  end
  return 0
end
CL.register_level_function(100001, level_100001)
--成都
local function level_100002(user_id,level_id,first)
  if first==1 then --战斗刚开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_PASSIVE, 1)
    addRandomFish(user_id, 11,1,2)
  end
  return 0
end
CL.register_level_function(100002, level_100002)
--杭州
local function level_100003(user_id,level_id,first)
  if first==1 then --战斗刚开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_PASSIVE, 1)
    addRandomFish(user_id, 11,1,2)
  end
  return 0
end
CL.register_level_function(100003, level_100003)
--鞑靼
local function level_100004(user_id,level_id,first)
  if first==1 then --战斗刚开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_PASSIVE, 1)
    addRandomFish(user_id, 11,1,2)
  end
  return 0
end
CL.register_level_function(100004, level_100004)
--京城
local function level_100005(user_id,level_id,first)
  if first==1 then --战斗刚开始
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_PASSIVE, 1)
    addRandomFish(user_id, 11,1,2)
  end
  return 0
end
CL.register_level_function(100005, level_100005)



local function add_npc_by_HH(user_id, x, y, d)
  local timePoint = GET_ROLE_TIME(user_id)
  local n = math.floor(timePoint/10) % cc.npcUnitsNum + 1
  local config_id = cc.npcUnits[n]
  cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, config_id, x, y, d)
end
local function add_mon_by_HH(user_id, x, y, d)
  local timePoint = GET_ROLE_TIME(user_id)
  local n = math.floor(timePoint/10) % cc.monUnitsNum + 1
  local config_id = cc.monUnits[n]
  cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, config_id, x, y, d)
end


--功能测试用关卡
local function level_20(user_id,level_id,first)
  --玩法逻辑
  if first==1 then --战斗刚开始
    --KILL_COMBAT_UNIT(user_id, cm.team_funcs.get_hero(user_id))  --清除主角
    cm.team_funcs.kill_units(user_id, cc.COMBAT_CAMP_PASSIVE, 1) --清除原有敌方单位
    
    --ai checked
    
    --五岳四龙
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101004, 12, 3, 2) --贾云长
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 1000049, 12, 3, 2) --雷震天 NOT AI CHECKED
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 1000050, 12, 3, 2) --赛飞鸿 NOT AI CHECKED
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 1000051, 12, 3, 2) --赵惊风 NOT AI CHECKED
    
    --琴棋书画
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101001, 12, 3, 2) --仙音
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101017, 12, 3, 2) --橘叟
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101020, 12, 3, 2) --书生
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101015, 12, 3, 2) --丹青
    
    --茶酒花佛
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101045, 12, 3, 2) --古实
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101021, 12, 3, 2) --醉仙
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101016, 12, 3, 2) --花翁
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101030, 12, 3, 2) --虚真
    
    --四萝莉
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101063, 12, 3, 2) --任清璇
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101007, 12, 3, 2) --齐丽
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101032, 12, 3, 2) --史燕
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101022, 12, 3, 2) --沈湘芸
    
    --四御姐
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101053, 12, 3, 2) --蓝婷
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101042, 12, 3, 2) --纪纹
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101001, 12, 3, 2) --仙音
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101055, 12, 3, 2) --香儿
    
    --四剑客
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101064, 12, 3, 2) --傅剑寒
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101031, 12, 3, 2) --丁冲
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101003, 12, 3, 2) --西门峰  天剑门
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101006, 12, 3, 2) --方云华
    
    --四刀客
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101048, 12, 3, 2) --沈丘山
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101005, 12, 3, 2) --关伟       长虹镖局
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101033, 12, 3, 2) --夏侯非  绝刀门
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101059, 12, 3, 2) --商仲仁  八卦门
    
    --四公子
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101060, 12, 3, 2) --任剑南  铸剑山庄
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101068, 12, 3, 2) --万凯       万兽山庄
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101039, 12, 3, 2) --唐冠南  唐门
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101002, 12, 3, 2) --江瑜       江府
    
    --四大国手
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101018, 12, 3, 2) --神医
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101019, 12, 3, 2) --怪医
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101037, 12, 3, 2) --巩光杰
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101022, 12, 3, 2) --沈湘芸
    
    --四大散人
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101057, 12, 3, 2) --叶孤
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101058, 12, 3, 2) --萧遥
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101056, 12, 3, 2) --高胜
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101069, 12, 3, 2) --杨云
    
    --天龙四凶
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101024, 12, 3, 2) --吃
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101025, 12, 3, 2) --喝
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101026, 12, 3, 2) --嫖
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101027, 12, 3, 2) --赌
    
    --逍遥师徒
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101010, 12, 3, 2) --无暇子
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101052, 12, 3, 2) --谷月轩
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101012, 12, 3, 2) --荆棘
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101014, 12, 3, 2) --三师兄
    
    --cm.capi_funcs.set_unit_ai(user_id,unit_id,30)
    
    --CHANGE_COMBAT_NPC_AI(user_id, get_enemy(user_id), 1)
    --CHANGE_COMBAT_NPC_AI(user_id, get_enemy(user_id), 2)
    --CHANGE_COMBAT_NPC_AI(user_id, get_enemy(user_id), 3)
    --CHANGE_COMBAT_NPC_AI(user_id, get_enemy(user_id), 3)
    --CHANGE_COMBAT_NPC_AI(user_id, get_enemy(user_id), 10)
    --CHANGE_COMBAT_NPC_AI(user_id, get_enemy(user_id), 20)
    --CHANGE_COMBAT_NPC_AI(user_id, get_enemy(user_id), 30)
    --CHANGE_COMBAT_NPC_AI(user_id, get_enemy(user_id), 40)
    
    --cm.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, 2101024, 12, 3, cc.FACE_RIGHT_DOWN)
    --CHANGE_COMBAT_NPC_AI(user_id, cm.team_funcs.get_enemy(user_id), cc.AI_MODE_SKILL_1)
    
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2000003, 12, 3, 2) --敌方
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2000003, 12, 3, 2) --敌方
    
    --add_npc_by_HH(user_id, 12, 3, 2)
    --add_mon_by_HH(user_id, 12, 3, 2)
    
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2300130, 12, 4, 2) --银钩
    --local unit_id = cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2300520, 12, 2, 2) --血蟾
    --CHANGE_COMBAT_NPC_AI(user_id, unit_id, cc.AI_MODE_SKILL_1)
    
    
    --血牛
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_ACTIVE, 2000003, 10, 3, 4) --己方
    cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2000003, 12, 3, 2) --敌方
    --闪避怪
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2000010, 10, 3, 2) 
    --机关
    --ADD_COMBAT_TRAP(user_id, cc.COMBAT_CAMP_PASSIVE, 2, 12, 5, 1)
    
    
    --怪物
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2300564, 12, 4, 2) --巴龙
    --cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2300565, 12, 4, 2) --郝虎
    
    --local mon_id = cm.team_funcs.add_unit(user_id,  cc.COMBAT_CAMP_PASSIVE, 2101015, 12, 3, 2) --丹青
    --CHANGE_COMBAT_NPC_AI(user_id, get_enemy(mon_id), 30)
    
    
    --cm.level_funcs.set_team_equal(user_id,100)
    
    --cm.team_funcs.recover_team(user_id, cc.COMBAT_CAMP_ACTIVE, 100, 100)
    local hero_id = cm.team_funcs.get_hero(user_id)
    
    --ADD_UNIT_BUFF(user_id, hero_id, hero_id, 141001)  --闪避+9999 3回合
    --cm.capi_funcs.set_unit_attr(user_id,hero_id,cc.FIGHT_ATTR_HP,5000)
    --cm.capi_funcs.set_unit_attr(user_id,hero_id,cc.FIGHT_ATTR_MP,5000)
    
    --cm.log_info("<COMBAT CHECK> level_id: %d mark: %d", level_id, 1)
  else
    if IS_COMBAT_ROUND_END(user_id) and GET_COMBAT_ROUND_NUM(user_id) >= 99 then
      cm.level_funcs.quit_battle(user_id)
    end
  end
  return 0
end
CL.register_level_function(20,level_20)



return CL



