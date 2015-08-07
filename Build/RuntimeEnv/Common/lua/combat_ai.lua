
local cc = require("combat_const")
local cm = require("combat_module")

local CA = {}
CA.ai_check_map = {}
function CA.register_ai_check(unit_type_id, func)
  CA.ai_check_map[unit_type_id] = func
end
function CA.get_ai_check(unit_type_id)
    return CA.ai_check_map[unit_type_id]
end

--空AI模板
CA.register_ai_check(0,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
  end
  local maxhp = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_HPMAX)
  local hp = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_HP)
  local maxmp = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_MPMAX)
  local mp = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_MP)
  
  cm.capi_funcs.set_unit_ai(user_id, unit_id, cc.AI_MODE_SKILL_0)
end)

--位置编码方法
local function get_ort_max(step)
  local max = 0
  for i=0,step do
    max = max + i * 4 --如果是hex就是6
  end
  return max
end
local function get_pos_from_id(id)
  local d = 4
  local a1 = 0
  local sn = id
  local n = math.ceil((1+math.pow(1+8*sn/d,0.5))/2)
  local step = n-1
  if n==1 then return {x=0,y=0} end
  local min_id = get_ort_max(step)+1
  local max_id = get_ort_max(step+1)
  if id==min_id then return {x=-step,y=0} end
  if id==max_id then return {x=step,y=0} end
  
  local index = id - min_id + 1
  local _x = -step+math.floor(index/2)
  local _y = (-1 + (index % 2)*2)*(step-math.abs(_x))
  return {x=_x,y=_y}
end
local function get_id_from_pos(x,y)
  local step = math.abs(x)+math.abs(y)
  if step==0 then return 0 end
  local n = step + 1  --step0 的通项为0，为a1
  local max_id = get_ort_max(n)
  local min_id = get_ort_max(n-1)+1
  if y==0 and x<0 then return min_id end
  if y==0 and x>0 then return max_id end
  if y>0 then return min_id + (x-(-step))*2 end
  if y<0 then return min_id + (x-(-step))*2-1 end
  return -1 --id从0开始，小于0即寻找id失败
end

local function p(_x,_y)
  return {x=_x,y=_y}
end
local function get_up(_x,_y)
  return {x=_x,y=_y-1}
end
local function get_down(_x,_y)
  return {x=_x,y=_y+1}
end
local function get_left(_x,_y)
  return {x=_x-1,y=_y}
end
local function get_right(_x,_y)
  return {x=_x+1,y=_y}
end
local function get_link(x,y,d)
  if d == 0 then return get_up(x,y) end
  if d == 1 then return get_right(x,y) end
  if d == 2 then return get_down(x,y) end
  if d == 3 then return get_left(x,y) end
end
local function add_step(user_id,o_x,o_y,tile,s,move)
  for i in pairs(tile) do
    --cm.log_info("<COMBAT CHECK> x:%d y:%d step:%d", tile[i].pos.x, tile[i].pos.y, tile[i].step)
    if tile[i].open then
      local x = tile[i].pos.x
      local y = tile[i].pos.y
      for d=0,3 do
        local _pos = get_link(x,y,d)
        if cm.level_funcs.pos_can_stand(user_id,_pos.x,_pos.y) then
          local _x = _pos.x-o_x
          local _y = _pos.y-o_y
          local _step = math.abs(_x)+math.abs(_y)
          local id = get_id_from_pos(_x,_y)
          if not tile[id] then
            tile[id]={pos=p(_pos.x,_pos.y),step=_step,open=true}
            if _step==move then tile[id].open = false end
            --cm.log_info("<COMBAT CHECK> _pos.x:%d _pos.y:%d step:%d", _pos.x, _pos.y, _step)
          end
        end
      end
      tile[i].open = false
    end
  end
end
local function get_reachable_tiles(user_id, unit_id)
  local move = math.floor(cm.capi_funcs.get_unit_attr(unit_id,cc.FIGHT_ATTR_MOVE)/10)
  --cm.log_info("<COMBAT MOVE> unit_id:%d move:%d", unit_id, move)
  --起始位置
  local tile = {}
  local x,y = cm.capi_funcs.get_unit_pos(unit_id)
  local id = get_id_from_pos(0,0)
  tile[id]={pos=p(x,y),step=0,open=true}
  for i=1,move+1 do --多遍历一次，用于防止遍历时插入引起的遗漏
    add_step(user_id,x,y,tile,i-1,move)
  end
  return tile
end


---------------------------------------------------------------------------------
--以下为群侠
---------------------------------------------------------------------------------

