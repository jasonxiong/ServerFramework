#todo 在后面实际使用过程中，这个地方需要根据实际容量进行数据量的评估，对数据进行分库，并且根据分库创建数据库的database

drop database if exists 1_RoleDB;
create database 1_RoleDB;
use 1_RoleDB;

create table IF NOT EXISTS t_crystal_userdata
(
	uin int unsigned not null,
	seq int unsigned not null,
	base_info blob,
	quest_info blob,
	item_info blob,
	fight_info blob,
	friend_info blob,
	reserved1 blob,
	reserved2 blob,
	primary key(uin, seq)
)engine = innodb default charset=utf8;

ALTER TABLE t_crystal_userdata ROW_FORMAT = COMPRESSED;
