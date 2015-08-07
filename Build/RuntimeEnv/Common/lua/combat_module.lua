
local cc = require("combat_const")
local CM = {}

CM.level_funcs={
  --默认胜负处理
  default_win = function(user_id)
    if CM.team_funcs.team_has_no_unit(user_id, cc.COMBAT_CAMP_PASSIVE) then
      CM.level_funcs.battle_win(user_id)
    elseif CM.team_funcs.team_has_no_unit(user_id, cc.COMBAT_CAMP_ACTIVE) then
      CM.level_funcs.battle_lose(user_id)
    end
  end,
  --位置是否可用
  pos_can_stand = function(user_id, x, y)           
    if GET_BATTLEFIELD_POS_BLOCK(user_id, x, y) == 1 then
      local unit_id = GET_COMBAT_UNIT_BY_POS(user_id, x, y)
      --CM.log_info("<COMBAT POS CHECK> unit_id: %d, x: %d, y: %d", unit_id,x,y)
      if unit_id < 0 then return true end
    end
    return false
  end,
  --获得可用位置
  get_free_pos = function(user_id, x, y)
    if CM.level_funcs.pos_can_stand(user_id, x, y) then return true, x, y end
    if CM.level_funcs.pos_can_stand(user_id, x+1, y) then return true, x+1, y end
    if CM.level_funcs.pos_can_stand(user_id, x-1, y) then return true, x-1, y end
    if CM.level_funcs.pos_can_stand(user_id, x, y+1) then return true, x, y+1 end
    if CM.level_funcs.pos_can_stand(user_id, x, y-1) then return true, x, y-1 end
    if CM.level_funcs.pos_can_stand(user_id, x+1, y+1) then return true, x+1, y+1 end
    if CM.level_funcs.pos_can_stand(user_id, x+1, y-1) then return true, x+1, y-1 end
    if CM.level_funcs.pos_can_stand(user_id, x-1, y+1) then return true, x-1, y+1 end
    if CM.level_funcs.pos_can_stand(user_id, x-1, y-1) then return true, x-1, y-1 end
    return false, x, y
  end,
  --获得周围敌人
  get_enemy_around = function(user_id, unit_id, x, y)     
    local targets = GET_AREA_TARGET_UNIT(user_id, x, y, cc.AREA_AROUND, CM.unit_funcs.get_enemy_camp(user_id, unit_id), 1)
    return #targets, targets
  end,
  --播放剧情
  show_movie = function(user_id, movie_id)          
    MOVIE_ACTION(user_id, movie_id * 1000+1,0,1)
  end,
  --一直线无地形阻挡
  can_move = function(user_id, startx, starty, dx, dy, dis) 
    --CM.log_info("<MOVE CHECK> x:%d y:%d", startx, starty)
    for i =0, dis do
      local x = startx + dx*i
      local y = starty + dy*i
      if GET_BATTLEFIELD_POS_BLOCK(user_id, x, y) > 1 then return false end
    end
    return true
  end,
  --立即敌方全灭
  quit_battle = function(user_id)
    for i=0,9 do
      local unit = GET_COMBAT_SIDE(user_id, cc.COMBAT_CAMP_PASSIVE, i)
      if unit>=0 then
        SET_COMBAT_UNIT_ATTR(user_id, unit, 3, 0)
      end
    end
  end,
  --判断当前战斗是否PVP
  is_PVP = function(user_id)
    local ai1 = GET_COMBAT_UNIT_AI(CM.team_funcs.get_hero(user_id))
    local ai1 = GET_COMBAT_UNIT_AI(CM.team_funcs.get_enemy(user_id))
    if (ai1 + ai2)==0 then return true end
    return false
  end,
  --获得标记
  get_mark = function(unit_id)                
    return GET_COMBAT_UNIT_ATTR(unit_id, cc.LEVEL_MARK_ATTR)
  end,
  --设置标记
  set_mark = function(user_id, unit_id, value)        
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.LEVEL_MARK_ATTR, value)
  end,
  --标记加值
  add_mark = function(user_id,unit_id, added)         
    local t = GET_COMBAT_UNIT_ATTR(unit_id, cc.LEVEL_MARK_ATTR)
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.LEVEL_MARK_ATTR, t + added)
  end,
  --获取游戏当前年月日时
  get_role_date = function(user_id)
    local timePoint = GET_ROLE_TIME(user_id)  --此处是游戏进度时间，而非强度时间，所以不用startTime修正
    --local timePoint = CM.capi_funcs.get_role_time(user_id)
    local y = timePoint / cc.TP2YY
    local YY = math.floor(y)
    local m = (timePoint - YY*cc.TP2YY) / cc.TP2MM
    local MM = math.floor(m)
    local d = (timePoint - YY*cc.TP2YY - MM*cc.TP2MM) / cc.TP2DD
    local DD = math.floor(d)
    local h = (timePoint - YY*cc.TP2YY - MM*cc.TP2MM - DD*cc.TP2DD) / cc.TP2HH
    local HH = math.floor(h)
    return YY+1,MM+1,DD+1,HH+1
  end,
  --布怪并计数
  put_monster = function (user_id, x, y, monster_id, face)
    CM.log_info("<COMBAT MON> mon_id:%d", monster_id)
    local xok, xx, yy = CM.level_funcs.get_free_pos(user_id, x, y) 
    if xok then
      local unit_id = CM.team_funcs.add_unit(user_id, cc.COMBAT_CAMP_PASSIVE, monster_id, xx, yy, face)
      return 1
    else
      return 0
    end
  end,
  --八向加怪
  put_monster_around = function(user_id, mon_id, maxcount)  
    local unit_id = CM.team_funcs.get_hero(user_id)
    if unit_id >= 0 then
      local x,y = GET_COMBAT_UNIT_POSITION(unit_id) --中心位置
      local count = 0
      if CM.level_funcs.can_move(user_id, x, y, 1, 0, 3)  and count<maxcount  then count = count + CM.level_funcs.put_monster(user_id, x + 2, y, mon_id, 4) end
      if CM.level_funcs.can_move(user_id, x, y, -1, 0, 3) and count<maxcount  then count = count + CM.level_funcs.put_monster(user_id, x - 2, y, mon_id, 2) end
      if CM.level_funcs.can_move(user_id, x, y, 0, 1, 3) and count<maxcount   then count = count + CM.level_funcs.put_monster(user_id, x, y + 2, mon_id, 1) end
      if CM.level_funcs.can_move(user_id, x, y, 0, -1, 3) and count<maxcount  then count = count + CM.level_funcs.put_monster(user_id, x, y - 2, mon_id, 3) end
      if CM.level_funcs.can_move(user_id, x, y, 1, -1, 2) and count<maxcount  then count = count + CM.level_funcs.put_monster(user_id, x + 2, y - 2, mon_id, 3) end
      if CM.level_funcs.can_move(user_id, x, y, 1, 1, 2) and count<maxcount   then count = count + CM.level_funcs.put_monster(user_id, x + 2, y + 2, mon_id, 1) end
      if CM.level_funcs.can_move(user_id, x, y, -1, 1, 2) and count<maxcount  then count = count + CM.level_funcs.put_monster(user_id, x - 2, y + 2, mon_id, 1) end
      if CM.level_funcs.can_move(user_id, x, y, -1, -1, 2) and count<maxcount then count = count + CM.level_funcs.put_monster(user_id, x - 2, y - 2, mon_id, 3) end
      if CM.level_funcs.can_move(user_id, x, y, 1, 0, 2)  and count<maxcount  then count = count + CM.level_funcs.put_monster(user_id, x + 2, y, mon_id, 4) end
      if CM.level_funcs.can_move(user_id, x, y, -1, 0, 2) and count<maxcount  then count = count + CM.level_funcs.put_monster(user_id, x - 2, y, mon_id, 2) end
      if CM.level_funcs.can_move(user_id, x, y, 0, 1, 2) and count<maxcount   then count = count + CM.level_funcs.put_monster(user_id, x, y + 2, mon_id, 1) end
      if CM.level_funcs.can_move(user_id, x, y, 0, -1, 2) and count<maxcount  then count = count + CM.level_funcs.put_monster(user_id, x, y - 2, mon_id, 3) end
      if CM.level_funcs.can_move(user_id, x, y, 1, -1, 1) and count<maxcount  then count = count + CM.level_funcs.put_monster(user_id, x + 2, y - 2, mon_id, 3) end
      if CM.level_funcs.can_move(user_id, x, y, 1, 1, 1) and count<maxcount   then count = count + CM.level_funcs.put_monster(user_id, x + 2, y + 2, mon_id, 1) end
      if CM.level_funcs.can_move(user_id, x, y, -1, 1, 1) and count<maxcount  then count = count + CM.level_funcs.put_monster(user_id, x - 2, y + 2, mon_id, 1) end
      if CM.level_funcs.can_move(user_id, x, y, -1, -1, 1) and count<maxcount then count = count + CM.level_funcs.put_monster(user_id, x - 2, y - 2, mon_id, 3) end
    end
  end,
  --战场陷阱数量
  get_trap_num = function(user_id)              
    local trap_num = 0
    local traps = GET_COMBAT_TRAP_INFO(user_id)
    for key in pairs(traps) do
      trap_num = trap_num +1
    end
    return trap_num
  end,
  --距离价值
  pos_value = function(m_x,m_y,t_x,t_y)
    local max_value = 1 --用于修正目标值（如combat_value）的倍数
    if m_x==t_x and m_y==t_y then
      max_value = 10
    else
      local x_off = math.abs(m_x-t_x)
      local y_off = math.abs(m_y-t_y)
      max_value = max_value / (x_off + y_off) --差2步的位置为倍数1
    end
    return max_value
  end,
  --单位所在位置判断
  check_pos = function(unit_id, t_x, t_y)
    local x,y = GET_COMBAT_UNIT_POSITION(unit_id)
    if x == t_x and y == t_y then
      return true
    end
    return false
  end,
  --战斗胜利
  battle_win = function(user_id)
    SET_CAMP_ACTIVE_WIN(user_id, 1)
    CM.log_info("<COMBAT RESULT WIN> user_id: %d", user_id)
    return 1
  end,
  --战斗失败
  battle_lose = function(user_id)
    SET_CAMP_ACTIVE_WIN(user_id, 0)
    CM.log_info("<COMBAT RESULT LOS> user_id: %d", user_id)
    return 1
  end,
  set_team_equal = function(user_id, power)
  local unit_id
  for i=0,9 do
    unit_id = GET_COMBAT_SIDE(user_id, cc.COMBAT_CAMP_ACTIVE, i)
    if unit_id>=0 then
      CM.unit_funcs.set_attrs(user_id,unit_id,{power,power,power,power,power,power})
      CM.capi_funcs.set_unit_attr(user_id, unit_id, cc.FIGHT_ATTR_MOVE, 20)
    end
    unit_id = GET_COMBAT_SIDE(user_id, cc.COMBAT_CAMP_PASSIVE, i)
    if unit_id>=0 then
      CM.unit_funcs.set_attrs(user_id,unit_id,{power,power,power,power,power,power})
      CM.capi_funcs.set_unit_attr(user_id, unit_id, cc.FIGHT_ATTR_MOVE, 20)
    end
  end
