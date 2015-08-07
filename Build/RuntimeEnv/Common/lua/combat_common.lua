
--战斗脚本通用函数定义文件

--定义阵型变量
COMBAT_CAMP_ACTIVE = 1    --主动战斗方
COMBAT_CAMP_PASSIVE = 2   --被动战斗方

--定义战斗额外属性的变量
COMBAT_EXTRAATTR_GENDER = 1		--战斗单位的性别
COMBAT_EXTRAATTR_GOOD = 2		--善恶值
COMBAT_EXTRAATTR_CONFIGID = 3	--战斗单位配置的ID

--定义技能脚本的调用类型
SKILL_SCRIPT_CALL_DAMAGE = 1		--计算伤害时调用
SKILL_SCRIPT_CALL_SKILLDONE = 2		--技能处理完成后调用
SKILL_SCRIPT_CALL_CHIEFDODGE = 3 	--主要目标产生闪避时调用

--定义脚本处理战斗单位移动的类型
COMBAT_UNIT_MOVE_NORMAL = 1		--普通移动，正常寻路的
COMBAT_UNIT_MOVE_TELEPORT = 2	--传送，可以是任意位置
COMBAT_UNIT_MOVE_JUMP = 3		--跳跃移动
COMBAT_UNIT_MOVE_RUSH = 4		--冲锋移动

--定制战斗单位身上状态的类型
COMBAT_UNIT_STATUS_NOTMOVE = 1		--禁止移动
COMBAT_UNIT_STATUS_NOTACTION = 2	--禁止行动
COMBAT_UNIT_STATUS_NOTROUND = 3		--禁止回合
COMBAT_UNIT_STATUS_IGNOREJOINT = 4	--无视合击
COMBAT_UNIT_STATUS_UNBENDING = 5	--不屈

--属性编号
FIGHT_ATTR_HP = 3			--生命值属性
FIGHT_ATTR_MP = 4			--内力值属性
--DYNAMIC_ATTR_HPMAX = 32		--生命上限属性
--DYNAMIC_ATTR_MPMAX = 33		--内力上限属性
DYNAMIC_ATTR_HPMAX = 27		--生命上限属性
DYNAMIC_ATTR_MPMAX = 28		--内力上限属性

--[[面向
	1-右上
	2-右下
	3-左下
	4-左上
]]--

npcUnits={}
npcUnitsNum = 29
npcUnits[0]=2101001
npcUnits[1]=2101002
npcUnits[2]=2101003
npcUnits[3]=2101005
npcUnits[4]=2101006
npcUnits[5]=2101007
npcUnits[6]=2101012
npcUnits[7]=2101015
npcUnits[8]=2101016
npcUnits[9]=2101017
npcUnits[10]=2101018
npcUnits[11]=2101019
npcUnits[12]=2101020
npcUnits[13]=2101021
npcUnits[14]=2101022
npcUnits[15]=2101024
npcUnits[16]=2101025
npcUnits[17]=2101026
npcUnits[18]=2101027
npcUnits[19]=2101030
npcUnits[20]=2101032
npcUnits[21]=2101033
npcUnits[22]=2101037
npcUnits[23]=2101039
npcUnits[24]=2101045
npcUnits[25]=2101048
npcUnits[26]=2101052
npcUnits[27]=2101053
npcUnits[28]=2101063
npcUnits[29]=2101064

----------------------------------------------------------------------------------------------------------------------------------------
--队伍单位
----------------------------------------------------------------------------------------------------------------------------------------
--默认的战斗脚本函数，判断一方全灭
function default_level_script(userID)
    if not check_side_has_unit(userID, COMBAT_CAMP_PASSIVE)
    then
        SET_CAMP_ACTIVE_WIN(userID, 1)--对方全灭
        return 
    elseif not check_side_has_unit(userID, COMBAT_CAMP_ACTIVE)
    then
        SET_CAMP_ACTIVE_WIN(userID, 0)--己方全灭
        return
    end
    return
end
--获取战斗阵营的单位组
function get_unit_by_side(userID, camp_type)
    local camp = {}
    for i=0,9 do
		camp[i] = GET_COMBAT_SIDE(userID,camp_type,i)
    end
    return camp
end
--获取双方所有单位
function get_all_units(userID)
	local units = {}
	units[COMBAT_CAMP_ACTIVE]=get_unit_by_side(userID, COMBAT_CAMP_ACTIVE)
	units[COMBAT_CAMP_PASSIVE]=get_unit_by_side(userID, COMBAT_CAMP_PASSIVE)
	return units
end
--检查战斗阵营是否有战斗单位
function check_side_has_unit(userID, camp_type)
    for i=0,9 do
		if GET_COMBAT_SIDE(userID,camp_type,i)>=0 then return true end
    end
    return false
end
--检查战斗阵营有几个活着的单位
function get_side_unit_number(userID, camp_type)
	local t = 0
    for i=0,9 do
		if GET_COMBAT_SIDE(userID,camp_type,i)>=0 then t=t+1 end
    end
    return t
