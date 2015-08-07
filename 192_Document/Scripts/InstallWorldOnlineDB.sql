
drop database if exists WorldOnlineDB;
create database WorldOnlineDB;

use WorldOnlineDB;

create table IF NOT EXISTS t_crystal_worldonline
(
	WorldID int unsigned not null, 
	ZoneID int unsigned not null, 
	OnlineRoleNum int unsigned, 
	RecordTime int unsigned,
	primary key(WorldID,ZoneID)
) engine=innodb default charset=utf8;
