--  C++提供给lua调用的接口,只供编程使用，不加载

--获取战斗阵营战斗单位unitid,-1表示非法
function GET_COMBAT_SIDE(uin, camp, unit_index)
	return -1
end

--获取战斗单位的属性信息
function GET_COMBAT_UNIT_ATTR(unit_id, attr_type)
	print('GET_COMBAT_UNIT_ATTR')
	return 100
end
--[[
	DYNAMIC_ATTR_XIANGONG = 30, // 先攻值属性
    DYNAMIC_ATTR_QINGGONG = 31, // 轻功距离属性
    DYNAMIC_ATTR_HPMAX = 32, // 生命上限属性
    DYNAMIC_ATTR_MPMAX = 33, // 内力上限属性
]]--

--获取战斗单位的阵营信息
function GET_COMBAT_UNIT_CAMP(uin, unit_id)
	print('GET_COMBAT_UNIT_CAMP')
	return 1
end

--设置战斗胜负
function SET_CAMP_ACTIVE_WIN(uin, resultType)
	print('SET_CAMP_ACTIVE_WIN:' .. resultType)
	return 1
end

--获取战斗回合数
function GET_COMBAT_ROUND_NUM(uin)
	--print('GET_COMBAT_ROUND_NUM')
	battleTurn = battleTurn +1
	return battleTurn
end

--判断战斗回合是否结束
function IS_COMBAT_ROUND_END(uin)
	--print('IS_COMBAT_ROUND_END')
	return true
end

--添加战斗单位, 返回<0失败，>=0表示unitID
function ADD_COMBAT_UNIT(uin, camp, config_id, posx, posy, direction)
	print('ADD_COMBAT_UNIT:' .. config_id)
	return 0
end

--获取战斗单位的AI
function GET_COMBAT_UNIT_AI(unit_id)
	print('GET_COMBAT_UNIT_AI')
end

--切换战斗NPC单位的AI
function CHANGE_COMBAT_NPC_AI(uin, unit_id, newAI)
	print('CHANGE_COMBAT_NPC_AI')
end

--添加额外的战斗掉落
--is_immediate: 1表示立即增加奖励; 0表示战斗结束后增加奖励
function ADD_EXTRA_COMBAT_DROPID(uin, drop_id, is_immediate)
	print('ADD_EXTRA_COMBAT_DROPID')
end

--强制杀死战斗单位
function KILL_COMBAT_UNIT(uin, unit_id)
	print('KILL_COMBAT_UNIT')
end

--打印调试日志信息
function LOG_INFO(...)
	print(string.format(...))
end

--战场上增加机关
function ADD_COMBAT_TRAP(uin, camp, trap_id, posx, posy, face)
    print('ADD_COMBAT_TRAP')
end

--删除战场上的机关
function DELETE_COMBAT_TRAP(uin, trap_obj_id)
    print('DELETE_COMBAT_TRAP')
end

--触发战场上的机关
function TRIGGER_COMBAT_TRAP(uin, trap_obj_id)
    print('TRIGGER_COMBAT_TRAP')
end

--给战斗单位添加buff
function ADD_UNIT_BUFF(uin, cast_unit_id, target_unit_id, buff_id)
    print('ADD_UNIT_BUFF')
end

--取战场上所有机关的{config_id=trap_obj_id, ...}
function GET_COMBAT_TRAP_INFO(uin)
    print('GET_COMBAT_TRAP_INFO')
    return ({{1,2},})
end

--获取战斗单位的面向，小于0表示出错
function GET_UNIT_FACE(unit_id)
    print('GET_UNIT_FACE')
    return 2
end

--获取战斗单位攻击的方向,小于0表示出错
function GET_ATTACK_FACE(cast_unit_id, target_unit_id)
    print('GET_ATTACK_FACE')
    return 2
end

--获取战斗单位的当前坐标x,y, x<0 表示非法的位置
function GET_COMBAT_UNIT_POSITION(unit_id)
    print('GET_COMBAT_UNIT_POSITION')
    return 1,2
end

--获取战斗单位额外属性，类型定义在combat_common.lua中,返回获取的值
--COMBAT_EXTRAATTR_GENDER = 1	--战斗单位的性别
--COMBAT_EXTRAATTR_GOOD = 2		--善恶值
--COMBAT_EXTRAATTR_CONFIGID = 3	--战斗单位配置的ID
function GET_COMBAT_UNIT_EXTRAATTR(uin, unit_id, extra_attr_type)
    print('GET_COMBAT_UNIT_EXTRAATTR')
    return 1
end

--获取战斗单位身上的装备ID，没有返回0
function GET_COMBAT_UNIT_EQUIP(uin, unit_id, equip_type)
    print('GET_COMBAT_UNIT_EXTRAATTR')
    return 0
end

--获取战斗单位武器的类型，没有返回0
function GET_COMBAT_UNIT_WEAPON_TYPE(unit_id)
    print('GET_COMBAT_UNIT_WEAPON_TYPE')
    return 0