end
--添加战斗单位
function add_unit(userID, side, monsterid, x, y, direction)
	local newUnitID = -1
	local xx = x
	local yy = y
	if pos_can_stand(userID, x, y) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID < 0 and pos_can_stand(userID, x+1, y+0) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID < 0 and pos_can_stand(userID, x-1, y+0) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID < 0 and pos_can_stand(userID, x+0, y+1) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID < 0 and pos_can_stand(userID, x+0, y-1) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID < 0 and pos_can_stand(userID, x+1, y-1) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID < 0 and pos_can_stand(userID, x+1, y+1) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID < 0 and pos_can_stand(userID, x-1, y+1) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID < 0 and pos_can_stand(userID, x-1, y-1) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID >= 0 then
		adjust_unit_power(userID, newUnitID, 0)	--调整强度
		adjust_unit_move(userID, unit_id)		--调整移动
		add_ai_check(userID, newUnitID)			--注册行为
		return newUnitID
	end
	LOG_INFO("Failed to add unit: %d\n", monsterid)
	return -1
end
--获得平均强度
function get_power(userID, units)
	local power=0
	local count=0--伙伴数量
	local enemy=0--敌人数量
	for i=1,9 do
		if units[COMBAT_CAMP_ACTIVE][i] >=0 then
			count=count+ 1
			power=power+ GET_COMBAT_UNIT_ATTR(units[COMBAT_CAMP_PASSIVE][0], 6)
			power=power+ GET_COMBAT_UNIT_ATTR(units[COMBAT_CAMP_PASSIVE][0], 7)
			power=power+ GET_COMBAT_UNIT_ATTR(units[COMBAT_CAMP_PASSIVE][0], 8)
			power=power+ GET_COMBAT_UNIT_ATTR(units[COMBAT_CAMP_PASSIVE][0], 9)
			power=power+ GET_COMBAT_UNIT_ATTR(units[COMBAT_CAMP_PASSIVE][0], 10)
			power=power+ GET_COMBAT_UNIT_ATTR(units[COMBAT_CAMP_PASSIVE][0], 11)
		end
		if units[COMBAT_CAMP_PASSIVE][i] >=0 then
			enemy=enemy+1
		end
	end
	power = math.floor(power/count/6)--伙伴平均强度
	return power,count,enemy
end
--补充额外敌人
function additional_enemy(userID, units, power, count, enemy, backup_x, backup_y)
	local backup_id_low = 2000100 + math.max(1,math.min(power,100)-5)
	local backup_id_high = 2000100 + math.min(power+5,100)
	for i=0,9 do
		if count>enemy then
			if units[COMBAT_CAMP_ACTIVE][i] <0 then
				newUnitID,newUnitIndex = add_unit(userID, units, COMBAT_CAMP_PASSIVE, math.random(backup_id_low,backup_id_high), backup_x, backup_y)
				count=count-1
				--print("enemy added")
			end
		end
	end

end
--
function add_enemy(userID, units, backup_x, backup_y)
	local power=0--伙伴平均强度
	local count=0--伙伴数量
	local enemy=0--敌人数量
	power,count,enemy = get_power(userID, units)
	if count>enemy then
		additional_enemy(userID, units, power, count, enemy, backup_x, backup_y)
	end
end
function kill_team(userID, units, side, startIndex)
	for i=startIndex,9 do
		if units[side][i] >=0 then
			KILL_COMBAT_UNIT(userID, units[side][i])--去除玩家操作的主角
			units[side][i]=-1
		end
	end
end
--特定位置布怪
function putMonster(userID, x, y, monster_id,direction)
	local tx,ty
	if pos_can_stand(userID, x, y) then
		tx=x
		ty=y
	elseif pos_can_stand(userID, x+1, y+0) then
		tx=x+1
		ty=y+0
	elseif pos_can_stand(userID, x-1, y+0) then
		tx=x-1
		ty=y+0
	elseif pos_can_stand(userID, x+0, y+1) then
		tx=x+0
		ty=y+1
	elseif pos_can_stand(userID, x+0, y-1) then
		tx=x+0
		ty=y-1
	elseif pos_can_stand(userID, x+1, y+1) then
		tx=x+1
		ty=y+1
	elseif pos_can_stand(userID, x-1, y-1) then
		tx=x-1
		ty=y-1
	elseif pos_can_stand(userID, x+1, y-1) then
		tx=x+1
		ty=y-1
	elseif pos_can_stand(userID, x-1, y+1) then
		tx=x-1
		ty=y+1
	else
		return 0
	end
	add_unit(userID, COMBAT_CAMP_PASSIVE, monster_id, tx, ty, direction)
	return 1