end
  
  --GET_COMBAT_STATUS
  --SET_COMBAT_STATUS(uin, status_type, is_status_set)

}

CM.unit_funcs={
  --单位回血
  add_hp_percent = function(user_id, unit_id, arg)      
    local hp = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_HP)
    local mhp = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_HPMAX)
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_HP, hp + mhp * arg / 100)
  end,
  --单位回蓝
  add_mp_percent = function(user_id, unit_id, arg)      
    local mp = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_MP)
    local mmp = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_MPMAX)
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_MP, mp + mmp * arg / 100)
  end,
  --单位虚弱（用于指定战败）
  set_weak = function(user_id, unit_id, dmg)          
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_DMG, dmg)     --调整伤害值
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_AVO, 0)     --闪避为0
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_CRI, 0)     --暴击为0
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_PAR, 0)     --招架为0
  end,
  --单位强壮（用于增强难度）
  set_wake = function(user_id, unit_id, dmg)          
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_DMG, dmg)     --调整伤害值
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_PAR, 9999)     --招架为9999
  end,
  --刷新属性（指定一级属性刷新全部属性）
  set_attrs = function(user_id, unit_id, attrs)       
    local attr_1 = attrs[1] or GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_STR)
    local attr_2 = attrs[2] or GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_DEX)
    local attr_3 = attrs[3] or GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_PWR)
    local attr_4 = attrs[4] or GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_EYE)
    local attr_5 = attrs[5] or GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_SPD)
    local attr_6 = attrs[6] or GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_EAR)
    --能力
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_STR, attr_1)
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_DEX, attr_2)
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_PWR, attr_3)
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_EYE, attr_4)
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_SPD, attr_5)
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_EAR, attr_6)
    --消耗
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_HPMAX, attr_1 * 30) --maxhp
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_HP, attr_1 * 30)    --hp
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_MPMAX, attr_3 * 20) --maxmp
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_MP, attr_3 * 20)    --mp
    --强度
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_ATK, attr_1 * 6)            --攻击
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_DEF, attr_1 * 6)            --防御
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_HIT, attr_4 * 6)            --命中
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_AVO, attr_4 * 6)            --闪避
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_CRI, attr_2 * 6)            --暴击
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_TOU, attr_2 * 6)            --坚韧
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_ACC, attr_3 * 6)            --精准
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_PAR, attr_3 * 6)            --格挡
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_DMG, attr_1 + attr_2 + attr_3 + attr_4 + attr_5 + attr_6) --伤害
    
    CM.unit_funcs.set_move(user_id, unit_id)
  end,
  --根据当前轻功值更新移动距离
  set_move = function(user_id, unit_id)
    local speed = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_SPD)
    local move_add = 0
    if speed >= 30 then move_add = 10 end
    if speed >= 60 then move_add = 20 end
    if speed >= 90 then move_add = 30 end
    local basemove = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_MOVE)
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_MOVE, basemove + move_add)
  end,
  --重置单位强度（用于取消setweak的效果）
  reset_power = function(user_id, unit_id)
    local attrs = {}
    for i=1,6 do
      attrs[i] = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_STR + i-1)
    end
    CM.unit_funcs.set_attrs(user_id, unit_id, attrs)
  end,
  --根据游戏时间设置单位成长后的强度
  set_growth = function(user_id, unit_id, time_offset)    
    local config_id = GET_COMBAT_UNIT_EXTRAATTR(user_id, unit_id, cc.EXTRAATTR_CONFIG_ID)
    --LOG_INFO(string.format("<SET UNIT GROWTH> config_id: %d", config_id))
    
    local maxpower = cc.hero_power[config_id]
    if maxpower then
      local time_adjust = math.min(1,(CM.capi_funcs.get_role_time(user_id) + time_offset) / (5 * cc.TP2YY)) --这里假设5年可以从初始值修炼到最终值
      local attrs = {}
      for i=1,6 do
        local attr = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_STR + i-1)
        attrs[i] = attr + (maxpower[i] - attr) * time_adjust
        --LOG_INFO(string.format("<ATTR %d> base %d max %d time %f", i, attr, maxpower[i], time_adjust))
      end
      CM.unit_funcs.set_attrs(user_id, unit_id, attrs)
    else
      --不是群侠的情况
      local szhan = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_SZHAN)
      if szhan then
        if szhan > 0 then --通用怪物强度（非固定强度）
          --CM.log_info("<SET UNIT GROWTH> szhan: %d", szhan)
          local weapontype = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_WUXUE)
          if weapontype == 0 then weapontype = 4 end  --默认按徒手类型算
          local monsterpower = cc.hero_power[szhan*100 + weapontype*10]
          if monsterpower then
            local time_adjust = math.min(1,(CM.capi_funcs.get_role_time(user_id) + time_offset) / (5 * cc.TP2YY)) --这里假设5年可以从初始值修炼到最终值
            local attrs = {}
            for i=1,6 do
              local attr = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_STR + i-1)
              attrs[i] = attr + (monsterpower[i] - attr) * time_adjust
            end
            CM.unit_funcs.set_attrs(user_id, unit_id, attrs)
          end
        end
        --怪物实战只用来作为通用强度调用，在使用后全部设为0，否则影响命中
        CM.capi_funcs.set_unit_attr(user_id,unit_id,cc.FIGHT_ATTR_SZHAN,0)
      end
      
    end
  end,
  --初始化单位回合AI判断
  init_ai = function(user_id, unit_id)            
    ADD_UNIT_BUFF(user_id, unit_id, unit_id, 1000)
  end,
  --获得敌方阵营
  get_enemy_camp = function(user_id, unit_id)         
    local camp_type = GET_COMBAT_UNIT_CAMP(user_id, unit_id)
    if camp_type == 1 then return 2 end
    return 1
  end,
  --生命低于百分比（包括不在场上，如死亡）
  hp_less_than_percent = function(unit_id, arg)       
    if unit_id>=0 then
      local hp = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_HP)
      local maxhp = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_HPMAX)
      if hp/maxhp >= arg/100 then return false end
    end
    return true --血量低于arg/100或该单位不存在（比如死亡）
  end,
  --获得单位等级评估
  get_unit_level = function(unit_id)
    local attr_sum = 0
    for i=1,6 do
      local attr = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_STR + i-1)
      attr_sum = attr_sum + attr
    end
    return math.ceil(attr_sum / 6)
  end,
  
  
}