end

--设置战斗单位的属性数值
function SET_COMBAT_UNIT_ATTR(uin, unit_id, attr_type, attr_value)
    print('SET_COMBAT_UNIT_ATTR')
end

--获取战斗单位身上的BUFF配置ID列表, <0表示非法
function GET_COMBAT_UNIT_BUFF(unit_id)
    print('GET_COMBAT_UNIT_BUFF')
    return {1,2,3,4,5}
end

--删除战斗单位身上的BUFF
function DEL_COMBAT_UNIT_BUFF(uin, unit_id, buff_id)
    print('DEL_COMBAT_UNIT_BUFF')
end

--获取到目标单位的攻击距离 1-7， 返回0表示攻击不到
function GET_TARGET_ATTACK_DISTANCE(cast_unit_id, target_unit_id, target_range_id)
    print('GET_TARGET_ATTACK_DISTANCE')
    return 3
end

--获取战斗当前的状态,status_type 为0-7，返回0，1
function GET_COMBAT_STATUS(uin, status_type)
    print('GET_COMBAT_STATUS')
    return 1
end

--设置战斗当前的状态, status_type 为0-7, is_status_set为0 1,表示设置清除状态
function SET_COMBAT_STATUS(uin, status_type, is_status_set)
    print('SET_COMBAT_STATUS')
end

--脚本使用战斗技能
function UNIT_CAST_SKILL(uin, cast_unit_id, target_pos_x, target_pos_y, skill_id)
    print('UNIT_CAST_SKILL')
end

--对战斗单位进行特殊位移,如果area_id==0表示直接传送，move_type表示位移类型
--move_type的类型: 
--  COMBAT_UNIT_MOVE_NORMAL = 1		--普通移动，正常寻路的
--  COMBAT_UNIT_MOVE_TELEPORT = 2	--传送，可以是任意位置	没有路径的过程，瞬间到达
--  COMBAT_UNIT_MOVE_JUMP = 3		--跳跃移动				位移过程有动画（约定位移动画编号，如FPS15_0？）
--  COMBAT_UNIT_MOVE_RUSH = 4		--冲锋移动				位移后播放动画（约定位移动画编号，如FPS15_1？）
function JUMP_COMBAT_UNIT(uin, unit_id, area_id, target_posx, target_posy, move_type, anim_type)
    print('JUMP_COMBAT_UNIT')
end

--根据战场位置获取战斗单位unit_id, <0 表示没有
function GET_COMBAT_UNIT_BY_POS(uin, pos_x, pos_y)
    print('GET_COMBAT_UNIT_BY_POS')
    return 1
end

--设置战场战斗单位的机动模式
function SET_COMBAT_UNIT_MOTORMODE(unit_id, target_posx, target_posy, distance, ai_id)
    print('SET_COMBAT_UNIT_MOTORMODE')
end

--设置战斗单位身上的状态
--status_type 的类型：
--  COMBAT_UNIT_STATUS_NOTMOVE = 1,		禁止移动
--  COMBAT_UNIT_STATUS_NOTACTION = 2,	禁止行动
--	COMBAT_UNIT_STATUS_NOTROUND = 3,	禁止回合
--  COMBAT_UNIT_STATUS_IGNOREJOINT = 4,	无视合击
--  COMBAT_UNIT_STATUS_UNBENDING = 5	不屈
--is_set：
--	0 表示清除状态
--	1 表示设置状态
function SET_COMBAT_UNIT_STATUS(unit_id, status_type, is_set)
	print('SET_COMBAT_UNIT_STATUS')
end

--获取unit_id单位最后攻击的战斗单位ID
function GET_LAST_ATTACK_UNIT(unit_id)
    print('GET_LAST_ATTACK_UNIT')
    return -1
end

--获取指定位置范围内某阵营单位的列表
function GET_AREA_TARGET_UNIT(uin, pos_x, pos_y, area_id, camp, direction)
    print('GET_AREA_TARGET_UNIT')
    return {}
end

--获取战斗单位的Score数值
function GET_UNIT_SCORE(unit_id, score_id)
    print('GET_UNIT_SCORE')
    return 0
end

function MOVIE_ACTION(userID, a,b,c)
    print('MOVIE_ACTION')
end

function GET_ROLE_TIME(userID)
    print('GET_ROLE_TIME')
    return 4
end

--获取主角使用技能的等级,返回0表示获取失败
function GET_ROLEUNIT_SKILL_LEVEL(uin, unit_id, skill_id)
    print('GET_ROLEUNIT_SKILL_LEVEL')
    return 1
end

--获取战场点的阻挡类型
--返回值含义: 1 可行走; 2 特殊阻挡可行走; 3 不可行走
function GET_BATTLEFIELD_POS_BLOCK(uin, pos_x, pos_y)
    print('GET_BATTLEFIELD_POS_BLOCK')
    return 1
end