end
--八向加怪
function freemapbattle(userID,level_id,first,monster_id,maxcount)
	local result = 0
	if first==1 then
		unitID = GET_COMBAT_SIDE(userID, COMBAT_CAMP_ACTIVE , 0)	--主角
		local x,y = GET_COMBAT_UNIT_POSITION(unitID)				--中心位置
		local count = 0
		if canMove(userID, x, y, 1, 0, 4) then
			count = count + putMonster(userID, x + 3, y, monster_id, 4)
		end
		if canMove(userID, x, y, -1, 0, 4) and count<maxcount then
			count = count + putMonster(userID, x - 3, y, monster_id, 2)
		end
		if canMove(userID, x, y, 0, 1, 4) and count<maxcount then
			count = count + putMonster(userID, x, y + 3, monster_id, 1)
		end
		if canMove(userID, x, y, 0, -1, 4) and count<maxcount then
			count = count + putMonster(userID, x, y - 3, monster_id, 3)
		end
		
		if canMove(userID, x, y, 1, -1, 4) and count<maxcount then
			count = count + putMonster(userID, x + 3, y - 3, monster_id, 3)
		end
		if canMove(userID, x, y, 1, 1, 4) and count<maxcount then
			count = count + putMonster(userID, x + 3, y + 3, monster_id, 3)
		end
		if canMove(userID, x, y, -1, 1, 4) and count<maxcount then
			count = count + putMonster(userID, x - 3, y + 3, monster_id, 3)
		end
		if canMove(userID, x, y, -1, -1, 4) and count<maxcount then
			count = count + putMonster(userID, x - 3, y - 3, monster_id, 3)
		end
	end
	return result
end
--特定方向上相连
function canMove(userID, startx, starty, dx, dy, distance)
	for i =0,distance do
		local x = startx + dx*distance
		local y = starty + dy*distance
		if GET_BATTLEFIELD_POS_BLOCK(userID, x, y) > 1 then
			return false
		end
	end
	return true
end
--特定位置周围的敌人
function getEnemyAround(userID, unit_id, x, y)
	local enemy = 0
	local targets = GET_AREA_TARGET_UNIT(userID, x, y, AREA_AROUND, getEnemyCamp(userID, unit_id), 1)
	for key in pairs(targets) do
		enemy = enemy + 1
	end
	return enemy
end

----------------------------------------------------------------------------------------------------------------------------------------
--单位属性
----------------------------------------------------------------------------------------------------------------------------------------
--获取敌方阵营，主要用于PVP
function getEnemyCamp(userID, unit_id)
	local camp = GET_COMBAT_UNIT_CAMP(userID, unit_id)
	if camp ==1 then
		return 2
	else
		return 1
	end
end
--判断是否敌人
function isEnemy(userID, unit_id, target_id)
	local camp1 = GET_COMBAT_UNIT_CAMP(userID, unit_id)
	local camp2 = GET_COMBAT_UNIT_CAMP(userID, target_id)
	if camp1~=camp2 then
		return true
	else
		return false
	end
end
--检查战斗阵营有几个活着的特定性别的角色
function get_side_gender_number(userID, camp_type, gender_check)
	local t = 0
    local camp = get_unit_by_side(userID, camp_type)
    for i=0,9 do
		if camp[i]>=0 then
			if gender_check == GET_COMBAT_UNIT_EXTRAATTR(userID, camp[i], COMBAT_EXTRAATTR_GENDER) then	t=t+1 end
		end
    end
    return t
end
--设置队伍AI
function set_team_ai(userID, units, side, ai, startIndex)
	for i=startIndex,9 do
		if units[side][i] > 0 then
			CHANGE_COMBAT_NPC_AI(userID, units[side][i], ai)
		end
	end
end
--从单位获取面向偏移
function getFacePos(userID, unit_id, distance)
	local face = GET_UNIT_FACE(unit_id)
	return getFaceOff(face, distance)
end
--从面向获取面向偏移
function getFaceOff(face, distance)
	local x_off,y_off = 0,0
	if face==1 then y_off = -distance end
	if face==2 then x_off = distance end
	if face==3 then y_off = distance end
	if face==4 then x_off = -distance end
	return x_off, y_off
end
--获得四向偏移
function getDXDY(x, y, target_posx, target_posy)
	local dx,dy
	dx = target_posx-x
	if dx>0 then dx=1 end
	if dx<0 then dx=-1 end
	dy = target_posy-y
	if dy>0 then dy=1 end
	if dy<0 then dy=-1 end
	return dx,dy
end
--获得跳跃距离
function getJumpDis(userID, start_posx, start_posy, target_posx, target_posy, step)
	local num = 0
	local x = start_posx
	local y = start_posy
	local x_off = target_posx - start_posx
	local y_off = target_posy - start_posy
	for i=1,step do
		local d = step - i
		local xx = target_posx + d * x_off
		local yy = target_posy + d * y_off
		local block = GET_BATTLEFIELD_POS_BLOCK(userID, xx, yy)
		if block < 3 then	--可行走
			local target = GET_COMBAT_UNIT_BY_POS(userID, xx, yy)
			if target<0 then	--无单位
				num = d + 1
				x = xx
				y = yy
				break
			end
		end
	end
	return num, x, y
end
--判断是否坠崖
function isJumpDown(userID, start_posx, start_posy, target_posx, target_posy, step)
	local n,x,y = getJumpDis(userID, start_posx, start_posy, target_posx, target_posy, step)
	local block = GET_BATTLEFIELD_POS_BLOCK(userID, x, y)
	if block == 2 then
		return true
	else
		return false
	end