--仙音
CA.register_ai_check(2101001,
function (user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7010)  --开场喊话
  end
  
  --local tile = get_reachable_tiles(user_id, unit_id)  --可移动范围（未去除阻挡及单位
  --for i in pairs(tile) do
  --  cm.log_info("<COMBAT TILES> x:%d y:%d s:%d", tile[i].pos.x, tile[i].pos.y, tile[i].step)
  --end
  
end)

--江瑜
CA.register_ai_check(2101002,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7020)  --开场喊话
  end
end)

--西门峰
CA.register_ai_check(2101003,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7030)  --开场喊话
  end
end)

--贾云长
CA.register_ai_check(2101004,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7040)  --开场喊话
  end
end)

--关伟
CA.register_ai_check(2101005,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7050)  --开场喊话
  end
end)

--方云华
CA.register_ai_check(2101006,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7060)  --开场喊话
  end
end)

--齐丽
CA.register_ai_check(2101007,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7070)  --开场喊话
  end
end)

--徐子易
--CA.register_ai_check(2101008,
--function(user_id, unit_id)
--  local turn = GET_COMBAT_ROUND_NUM(user_id)
--  if turn == 1 then
--    cm.level_funcs.show_movie(user_id, 7080)  --开场喊话
--  end
--end)

--江天雄
CA.register_ai_check(2101009,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7090)  --开场喊话
  end
end)

--无瑕子
CA.register_ai_check(2101010,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7100)  --开场喊话
  end
end)

--玄冥子
CA.register_ai_check(2101011,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7110)  --开场喊话
  end
end)

--荆棘
CA.register_ai_check(2101012,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7120)  --开场喊话
  end
  
  local maxhp = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_HPMAX)
  local hp = GET_COMBAT_UNIT_ATTR(unit_id, 3)
  
  if hp/maxhp >0.5 and not cm.skill_funcs.target_has_buff(unit_id, 10520301) then
    cm.capi_funcs.set_unit_ai(user_id, unit_id, 30)  --狂刀饮血
  else
    cm.capi_funcs.set_unit_ai(user_id, unit_id, 2) --普攻 / 刀光剑影
  end
end)

--老胡
--CA.register_ai_check(2101013,
--function(user_id, unit_id)
--  local turn = GET_COMBAT_ROUND_NUM(user_id)
--  if turn == 1 then
--    cm.level_funcs.show_movie(user_id, 7130)  --开场喊话
--  end
--end)

--三师兄
CA.register_ai_check(2101014,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7140)  --开场喊话
  end
end)

--丹青
CA.register_ai_check(2101015,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7150)  --开场喊话
  end
end)

--花翁
CA.register_ai_check(2101016,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7160)  --开场喊话
  end
end)

--橘叟
CA.register_ai_check(2101017,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7170)  --开场喊话
  end
end)

--神医
CA.register_ai_check(2101018,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7180)  --开场喊话
  end
end)

--怪医
CA.register_ai_check(2101019,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7190)  --开场喊话
  end
end)

--书生
CA.register_ai_check(2101020,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7200)  --开场喊话
  end
end)

--醉仙
CA.register_ai_check(2101021,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7210)  --开场喊话
  end
end)

--沈湘云
CA.register_ai_check(2101022,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7220)  --开场喊话
  end
end)

--夜叉
CA.register_ai_check(2101023,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7230)  --开场喊话
  end
end)

--吃
CA.register_ai_check(2101024,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7240)  --开场喊话
  end
end)

--喝
CA.register_ai_check(2101025,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7250)  --开场喊话
  end
end)

--嫖
CA.register_ai_check(2101026,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7260)  --开场喊话
  end
end)

--赌
CA.register_ai_check(2101027,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7270)  --开场喊话
  end
end)

--铁面
CA.register_ai_check(2101028,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7280)  --开场喊话
  end
end)

--银钩
CA.register_ai_check(2101029,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7290)  --开场喊话
  end
end)

--虚真
CA.register_ai_check(2101030,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7300)  --开场喊话
  end
end)

--丁冲
CA.register_ai_check(2101031,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7310)  --开场喊话
  end
end)

--史燕
CA.register_ai_check(2101032,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7320)  --开场喊话
  end
end)

--夏侯非
CA.register_ai_check(2101033,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7330)  --开场喊话
  end
end)

--摩呼罗迦
CA.register_ai_check(2101034,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7340)  --开场喊话
  end
end)

--紫阳子
CA.register_ai_check(2101035,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7350)  --开场喊话
  end
end)

--司马玄
CA.register_ai_check(2101036,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7360)  --开场喊话
  end
end)

--巩光杰
CA.register_ai_check(2101037,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7370)  --开场喊话
  end
