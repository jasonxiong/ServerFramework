#初始化数据库帐号和权限

#insert into mysql.user(Host,User,Password) values("*","crystalonline",password("crystalonline20140227"));

GRANT ALL PRIVILEGES ON *.* TO 'crystalonline'@'%' IDENTIFIED BY 'crystalonline20140227' WITH GRANT OPTION; 
GRANT ALL PRIVILEGES ON *.* TO 'crystalonline'@'localhost' IDENTIFIED BY 'crystalonline20140227' WITH GRANT OPTION; 

flush privileges;