end
--获得冲刺距离
function getRushDis(userID, caster_unit_id, target_posx, target_posy, step)
	local num = 0
	local x_off,y_off = getFacePos(userID, caster_unit_id, 1)
	for d=0,step-1 do
		local block = GET_BATTLEFIELD_POS_BLOCK(userID, target_posx + d * x_off, target_posy + d * y_off)
		if block < 3 then	--可行走
			local target = GET_COMBAT_UNIT_BY_POS(userID, target_posx + d * x_off, target_posy + d * y_off)
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
end
--是否男性
function isMale(user_id, unit_id)
	local gender = GET_COMBAT_UNIT_EXTRAATTR(user_id, unit_id, 1)
	if gender == 101 then
		return true
	else
		return false
	end
end
--生命低于百分比
function hp_less_than_percent(unit,num)
	local hp = GET_COMBAT_UNIT_ATTR(unit, 3)
	local maxhp = GET_COMBAT_UNIT_ATTR(unit, DYNAMIC_ATTR_HPMAX)
	if hp/maxhp < num/100 then
		return true
	end
	return false
end
--目标是否带有特定BUFF
function target_has_buff(unit_id, buff_id)
	local buffs = GET_COMBAT_UNIT_BUFF(unit_id)
	for key in pairs(buffs) do
		if buffs[key] == buff_id then return true end
	end
	return false
end
--队伍带有特定BUFF的计数
function count_team_has_buff(userID, side, buffID)
	local c = 0
	local units = get_unit_by_side(userID, side)
    for i=0,9 do
		if units[i]>=0 then
			if target_has_buff(units[i], buffID) then c = c + 1 end
		end
    end
    return c
end

----------------------------------------------------------------------------------------------------------------------------------------
--关卡功能
----------------------------------------------------------------------------------------------------------------------------------------
--全体回蓝
function recover_mp(userID, reg)
	local mp = 0
	local mmp = 0
    for i=0,9 do
		local unit = GET_COMBAT_SIDE(userID,COMBAT_CAMP_ACTIVE,i)
		if unit>=0 then
			mp = GET_COMBAT_UNIT_ATTR(unit, 4)
			mmp = GET_COMBAT_UNIT_ATTR(unit, DYNAMIC_ATTR_MPMAX)
			SET_COMBAT_UNIT_ATTR(userID, unit, 4, mp + mmp*reg)
		end
		unit = GET_COMBAT_SIDE(userID,COMBAT_CAMP_PASSIVE,i)
		if unit>=0 then
			mp = GET_COMBAT_UNIT_ATTR(unit, 4)
			mmp = GET_COMBAT_UNIT_ATTR(unit, DYNAMIC_ATTR_MPMAX)
			SET_COMBAT_UNIT_ATTR(userID, unit, 4, mp + mmp*reg)
		end
    end
end
--全体回血
function recover_hp(userID, reg)
	local hp = 0
	local mhp = 0
    for i=0,9 do
		local unit = GET_COMBAT_SIDE(userID,COMBAT_CAMP_ACTIVE,i)
		if unit>=0 then
			hp = GET_COMBAT_UNIT_ATTR(unit, 3)
			mhp = GET_COMBAT_UNIT_ATTR(unit, DYNAMIC_ATTR_HPMAX)
			SET_COMBAT_UNIT_ATTR(userID, unit, 3, hp + mhp*reg)
		end
		unit = GET_COMBAT_SIDE(userID,COMBAT_CAMP_PASSIVE,i)
		if unit>=0 then
			hp = GET_COMBAT_UNIT_ATTR(unit, 3)
			mhp = GET_COMBAT_UNIT_ATTR(unit, DYNAMIC_ATTR_HPMAX)
			SET_COMBAT_UNIT_ATTR(userID, unit, 3, hp + mhp*reg)
		end
    end
end
--队伍强度调整
function team_adjust(userID, side, reg)
	local hp = 0
	local mhp = 0
    for i=0,9 do
		local unit = GET_COMBAT_SIDE(userID,side,i)
		if unit>=0 then
			--maxhp,hp
			local mhp = GET_COMBAT_UNIT_ATTR(unit, DYNAMIC_ATTR_HPMAX)
			SET_COMBAT_UNIT_ATTR(userID, unit, DYNAMIC_ATTR_HPMAX, mhp * reg)
			SET_COMBAT_UNIT_ATTR(userID, unit, 3, 1+mhp * reg)
			--伤害
			local dmg = GET_COMBAT_UNIT_ATTR(unit, 20)
			SET_COMBAT_UNIT_ATTR(userID, unit, 20, 1+dmg * reg)
		end
    end
end

--通用的轮到行动执行BUFF（用于调整AI、播放剧情等）
function add_ai_check_all(userID)
	local unit
    for i=0,9 do
		unit = GET_COMBAT_SIDE(userID,COMBAT_CAMP_ACTIVE,i)
		if unit>=0 then
			add_ai_check(userID,unit)
		end
		unit = GET_COMBAT_SIDE(userID,COMBAT_CAMP_PASSIVE,i)
		if unit>=0 then
			add_ai_check(userID,unit)
		end
    end