end)

--夏侯城
CA.register_ai_check(2101038,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7380)  --开场喊话
  end
end)

--唐冠南
CA.register_ai_check(2101039,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7390)  --开场喊话
  end
end)

--唐中慧
CA.register_ai_check(2101040,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7400)  --开场喊话
  end
end)

--唐飞
CA.register_ai_check(2101041,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7410)  --开场喊话
  end
end)

--纪纹
CA.register_ai_check(2101042,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7420)  --开场喊话
  end
end)

--商鹤鸣
CA.register_ai_check(2101043,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7430)  --开场喊话
  end
end)

--卓人清
CA.register_ai_check(2101044,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7440)  --开场喊话
  end
end)

--古实
CA.register_ai_check(2101045,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7450)  --开场喊话
  end
end)

--无慧
CA.register_ai_check(2101046,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7460)  --开场喊话
  end
end)

--沈丘山 少年
--local function ai_check_2101047(user_id, unit_id)
--  local turn = GET_COMBAT_ROUND_NUM(user_id)
--  if turn == 1 then
--    cm.level_funcs.show_movie(user_id, 7470)  --开场喊话
--  end
--end
--CA.register_ai_check(2101047, ai_check_2101047)

--沈丘山
CA.register_ai_check(2101048,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7480)  --开场喊话
  end
end)

--雷震天
CA.register_ai_check(2101049,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7490)  --开场喊话
  end
end)

--赛飞鸿 怪物
CA.register_ai_check(2101050,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7500)  --开场喊话
  end
  --万夫莫敌
  local x,y = GET_COMBAT_UNIT_POSITION(unit_id)
  local enemyNum = cm.skill_funcs.get_enemy_around(user_id, unit_id, x, y)
  if enemyNum>0 then
    ADD_UNIT_BUFF(user_id, unit_id, unit_id, 190101 - 1 + enemyNum)
  end
end)

--赵惊风
CA.register_ai_check(2101051,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7510)  --开场喊话
  end
end)

--谷月轩
CA.register_ai_check(2101052,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7520)  --开场喊话
  end
  
  local maxhp = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_HPMAX)
  local hp = GET_COMBAT_UNIT_ATTR(unit_id, 3)
  local maxmp = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_MPMAX)
  local mp = GET_COMBAT_UNIT_ATTR(unit_id, 4)
  
  if hp/maxhp < 0.5 and (maxhp * mp/maxmp - hp) > maxhp * 0.2 then  --能回复20%以上的血
    cm.capi_funcs.set_unit_ai(user_id, unit_id, 40)  --逆劲护元
  else
    if not cm.skill_funcs.target_has_buff(unit_id, 10520301) then
      if mp/maxmp > 0.6 then
        cm.capi_funcs.set_unit_ai(user_id, unit_id, 30)  --逍遥身法
      end
    else
      cm.capi_funcs.set_unit_ai(user_id, unit_id, 20)  --正气凛人
    end
  end
end)

--蓝婷
CA.register_ai_check(2101053,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7530)  --开场喊话
  end
end)

--罗那
CA.register_ai_check(2101054,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7540)  --开场喊话
  end
end)

--香儿
CA.register_ai_check(2101055,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7550)  --开场喊话
  end
end)

--高胜
CA.register_ai_check(2101056,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7560)  --开场喊话
  end
  
  local maxmp = GET_COMBAT_UNIT_ATTR(unit_id, cc.FIGHT_ATTR_MPMAX)
  local mp = GET_COMBAT_UNIT_ATTR(unit_id, 4)
  
  if mp/maxmp >0.8 then
    cm.capi_funcs.set_unit_ai(user_id, unit_id, 30)  --剑啸昆仑
  else
    if math.random(1,10)>4 then
      cm.capi_funcs.set_unit_ai(user_id, unit_id, 40)  --人为棋阵
    else
      cm.capi_funcs.set_unit_ai(user_id, unit_id, 10)  --普攻
    end
  end
end)

--叶孤
CA.register_ai_check(2101057,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7570)  --开场喊话
  end
end)

--萧遥
CA.register_ai_check(2101058,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7580)  --开场喊话
  end
end)

--商仲仁
CA.register_ai_check(2101059,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7590)  --开场喊话
  end
end)

--任剑南
CA.register_ai_check(2101060,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7600)  --开场喊话
  end
end)

--史刚
CA.register_ai_check(2101061,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7610)  --开场喊话
  end
end)

--曹萼华
CA.register_ai_check(2101062,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7620)  --开场喊话
  end
end)

--任清璇
CA.register_ai_check(2101063,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7630)  --开场喊话
  end
end)

