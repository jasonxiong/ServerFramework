#todo 在后面实际使用过程中，这个地方需要根据实际容量进行数据量的评估，对数据进行分库，并且根据分库创建数据库的database

drop database if exists UniqueUinDB;
create database UniqueUinDB;
use UniqueUinDB;

create table IF NOT EXISTS t_crystal_uniquindata
(
	uin int unsigned not null auto_increment,
	primary key(uin)
)auto_increment=1001 engine = innodb default charset=utf8;