end
function add_ai_check(userID, unit_id)
	ADD_UNIT_BUFF(userID, unit_id, unit_id, 1000)
end

--随机5个单位测试PVP战场
function set5RandomUnit(userID, units)
	local u={}
	u[0]=2101001
	u[1]=2101002
	u[2]=2101003
	u[3]=2101005
	u[4]=2101006
	u[5]=2101007
	u[6]=2101012
	u[7]=2101015
	u[8]=2101016
	u[9]=2101017
	u[10]=2101018
	u[11]=2101019
	u[12]=2101020
	u[13]=2101021
	u[14]=2101022
	u[15]=2101024
	u[16]=2101025
	u[17]=2101026
	u[18]=2101027
	u[19]=2101030
	u[20]=2101032
	u[21]=2101033
	u[22]=2101037
	u[23]=2101039
	u[24]=2101045
	u[25]=2101048
	u[26]=2101052
	u[27]=2101053
	u[28]=2101063
	u[29]=2101064
		
	add_unit(userID, units, COMBAT_CAMP_PASSIVE, u[math.random(0,29)], 16, 14, 4)
	add_unit(userID, units, COMBAT_CAMP_PASSIVE, u[math.random(0,29)], 16, 15, 4)
	add_unit(userID, units, COMBAT_CAMP_PASSIVE, u[math.random(0,29)], 14, 15, 4)
	add_unit(userID, units, COMBAT_CAMP_PASSIVE, u[math.random(0,29)], 14, 16, 4)
	add_unit(userID, units, COMBAT_CAMP_PASSIVE, u[math.random(0,29)], 12, 16, 4)
end
START_TIME = 0
TP2HH = 4 * 10
TP2DD = 6 * TP2HH
TP2MM = 3 * TP2DD
TP2YY = 12 * TP2MM
--获取游戏当前年月日时
function get_role_date(userID)
	local timePoint = GET_ROLE_TIME(userID)
	local y = timePoint / TP2YY
	local YY = math.floor(y)
	local m = (y - YY) / TP2MM
	local MM = 1 + math.floor(m)
	local d = (m - MM) / TP2DD
	local DD = 1 + math.floor(d)
	local t = (d - DD) / TP2HH
	local TT = 1 + math.floor(t)
	return YY,MM,DD,TT
end
--角色强度随时间变化配置
local hero_power = {}
function init_hero_power()
	hero_power[2101010]=160
	hero_power[2101023]=132
	hero_power[2101054]=128
	hero_power[2101034]=126
	hero_power[2101055]=126
	hero_power[2101052]=120
	hero_power[2101012]=120
	hero_power[2200023]=116
	hero_power[2101044]=116
	hero_power[2101046]=114
	hero_power[2101021]=112
	hero_power[2101017]=112
	hero_power[2101011]=108
	hero_power[2101009]=106
	hero_power[2101064]=106
	hero_power[2101006]=106
	hero_power[2101030]=106
	hero_power[2101016]=106
	hero_power[2101001]=104
	hero_power[2101043]=104
	hero_power[2300123]=104
	hero_power[2101056]=104
	hero_power[2101038]=104
	hero_power[2300527]=104 --青霞子
	hero_power[2101018]=102
	hero_power[2101019]=102
	hero_power[2200016]=102
	hero_power[2101061]=100
	hero_power[2101041]=100
	hero_power[2101060]=100
	hero_power[2101048]=98
	hero_power[2101014]=98
	hero_power[2101033]=98
	--hero_power[2101035]=98
	hero_power[2300166]=98 --紫阳子
	hero_power[2101068]=96
	hero_power[2101015]=96
	hero_power[2101057]=96
	hero_power[2101045]=96
	hero_power[2101058]=96
	hero_power[2101036]=96
	hero_power[2101008]=94
	hero_power[2101005]=94
	hero_power[2101025]=94
	hero_power[2101028]=94
	hero_power[2101029]=94
	hero_power[2200014]=94
	hero_power[2101020]=92
	hero_power[2101024]=92
	hero_power[2101037]=92
	hero_power[2101059]=90
	hero_power[2101002]=90
	hero_power[2101063]=90
	hero_power[2101069]=90
	hero_power[2101039]=88
	hero_power[2101031]=88
	hero_power[2200015]=88
	hero_power[2101026]=84
	hero_power[2101003]=80
	hero_power[2101053]=78
	hero_power[2200013]=76
	hero_power[2200010]=76
	hero_power[2200012]=72
	hero_power[2101042]=70
	hero_power[2101062]=70
	hero_power[2101027]=70
	hero_power[2101049]=68
	hero_power[2101022]=68
	hero_power[2101040]=66
	hero_power[2101032]=66
	hero_power[2101007]=64
	hero_power[2101004]=64
	hero_power[2200011]=62
	hero_power[2101050]=60
	hero_power[2101051]=60