--傅剑寒
CA.register_ai_check(2101064,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7640)  --开场喊话
  end
end)

--西门玄 2300138
CA.register_ai_check(2300138,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7650)  --开场喊话
  end
end)

--灵相
--CA.register_ai_check(2101066,
--function(user_id, unit_id)
--  local turn = GET_COMBAT_ROUND_NUM(user_id)
--  if turn == 1 then
--    cm.level_funcs.show_movie(user_id, 7660)  --开场喊话
--  end
--end)

--空缺
--local function ai_check_2101067(user_id, unit_id)
--  local turn = GET_COMBAT_ROUND_NUM(user_id)
--  if turn == 1 then
--    cm.level_funcs.show_movie(user_id, 7670)  --开场喊话
--  end
--end
--CA.register_ai_check(2101067, ai_check_2101067)

--万凯
CA.register_ai_check(2101068,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7680)  --开场喊话
  end
end)

--杨云
CA.register_ai_check(2101069,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 7690)  --开场喊话
  end
end)

--关长虹
CA.register_ai_check(2300123,
function(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  if turn == 1 then
    cm.level_funcs.show_movie(user_id, 8123)  --开场喊话
  end
end)






---------------------------------------------------------------------------------
--以下为剧情怪物
---------------------------------------------------------------------------------

--叶枫
local function ai_check_2200017(user_id, unit_id)
  local turn = GET_COMBAT_ROUND_NUM(user_id)
  local ai_type = cm.level_funcs.get_mark(unit_id)
  if ai_type == 1 then
    cm.level_funcs.show_movie(user_id, 6009)  --逃跑宣言
    cm.level_funcs.set_mark(user_id,unit_id,2)
    
    SET_COMBAT_UNIT_ATTR(user_id, unit_id, cc.FIGHT_ATTR_MOVE, 40) --移动加成
    ADD_UNIT_BUFF(user_id, unit_id, unit_id, 10320401)  --神行百变
    ADD_UNIT_BUFF(user_id, unit_id, unit_id, 141001)  --闪避+9999 3回合
    --需要加一个提示脱离位置的陷阱（无法触发）
      
    if cm.level_funcs.pos_can_stand(user_id,10,1) then
      if cm.level_funcs.pos_can_stand(user_id,10,0) then
        cm.checkValue(user_id,unit_id,-2)
        SET_COMBAT_UNIT_MOTORMODE(unit_id, 10, 0, 0, cc.AI_MODE_SKILL_0)
      else
        SET_COMBAT_UNIT_MOTORMODE(unit_id, 10, 0, -1, cc.AI_MODE_SKILL_0)--关闭寻路
        cm.level_funcs.set_mark(user_id,unit_id,3)  --优先攻击10,0位置单位的标记
        --cm.checkValue(user_id,unit_id,-3)
      end
    else
      SET_COMBAT_UNIT_MOTORMODE(unit_id, 10, 0, -1, cc.AI_MODE_SKILL_0)--关闭寻路
      cm.level_funcs.set_mark(user_id,unit_id,4)  --优先攻击10,1位置单位的标记
      --cm.checkValue(user_id,unit_id,-4)
      --SET_COMBAT_UNIT_MOTORMODE(unit_id, 10, 2, 0, cc.AI_MODE_SKILL_0)
    end
  end
  --已经做过逃跑宣言，每回合行动时都检查是否向目标位置寻路
  if ai_type >1 then
    if cm.level_funcs.pos_can_stand(user_id,10,1) then
      if cm.level_funcs.pos_can_stand(user_id,10,0) then
        cm.checkValue(user_id,unit_id,-2)
        SET_COMBAT_UNIT_MOTORMODE(unit_id, 10, 0, 0, cc.AI_MODE_SKILL_0)
      else
        SET_COMBAT_UNIT_MOTORMODE(unit_id, 10, 0, -1, cc.AI_MODE_SKILL_0)--关闭寻路
        cm.level_funcs.set_mark(user_id,unit_id,3)  --优先攻击10,0位置单位的标记
        --cm.checkValue(user_id,unit_id,-3)
      end
    else
      SET_COMBAT_UNIT_MOTORMODE(unit_id, 10, 0, -1, cc.AI_MODE_SKILL_0)--关闭寻路
      cm.level_funcs.set_mark(user_id,unit_id,4)  --优先攻击10,1位置单位的标记
      --cm.checkValue(user_id,unit_id,-4)
      --SET_COMBAT_UNIT_MOTORMODE(unit_id, 10, 2, 0, cc.AI_MODE_SKILL_0)
    end
  end
end
CA.register_ai_check(2200017, ai_check_2200017)







return CA