CM.team_funcs={
  --获得本方单位0的unit_id，一般就是主角
  get_hero = function(user_id)
    local team = CM.team_funcs.get_team(user_id,cc.COMBAT_CAMP_ACTIVE)
    if #team>0 then
      --CM.log_info("<COMBAT HERO> hero_id:%d",team[1])
      return team[1]
    else
      return nil
    end
    --return GET_COMBAT_SIDE(user_id, cc.COMBAT_CAMP_ACTIVE, 0)
  end,
  --获得敌方单位0的unit_id
  get_enemy = function(user_id)               
    return GET_COMBAT_SIDE(user_id, cc.COMBAT_CAMP_PASSIVE, 0)
  end,
  --获得某方全部单位的unit_id
  get_team = function(user_id, camp_type)           
    local team = {}
    for i=0,9 do
      local unit_id = GET_COMBAT_SIDE(user_id, camp_type, i)
      if unit_id >= 0 then table.insert(team, unit_id) end
    end
    return team
  end,
  --队伍全灭判断
  team_has_no_unit = function(user_id, camp_type)
    local unit_id = GET_COMBAT_SIDE(user_id, camp_type, 0)
    if unit_id >= 0 then
      return false
    else
      local team = CM.team_funcs.get_team(user_id, camp_type)
      if #team>0 then return false end
    end
    return true
  end,
  --队伍恢复
  recover_team = function(user_id, camp_type, hpreg, mpreg) 
    local team = CM.team_funcs.get_team(user_id, camp_type)
    if #team>0 then
      for key in pairs(team) do
        CM.unit_funcs.add_hp_percent(user_id, team[key], hpreg)
        CM.unit_funcs.add_mp_percent(user_id, team[key], mpreg)
      end
    end
  end,
  --敌我全体恢复
  recover_all = function(user_id, hpreg, mpreg)       
    CM.team_funcs.recover_team(user_id, cc.COMBAT_CAMP_ACTIVE, hpreg, mpreg)
    CM.team_funcs.recover_team(user_id, cc.COMBAT_CAMP_PASSIVE, hpreg, mpreg)
  end,
  --队伍虚弱（用于指定战败）
  set_weak = function(user_id, camp_type, dmg)        
    local team = CM.team_funcs.get_team(user_id,camp_type)
    if #team>0 then
      for key in pairs(team) do
        CM.unit_funcs.set_weak(user_id, team[key], dmg)
      end
    end
  end,
  --队伍强壮（用于加强难度）
  set_wake = function(user_id, camp_type, dmg)        
    local team = CM.team_funcs.get_team(user_id,camp_type)
    if #team>0 then
      for key in pairs(team) do
        CM.unit_funcs.set_wake(user_id, team[key], dmg)
      end
    end
  end,
  --队伍重置（用于抵消SET_WEAK）
  reset_power = function(user_id, camp_type)    
    local team = CM.team_funcs.get_team(user_id,camp_type)
    if #team>0 then
      for key in pairs(team) do
        CM.unit_funcs.reset_power(user_id,team[key])
      end
    end
  end,
  --设置敌方队伍成长强度
  set_enemy_growth = function(user_id)            
    local team = CM.team_funcs.get_team(user_id, cc.COMBAT_CAMP_PASSIVE)
    if #team>0 then
      for key in pairs(team) do
        CM.unit_funcs.set_growth(user_id, team[key], 0)
      end
    end
  end,
  --初始化队伍回合AI判断
  init_ai = function(user_id)                 
    local unit_id
    for i=0,9 do
      unit_id = GET_COMBAT_SIDE(user_id, cc.COMBAT_CAMP_ACTIVE, i)
      if unit_id >= 0 then CM.unit_funcs.init_ai(user_id, unit_id) end
      unit_id = GET_COMBAT_SIDE(user_id, cc.COMBAT_CAMP_PASSIVE, i)
      if unit_id >= 0 then CM.unit_funcs.init_ai(user_id, unit_id) end
    end
  end,
  --从指定index开始去除单位，第一位为1
  kill_units = function(user_id, camp_type, startIndex)   
    local index = startIndex or 1 --tabel的默认下标从1开始
    local team = CM.team_funcs.get_team(user_id, camp_type)
    if #team>0 then
      for key in pairs(team) do
        if key>= index then KILL_COMBAT_UNIT(user_id, team[key]) end
      end
    end
  end,
  --添加单位
  add_unit = function(user_id, camp_type, config_id, x, y, d) 
    local canstand,xx,yy = CM.level_funcs.get_free_pos(user_id, x, y)
    if canstand then
      local unit_id = ADD_COMBAT_UNIT(user_id, camp_type, config_id, xx, yy, d)
      if unit_id >= 0 then
        CM.unit_funcs.set_growth(user_id, unit_id, 0)
        CM.unit_funcs.init_ai(user_id, unit_id)
        return unit_id
      end
    end
    return -1
  end,
  --特定单位是否不在场上
  hero_dead = function(user_id, hero_id, camp_type)  
    local team = CM.team_funcs.get_team(user_id, camp_type)
    if #team>0 then
      for key in pairs(team) do
        local config_id = GET_COMBAT_UNIT_EXTRAATTR(user_id, team[key], cc.EXTRAATTR_CONFIG_ID)
        --CM.log_info("<COMBAT UNIT> config_id = %d", config_id)
        if config_id == hero_id then return false end
      end
    end
    return true
  end,
  --获得特定单位的UNIT_ID
  get_unit_id_by_config = function(user_id, hero_id, camp_type)  
    local team = CM.team_funcs.get_team(user_id, camp_type)
    if #team>0 then
      for key in pairs(team) do  
        if GET_COMBAT_UNIT_EXTRAATTR(user_id, team[key], cc.EXTRAATTR_CONFIG_ID) == hero_id then
          return team[key]
        end
      end
    end
    return nil
  end,
  --刷新队伍移动距离
  set_team_move = function(user_id, camp_type)
    local team = CM.team_funcs.get_team(user_id, camp_type)
    if #team>0 then
      for key in pairs(team) do
        CM.unit_funcs.set_move(user_id, team[key])
      end
    end
  end,
  --检查战斗阵营有几个活着的单位
  get_team_number = function(user_id, camp_type)        
    local t = 0
    for i=0,9 do
      if GET_COMBAT_SIDE(user_id, camp_type, i)>=0 then t=t+1 end
    end
    return t
  end,
  --检查战斗阵营有几个活着的特定性别的角色
  get_gender_number = function(user_id, camp_type, gender)  
    local t = 0
    local team = CM.team_funcs.get_team(user_id, camp_type)
    if #team>0 then
      for key in pairs(team) do
        if gender == GET_COMBAT_UNIT_EXTRAATTR(user_id, team[key], cc.EXTRAATTR_GENDER) then t=t+1 end
      end
    end
    return t
  end,
  --按游戏时间调整单位强度（根据给定范围）
  adjust_monster_power = function(user_id, startpower, endpower, years)
    --强度获得
    local time_pass = CM.capi_funcs.get_role_time(user_id)
    local attr_1 = startpower + (endpower-startpower) * time_pass/(cc.TP2YY * years)
    local attr_2 = startpower + (endpower-startpower) * time_pass/(cc.TP2YY * years)
    local attr_3 = startpower + (endpower-startpower) * time_pass/(cc.TP2YY * years)
    local attr_4 = startpower + (endpower-startpower) * time_pass/(cc.TP2YY * years)
    local attr_5 = startpower + (endpower-startpower) * time_pass/(cc.TP2YY * years)
    local attr_6 = startpower + (endpower-startpower) * time_pass/(cc.TP2YY * years)
    --设置属性
    local team = CM.team_funcs.get_team(user_id, cc.COMBAT_CAMP_PASSIVE)
    if #team>0 then
      for key in pairs(team) do
        local attrs = {
          attr_1 -5 + math.random(0,10), 
          attr_2 -5 + math.random(0,10), 
          attr_3 -5 + math.random(0,10), 
          attr_4 -5 + math.random(0,10), 
          attr_5 -5 + math.random(0,10), 
          attr_6 -5 + math.random(0,10)
        }
        CM.unit_funcs.set_attrs(user_id,team[key],attrs)
      end
    end
  end,
  --获得队伍等级评估
  get_enemy_level = function(user_id)
    local team = CM.team_funcs.get_team(user_id, cc.COMBAT_CAMP_PASSIVE)
    local level = 0
    if #team>0 then
      for key in pairs(team) do
        level = level + CM.unit_funcs.get_unit_level(team[key])
      end
    end
    return math.ceil(level/#team)
  end,
  
  --GET_FIGHTUNIT_LIST
  
}

CM.skill_funcs={
  --获取攻击面向
  get_attack_face_by_pos = function(cast_x, cast_y, targ_x, targ_y)
    local face = 0
    local x = targ_x - cast_x
    local y = targ_y - cast_y
    if (math.abs(x) <= math.abs(y) and y>0) then face = 3 end
    if (math.abs(x) <= math.abs(y) and y<0) then face = 1 end
    if (math.abs(x) >  math.abs(y) and x>0) then face = 2 end
    if (math.abs(x) >  math.abs(y) and x<0) then face = 4 end
    return face
  end,
  --是否侧击
  is_side_attack = function(attack_face, target_unit_id)
    if math.abs(attack_face - GET_UNIT_FACE(target_unit_id))==1 then
      return true
    end
    return false
  end,
  --是否背击
  is_back_attack = function(attack_face, target_unit_id)
    if attack_face == GET_UNIT_FACE(target_unit_id) then
      return true
    end
    return false
  end,
  --目标是否带有特定BUFF
  target_has_buff = function(unit_id, buff_id)        
    local buffs = GET_COMBAT_UNIT_BUFF(unit_id)
    for key in pairs(buffs) do
      if buffs[key] == buff_id then return true end
    end
    return false
  end,
  --队伍带有特定BUFF的计数
  count_team_has_buff = function(user_id, camp_type, buff_id) 
    local c = 0
    local team = CM.team_funcs.get_team(user_id, camp_type)
    if #team>0 then
      for key in pairs(team) do
        if CM.skill_funcs.target_has_buff(team[key], buff_id) then c = c + 1 end
      end
    end
    return c
  end,
  --从面向获取面向偏移
  get_face_off = function(face, distance)           
    local x_off,y_off = 0,0
    if face==1 then y_off = -distance end
    if face==2 then x_off = distance end
    if face==3 then y_off = distance end
    if face==4 then x_off = -distance end
    return x_off, y_off
  end,
  --从单位获取面向偏移
  get_face_pos = function(user_id, unit_id, distance)     
    local face = GET_UNIT_FACE(unit_id)
    return CM.skill_funcs.get_face_off(face, distance)
  end,
  --获得四向偏移
  get_dx_dy = function(x, y, target_posx, target_posy)    
    local dx,dy
    dx = target_posx-x
    if dx>0 then dx=1 end
    if dx<0 then dx=-1 end
    dy = target_posy-y
    if dy>0 then dy=1 end
    if dy<0 then dy=-1 end
    return dx,dy
  end,
  --判断是否敌人
  is_enemy = function(user_id, unit_a, unit_b)        
    local camp1 = GET_COMBAT_UNIT_CAMP(user_id, unit_a)
    local camp2 = GET_COMBAT_UNIT_CAMP(user_id, unit_b)
    if camp1~=camp2 then return true end
    return false
  end,
  --是否男性
  is_male = function(user_id, unit_id)            
    local gender = GET_COMBAT_UNIT_EXTRAATTR(user_id, unit_id, cc.EXTRAATTR_GENDER)
    if gender == cc.GENDER_MALE then return true end
    return false
  end,
  --特定位置周围的敌人
  get_enemy_around = function(user_id, unit_id, x, y)     
    local c = 0
    local targets = GET_AREA_TARGET_UNIT(user_id, x, y, cc.AREA_AROUND, CM.skill_funcs.get_enemy_camp(user_id, unit_id), 1)
    for key in pairs(targets) do c = c + 1 end
    return c
  end,
  --获取敌方阵营
  get_enemy_camp = function(user_id, unit_id)         
    local camp = GET_COMBAT_UNIT_CAMP(user_id, unit_id)
    if camp == 1 then return 2 end
    return 1
  end,
  --普攻范围类型
  get_normal_attack_range = function(user_id, unit_id)    
    local weapon = GET_COMBAT_UNIT_WEAPON_TYPE(user_id, unit_id)
    if weapon == 0 then return 204 end  --平拳
    if weapon == 1 then return 101 end  --刀命
    if weapon == 2 then return 102 end  --剑命
    if weapon == 3 then return 103 end  --棍命
    if weapon == 4 then return 104 end  --拳命
    if weapon == 5 then return 105 end  --暗命
    if weapon == 6 then return 106 end  --奇命
  end,
  --获得跳跃距离
  get_jump_dis = function(user_id, x0, y0, xt, yt, step)    
    local num = 0
    local x = x0
    local y = y0
    local x_off = xt - x0
    local y_off = yt - y0
    for i=1,step do
      local d = step - i
      local xx = xt + d * x_off
      local yy = yt + d * y_off
      local block = GET_BATTLEFIELD_POS_BLOCK(user_id, xx, yy)
      if block < 3 then --可行走
        local target = GET_COMBAT_UNIT_BY_POS(user_id, xx, yy)
        if target<0 then  --无单位
          num = d + 1
          x = xx
          y = yy
          break
        end
      end
    end
    return num, x, y
  end,  
  --获得冲刺距离
  get_rush_dis = function(user_id, unit_id, xt, yt, step)   
    local num = 0
    local x_off,y_off = CM.skill_funcs.get_face_pos(user_id, unit_id, 1)
    for d=0,step-1 do
      local block = GET_BATTLEFIELD_POS_BLOCK(user_id, xt + d * x_off, yt + d * y_off)
      if block < 3 then --可行走
        local target = GET_COMBAT_UNIT_BY_POS(user_id, xt + d * x_off, yt + d * y_off)
        if target<0 then
          num = d + 1
        else
          break
        end
      else
        break
      end
    end
    return num
  end,
  --判断是否坠崖
  is_jump_down = function(user_id, x0, y0, xt, yt, step)    
    local n,x,y = CM.skill_funcs.get_jump_dis(user_id, x0, y0, xt, yt, step)
    local block = GET_BATTLEFIELD_POS_BLOCK(user_id, x, y)
    if block == 2 then return true end
    return false
  end,
  
}

CM.buff_funcs={
  --是否负面效果
  is_bad_buff = function(buff_id)               
    for key in pairs(cc.bad_buffs) do
      if buff_id == cc.bad_buffs[key] then
        return true
      end
    end
    return false
  end,
  --是否持有负面效果
  has_bad_buff = function(unit_id)              
    local buffs = GET_COMBAT_UNIT_BUFF(unit_id)
    for key in pairs(buffs) do
      if CM.buff_funcs.is_bad_buff(buffs[key]) then return true end
    end
    return false
  end,
  --清除负面效果
  clear_bad_buff = function (user_id, unit_id)         
    local buffs = GET_COMBAT_UNIT_BUFF(unit_id)
    for key in pairs(buffs) do
      if CM.buff_funcs.is_bad_buff(buffs[key]) then
        DEL_COMBAT_UNIT_BUFF(user_id, unit_id, buffs[key])
      end
    end
  end,
  --是否控制效果
  is_control_buff = function(buff_id)             
    for key in pairs(cc.control_buffs) do
      if buff_id == cc.control_buffs[key] then return true end
    end
    return false
  end,
  --是否持有控制效果
  has_control_buff = function(unit_id)            
    local buffs = GET_COMBAT_UNIT_BUFF(unit_id)
    for key in pairs(buffs) do
      if CM.buff_funcs.is_control_buff(buffs[key]) then return true end
    end
    return false
  end,
  --清除控制效果
  clear_control = function(user_id, unit_id)             
    local buffs = GET_COMBAT_UNIT_BUFF(unit_id)
    for key in pairs(buffs) do
      if CM.buff_funcs.is_control_buff(buffs[key]) then
        DEL_COMBAT_UNIT_BUFF(user_id, unit_id, buffs[key])
      end
    end
    return false
  end,
  --是否伤害提升效果
  is_dmgUP_buff = function(buff_id)             
    for key in pairs(cc.dmgUP_buffs) do
      if buff_id == cc.dmgUP_buffs[key] then
        return true
      end
    end
    return false
  end,
  --是否持有伤害提升效果
  has_dmgUP_buff = function(unit_id)              
    local buffs = GET_COMBAT_UNIT_BUFF(unit_id)
    for key in pairs(buffs) do
      if CM.buff_funcs.is_dmgUP_buff(buffs[key]) then return true end
    end
    return false
  end,
  --清除持续伤害效果（毒素等）
  clear_poison = function(user_id, unit_id)         
    DEL_COMBAT_UNIT_BUFF(user_id, unit_id,10390101 )  --毒效1~5层
    DEL_COMBAT_UNIT_BUFF(user_id, unit_id,10390102 )
    DEL_COMBAT_UNIT_BUFF(user_id, unit_id,10390103 )
    DEL_COMBAT_UNIT_BUFF(user_id, unit_id,10390104 )
    DEL_COMBAT_UNIT_BUFF(user_id, unit_id,10390105 )
    DEL_COMBAT_UNIT_BUFF(user_id, unit_id,10530000 )  --毒蛊标记
    DEL_COMBAT_UNIT_BUFF(user_id, unit_id,10530201 )  --潜龙毒影
    DEL_COMBAT_UNIT_BUFF(user_id, unit_id,10180301 ) --火云神掌
    DEL_COMBAT_UNIT_BUFF(user_id, unit_id,10190201 )  --狂化邪毒
    DEL_COMBAT_UNIT_BUFF(user_id, unit_id,10190301 )  --七步追魂
  end,

}

CM.capi_funcs={
  get_unit_attr = function(unit_id, attr_type)
    return GET_COMBAT_UNIT_ATTR(unit_id, attr_type)
  end,
  get_unit_extra_attr = function(user_id, unit_id, extra_attr_type)
    return GET_COMBAT_UNIT_EXTRAATTR(user_id, unit_id, extra_attr_type)
  end,
  set_unit_attr = function(user_id, unit_id, attr_type, attr_value)
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, attr_type, attr_value)
  end,
  set_unit_ai = function(user_id, unit_id, ai_type)
    CHANGE_COMBAT_NPC_AI(user_id, unit_id, ai_type)
  end,
  get_unit_by_pos = function(user_id, target_posx,target_posy)
    return GET_COMBAT_UNIT_BY_POS(user_id, target_posx,target_posy)
  end,
  set_unit_ai = function(user_id, unit_id, ai)
    CHANGE_COMBAT_NPC_AI(user_id, unit_id, ai)
  end,
  add_unit_buff = function(user_id, caster_unit_id, target_unit_id, buff_id)
    ADD_UNIT_BUFF(user_id, caster_unit_id, target_unit_id, buff_id)
  end,
  unit_cast_skill = function(user_id, caster_unit_id, target_posx, target_posy, skill_id)
    UNIT_CAST_SKILL(user_id, caster_unit_id, target_posx, target_posy, skill_id)
  end,
  get_unit_buff = function(unit_id)
    return GET_COMBAT_UNIT_BUFF(unit_id)
  end,
  del_unit_buff = function(user_id, unit_id, buff_id)
    DEL_COMBAT_UNIT_BUFF(user_id, unit_id, buff_id)
  end,
  get_unit_pos = function(unit_id)
    return GET_COMBAT_UNIT_POSITION(unit_id)
  end,
  add_drop = function(user_id, drop_id, drop_type)
    ADD_EXTRA_COMBAT_DROPID(user_id, drop_id, drop_type)
  end,
  get_atk_type = function(unit_id)
    return GET_COMBAT_UNIT_WEAPON_TYPE(unit_id)
  end,
  get_unit_face = function(unit_id)
    return GET_UNIT_FACE(unit_id)
  end,
  get_atk_face = function(caster_unit_id, target_unit_id)
    return GET_ATTACK_FACE(caster_unit_id, target_unit_id)
  end,
  get_role_time = function(user_id)
    local startTime = cc.TP2YY + 9*cc.TP2MM
    local timePoint = GET_ROLE_TIME(user_id) - startTime
    return timePoint
  end,
  get_round_num = function(user_id)
    return GET_COMBAT_ROUND_NUM(user_id)
  end,
  get_round_end = function(user_id)
    if IS_COMBAT_ROUND_END(user_id) then return GET_COMBAT_ROUND_NUM(user_id) end
    return -1
  end,
  --ADD_UNIT_BUFF(uin, cast_unit_id, target_unit_id, buff_id)
  --DEL_COMBAT_UNIT_BUFF(uin, unit_id, buff_id)
  
}

function CM.log_info(...)
  LOG_INFO(string.format(...))
end

function CM.checkValue(user_id, unit_id, value)          --输出特定值（仅用于测试）
  local hp = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_HP)
  SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_HP, hp+value) --通过加血来输出值
end

function CM.showDate(user_id)
    local YY,MM,DD,TT = CM.level_funcs.get_role_date(user_id)
    LOG_INFO(string.format("<COMBAT DATE> YEAR: %d, MONTH: %d, DAY: %d, HOUR: %d",YY,MM,DD,TT))
end

return CM