end
init_hero_power()
--获取角色基本强度
local max_power = 200
local min_power = 60
local max_percent = 90
local min_percent = 10
local good_exp = 2.5
local good_a = (max_percent-min_percent)/(max_power^good_exp-min_power^good_exp)
local good_b = max_percent-max_power^good_exp*good_a
local evil_exp = 1.5
local evil_a = (max_percent-min_percent)/(max_power^evil_exp-min_power^evil_exp)
local evil_b = max_percent-max_power^evil_exp*evil_a
function getBasePower(unit_power, good_or_evil)
	local p = unit_power
	if good_or_evil > 0 then
		p = unit_power^good_exp*good_a+good_b
	else
		p = unit_power^evil_exp*evil_a+evil_b
	end
	return p
end
--按游戏时间调整单位强度
function adjust_unit_power(userID, unit_id, time_offset)
    local maxpower = hero_power[GET_COMBAT_UNIT_EXTRAATTR(userID, unit_id, 3)]
    if maxpower then
		local basepower = getBasePower(maxpower, GET_COMBAT_UNIT_EXTRAATTR(userID, unit_id, 2))
		--获取属性分布
		local attr_1 = GET_COMBAT_UNIT_ATTR(unit_id, 6)/basepower
		local attr_2 = GET_COMBAT_UNIT_ATTR(unit_id, 7)/basepower
		local attr_3 = GET_COMBAT_UNIT_ATTR(unit_id, 8)/basepower
		local attr_4 = GET_COMBAT_UNIT_ATTR(unit_id, 9)/basepower
		local attr_5 = GET_COMBAT_UNIT_ATTR(unit_id, 10)/basepower
		local attr_6 = GET_COMBAT_UNIT_ATTR(unit_id, 11)/basepower
		local time_pass = GET_ROLE_TIME(userID)
		local train_time = 5 * TP2YY --这里假设5年可以从基本值修炼到最终值
		local currentpower = basepower+ math.floor((maxpower - basepower) * (time_pass+time_offset) / train_time)
		
		--调整一级属性
		attr_1 = attr_1 * currentpower
		attr_2 = attr_2 * currentpower
		attr_3 = attr_3 * currentpower
		attr_4 = attr_4 * currentpower
		attr_5 = attr_5 * currentpower
		attr_6 = attr_6 * currentpower
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 6, attr_1)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 7, attr_2)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 8, attr_3)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 9, attr_4)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 10, attr_5)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 11, attr_6)
		
		--调整二级属性
		--maxhp,hp
		SET_COMBAT_UNIT_ATTR(userID, unit_id, DYNAMIC_ATTR_HPMAX, attr_1*30)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 3, attr_1*30)
		--maxmp,mp
		SET_COMBAT_UNIT_ATTR(userID, unit_id, DYNAMIC_ATTR_MPMAX, attr_3*20)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 4, attr_3*20)
		--攻击,防御
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 12, attr_1*6)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 13, attr_1*6)
		--命中,闪避
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 14, attr_4*6)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 15, attr_4*6)
		--暴击,坚韧
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 16, attr_2*6)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 17, attr_2*6)
		--精准,格挡
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 18, attr_3*6)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 19, attr_3*6)
		--伤害
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 20, attr_1 + attr_2 + attr_3 + attr_4 + attr_5 + attr_6)
		
		--local t = GET_COMBAT_UNIT_ATTR(unit_id, 12)
		--checkValue(userID, unit_id, -t)
	end
end
--调整所有的单位的移动
function adjust_units_move(userID)
	local unit_id = -1
    for i=0,9 do
		unit_id = GET_COMBAT_SIDE(userID, COMBAT_CAMP_ACTIVE, i)
		if unit_id>=0 then	adjust_unit_move(userID, unit_id) end
		unit_id = GET_COMBAT_SIDE(userID, COMBAT_CAMP_PASSIVE, i)
		if unit_id>=0 then	adjust_unit_move(userID, unit_id) end
    end
end
--根据轻功值调整单位移动
function adjust_unit_move(userID, unit_id)
	local qinggong = GET_COMBAT_UNIT_ATTR(unit_id, 10)
	local move_add = 0
	if qinggong >= 30 then move_add = 10 end
	if qinggong >= 60 then move_add = 20 end
	if qinggong >= 90 then move_add = 30 end
	local basemove = GET_COMBAT_UNIT_ATTR(unit_id, 5)
	SET_COMBAT_UNIT_ATTR(userID, unit_id, 5, basemove + move_add)
