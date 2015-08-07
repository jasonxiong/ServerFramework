#todo 在后面实际使用过程中，这个地方需要根据实际容量进行数据量的评估，对数据进行分库，并且根据分库创建数据库的database
#todo jasonxiong 这个数据库的分库分表后续在实际开发中再确定
#todo jasonxiong 帐号数据库中需要存储的字段后续也需要确定，并且可能需要优化

drop database if exists 1_AccountDB;
create database 1_AccountDB;
use 1_AccountDB;

create table IF NOT EXISTS t_crystal_accountdata
(
	accountID varchar(64) not null,
	accountType tinyint not null,
	uin int unsigned not null,
	password varchar(128) not null,
	lastWorldID int unsigned,
	isBinded tinyint not null,

	primary key(accountID,accountType)
)engine = innodb default charset=utf8;
