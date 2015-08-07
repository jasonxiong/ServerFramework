#!/bin/sh

#创建MYSQL帐号并初始化权限
mysql -uroot -pcrystalonline20140227 < GrantPrivileges.sql 
mysql -ucrystalonline -pcrystalonline20140227 < InstallAccountDB.sql
mysql -ucrystalonline -pcrystalonline20140227 < InstallNameDB.sql 
mysql -ucrystalonline -pcrystalonline20140227 < InstallRoleDB.sql
mysql -ucrystalonline -pcrystalonline20140227 < InstallUniqUinDB.sql
mysql -ucrystalonline -pcrystalonline20140227 < InstallWorldOnlineDB.sql