end
function adjust_monster_power(userID, startpower, endpower, years)
	--强度获得
	local time_pass = GET_ROLE_TIME(userID)
	local attr_1 = startpower + (endpower-startpower) * time_pass/(TP2YY * years)
	local attr_2 = startpower + (endpower-startpower) * time_pass/(TP2YY * years)
	local attr_3 = startpower + (endpower-startpower) * time_pass/(TP2YY * years)
	local attr_4 = startpower + (endpower-startpower) * time_pass/(TP2YY * years)
	local attr_5 = startpower + (endpower-startpower) * time_pass/(TP2YY * years)
	local attr_6 = startpower + (endpower-startpower) * time_pass/(TP2YY * years)
	--设置属性
	enemy = get_unit_by_side(userID, COMBAT_CAMP_PASSIVE)
	for i=0,9 do
		unit_id = enemy[i]
		if unit_id>=0 then
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 6, attr_1-10 + math.random(0,20))
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 7, attr_2-10 + math.random(0,20))
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 8, attr_3-10 + math.random(0,20))
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 9, attr_4-10 + math.random(0,20))
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 10, attr_5-10 + math.random(0,20))
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 11, attr_6-10 + math.random(0,20))
			--maxhp,hp
			SET_COMBAT_UNIT_ATTR(userID, unit_id, DYNAMIC_ATTR_HPMAX, attr_1*30)
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 3, attr_1*30)
			--maxmp,mp
			SET_COMBAT_UNIT_ATTR(userID, unit_id, DYNAMIC_ATTR_MPMAX, attr_3*20)
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 4, attr_3*20)
			--攻击,防御
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 12, attr_1*6)
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 13, attr_1*6)
			--命中,闪避
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 14, attr_4*6)
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 15, attr_4*6)
			--暴击,坚韧
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 16, attr_2*6)
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 17, attr_2*6)
			--精准,格挡
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 18, attr_3*6)
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 19, attr_3*6)
			--伤害
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 20, attr_1 + attr_2 + attr_3 + attr_4 + attr_5 + attr_6)
		end
	end
end
--按游戏时间调整单位强度（全体敌人)
function adjust_enemy_power(userID)
    for i=0,9 do
		local unit_id = GET_COMBAT_SIDE(userID, COMBAT_CAMP_PASSIVE, i)
		if unit_id>=0 then	adjust_unit_power(userID, unit_id, 0) end
    end
end
--位置是否可用
function pos_can_stand(userID, x, y)
	if GET_BATTLEFIELD_POS_BLOCK(userID, x, y) == 1 then
		if GET_COMBAT_UNIT_BY_POS(userID, x, y) < 0 then
			return true
		end
	end
	return false
end
--特定单位是否死亡
function heroDead(userID, heroID, campside)
    for i=0,9 do
		local unitID = GET_COMBAT_SIDE(userID,campside,i)
		if unitID >= 0 then
			if GET_COMBAT_UNIT_EXTRAATTR(userID, unitID, 3) == heroID then
				return false
			end
		end
    end
	return true
end
----------------------------------------------------------------------------------------------------------------------------------------
--净化处理
----------------------------------------------------------------------------------------------------------------------------------------
--负面效果
bad_buffs = {}
function init_bad_buffs()
	for i=1,10 do
		--属性修正1
		table.insert(bad_buffs, 11100+i )
		table.insert(bad_buffs, 21100+i )
		table.insert(bad_buffs, 31100+i )
		table.insert(bad_buffs, 41100+i )
		table.insert(bad_buffs, 51100+i )
		table.insert(bad_buffs, 61100+i )
		--属性修正2
		table.insert(bad_buffs, 111100+i )
		table.insert(bad_buffs, 121100+i )
		table.insert(bad_buffs, 131100+i )
		table.insert(bad_buffs, 141100+i )
		table.insert(bad_buffs, 151100+i )
		table.insert(bad_buffs, 161100+i )
		table.insert(bad_buffs, 171100+i )
		table.insert(bad_buffs, 181100+i )
		table.insert(bad_buffs, 191100+i )
	end
	--小伙伴特殊技能
	table.insert(bad_buffs, 10020201 ) --放下屠刀
	table.insert(bad_buffs, 10030000 ) --缴械
	table.insert(bad_buffs, 10040201 ) --刀光晃眼
	table.insert(bad_buffs, 10050201 ) --春秋刀法
	table.insert(bad_buffs, 10310101 ) --缴械
	table.insert(bad_buffs, 10310201 ) --缴械
	table.insert(bad_buffs, 10330301 ) --破防绝刀
	table.insert(bad_buffs, 10520201 ) --正气凌然
	table.insert(bad_buffs, 10630201 ) --花不堪折
end
init_bad_buffs()
function is_bad_buff(buff_id)
	for key in pairs(bad_buffs) do
		if buff_id == bad_buffs[key] then
			return true
		end
	end
	return false
end
function has_bad_buff(unit_id)
	local buffs = GET_COMBAT_UNIT_BUFF(unit_id)
	for key in pairs(buffs) do
		if is_bad_buff(buffs[key]) then return true end
	end
	return false
end
function clear_bad(userID, unit_id)
	for key in pairs(bad_buffs) do
		DEL_COMBAT_UNIT_BUFF(userID, unit_id, bad_buffs[key])
	end
