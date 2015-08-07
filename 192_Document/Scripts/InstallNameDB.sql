#todo 在后面实际使用过程中，这个地方需要根据实际容量进行数据量的评估，对数据进行分库，并且根据分库创建数据库的database

drop database if exists 1_NameDB;
create database 1_NameDB;
use 1_NameDB;

create table IF NOT EXISTS t_crystal_nameinfo
(
	nickname varchar(64) not null,
	nametype tinyint not null,
	nameid int unsigned not null,

	primary key(nickname, nametype)
)engine = innodb default charset=utf8;