end
--控制技能
control_buffs = {}
function init_control_buffs()
	for i=1,5 do
		table.insert(control_buffs, 1000+i ) --行动
		table.insert(control_buffs, 1100+i ) --移动
		table.insert(control_buffs, 1200+i ) --普攻
		table.insert(control_buffs, 1300+i ) --招式
		table.insert(control_buffs, 1400+i ) --道具
	end
	table.insert(control_buffs, 10150401)	--诗情画意
	table.insert(control_buffs, 10160201)	--落花有意
	table.insert(control_buffs, 10170401)	--移形换位
	table.insert(control_buffs, 10200201)	--铁扇封穴
	table.insert(control_buffs, 10550302)	--魅惑魔音
	table.insert(control_buffs, 10550303)	--魅惑魔音
	table.insert(control_buffs, 10550401)	--沉鱼落雁
end
init_control_buffs()
function is_control_buff(buff_id)
	for key in pairs(control_buffs) do
		if buff_id == control_buffs[key] then
			return true
		end
	end
	return false
end
function has_control_buff(unit_id)
	local buffs = GET_COMBAT_UNIT_BUFF(unit_id)
	for key in pairs(buffs) do
		if is_control_buff(buffs[key]) then return true end
	end
	return false
end
function clear_control(userID, unit_id)
	for key in pairs(control_buffs) do
		DEL_COMBAT_UNIT_BUFF(userID, unit_id, control_buffs[key])
	end
end

dmgUP_buffs = {}
function init_dmgUP_buffs()
	for i=1,10 do
		--属性修正2
		table.insert(dmgUP_buffs, 190100+i )
		table.insert(dmgUP_buffs, 10120401+i )	--魔剑噬心
	end
	--小伙伴特殊技能
	table.insert(dmgUP_buffs, 10680301)	--兽血沸腾
	table.insert(dmgUP_buffs, 10600201)	--千锤百炼
	table.insert(dmgUP_buffs, 10600202)	--千锤百炼
	table.insert(dmgUP_buffs, 10600203)	--千锤百炼
	table.insert(dmgUP_buffs, 10600204)	--千锤百炼
	table.insert(dmgUP_buffs, 10600205)	--千锤百炼
	table.insert(dmgUP_buffs, 10600206)	--千锤百炼
end
init_dmgUP_buffs()
function is_dmgUP_buff(buff_id)
	for key in pairs(dmgUP_buffs) do
		if buff_id == dmgUP_buffs[key] then
			return true
		end
	end
	return false
end
function has_dmgUP_buff(unit_id)
	local buffs = GET_COMBAT_UNIT_BUFF(unit_id)
	for key in pairs(buffs) do
		if is_dmgUP_buff(buffs[key]) then return true end
	end
	return false
end

--持续伤害、毒
function clear_poison(userID, unit_id)
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10390101 )	--毒效1~5层
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10390102 )
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10390103 )
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10390104 )
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10390105 )
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10530000 )	--毒蛊标记
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10530201 )	--潜龙毒影
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10180301 ) --火云神掌
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10190201 )	--狂化邪毒
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10190301 )	--七步追魂
end

----------------------------------------------------------------------------------------------------------------------------------------
--其它
----------------------------------------------------------------------------------------------------------------------------------------
--输出特定值（仅用于测试）
function checkValue(userID, unit_id, value)
	local hp = GET_COMBAT_UNIT_ATTR(unit_id, 3)
	SET_COMBAT_UNIT_ATTR(userID, unit_id, 3, hp+value) --通过加血来输出值
end
--战场陷阱数量
function getTrapNum(userID)
	local trap_num = 0
	local traps = GET_COMBAT_TRAP_INFO(userID)
	for key in pairs(traps) do
		trap_num = trap_num +1
	end
	return trap_num
end
--是否PVP
function isPVP(unit_a, unit_b)
	local ai1 = GET_COMBAT_UNIT_AI(unit_a)
	local ai2 = GET_COMBAT_UNIT_AI(unit_b)
	if (ai1+ai2)==0 then
		return true
	else
		return false
	end
end
--普攻范围类型
function get_normal_attack_range(user_id, unit_id)
	local weapon = GET_COMBAT_UNIT_WEAPON_TYPE(user_id, unit_id)
	if weapon == 0 then return 204 end	--平拳
	if weapon == 1 then return 101 end	--刀命
	if weapon == 2 then return 102 end	--剑命
	if weapon == 3 then return 103 end	--棍命
	if weapon == 4 then return 104 end	--拳命
	if weapon == 5 then return 105 end	--暗命
	if weapon == 6 then return 106 end	--奇命
end
--添加标记
function add_mark(userID,unit_id, added)
	local t = GET_COMBAT_UNIT_ATTR(unit_id, 25)
	SET_COMBAT_UNIT_ATTR(userID, unit_id, 25, t + added) --以暗防值数据为标记
end
--获得标记
function get_mark(unit_id)
	return GET_COMBAT_UNIT_ATTR(unit_id, 25)
end
--获得标记
function set_mark(userID, unit_id, value)
	SET_COMBAT_UNIT_ATTR(userID, unit_id, 25, value)
end
--退出战斗
function quitBattle(userID)
	for i=0,9 do
		local unit = GET_COMBAT_SIDE(userID,COMBAT_CAMP_ACTIVE,i)
		if unit>=0 then
			SET_COMBAT_UNIT_ATTR(userID, unit, 3, 0)
		end
	end
end



























