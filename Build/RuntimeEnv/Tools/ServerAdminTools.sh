#!/bin/sh

#所有服务器的管理工具

#服务器程序的部署安装路径
QMONSTER_RUNTIME_ENV_PATH=/home/wulinonlinetest/RuntimeEnv

#更新包在服务器上存放的路径
QMONSTER_UPDATE_PACKAGE_PATH=/data/qmonster/Packages

#脚本使用帮助文档
Usage()
{
	echo "Usage: ./ServerAdminTools.sh -I <SERVERIP> <ACCOUNT_PORT> <ZONE_PORT> <REGAUTH_PORT> <WORLDID> <ZONEID> <DBUser> <DBPasswd>"
	echo "                        --Init Server Project"
	echo "       ./ServerAdminTools.sh -D <PackageName>"
	echo "                        --Deploy Server update package to runtime env"
	echo "       ./ServerAdminTools.sh -K"
	echo "                        --Kill All Servers of local env"
	echo "       ./ServerAdminTools.sh -S"
	echo "                        --Restart All Servers of local env"

	echo "       ./ServerAdminTools.sh -R"
	echo "                        --Reload All Servers of local env"

	exit 1
}

#初始化所有Server
Init_Server()
{
	if [ $# -ne 8 ]
	then
		echo "Usage: ./ServerAdminTools.sh -I <SERVERIP> <ACCOUNT_PORT> <ZONE_PORT> <REGAUTH_PORT> <WORLDID> <ZONEID> <DBUser> <DBPasswd>"
		exit 1
	fi

	#如果原来的运行时环境存在，先清空
	if [ -d "$QMONSTER_RUNTIME_ENV_PATH" ]
	then
		rm -rf $QMONSTER_RUNTIME_ENV_PATH
	fi

	#建立运行时环境
	mkdir -p $QMONSTER_RUNTIME_ENV_PATH

	cp ../* $QMONSTER_RUNTIME_ENV_PATH/ -rf

	cd $QMONSTER_RUNTIME_ENV_PATH/
	
	#设置AccountGatewayServer绑定的服务器IP
	perl -i -p -e 's/'#ACCOUNT_GATEWAY_IP#'/'$1'/g' World/AccountGatewayServer/conf/LotusListener.tcm

	#设置AccountGatewayServer绑定的服务器端口
	perl -i -p -e 's/'#ACCOUNT_GATEWAY_PORT#'/'$2'/g' World/AccountGatewayServer/conf/LotusListener.tcm

	#设置ZoneGatewayServer绑定的服务器IP
	perl -i -p -e 's/'#ZONE_GATEWAY_IP#'/'$1'/g' Zone/ZoneGatewayServer/conf/LotusListener.tcm

	#设置ZoneGatewayServer绑定的服务器端口
	perl -i -p -e 's/'#ZONE_GATEWAY_PORT#'/'$3'/g' Zone/ZoneGatewayServer/conf/LotusListener.tcm

	#设置RegAuthGatewayServer绑定的服务器IP
	perl -i -p -e 's/'#REGAUTH_GATEWAY_IP#'/'$1'/g' Global/RegAuthGatewayServer/conf/LotusListener.tcm
	
	#设置RegAuthGatewayServer绑定的服务器端口
	perl -i -p -e 's/'#REGAUTH_GATEWAY_PORT#'/'$4'/g' Global/RegAuthGatewayServer/conf/LotusListener.tcm

	#设置所有配置文件中的服务器通信BUS
	perl -i -p -e 's/'#WORLDID#'/'$5'/g' Zone/ZoneServer/conf/GameServer.tcm World/AccountServer/conf/GameServer.tcm World/RoleDBServer/conf/GameServer.tcm World/WorldServer/conf/GameServer.tcm World/NameDBServer/conf/GameServer.tcm

	perl -i -p -e 's/'#ZONEID#'/'$6'/g' Zone/ZoneServer/conf/GameServer.tcm World/WorldServer/conf/GameServer.tcm

	perl -i -p -e 's/'#SERVERIP#'/'$1'/g' Zone/ZoneServer/conf/GameServer.tcm Global/RegAuthServer/conf/GameServer.tcm Global/AccountDBServer/conf/GameServer.tcm World/AccountServer/conf/GameServer.tcm World/RoleDBServer/conf/GameServer.tcm World/WorldServer/conf/GameServer.tcm World/NameDBServer/conf/GameServer.tcm

	#设置游戏使用DB的详细信息
	perl -i -p -e 's/'#DB_IP#'/'$1'/g' World/RoleDBServer/conf/DBMSConf.xml World/NameDBServer/conf/DBMSConf.xml Global/AccountDBServer/conf/DBMSConf_UniqUin.xml Global/AccountDBServer/conf/DBMSConf.xml
	perl -i -p -e 's/'#DB_USER#'/'$7'/g' World/RoleDBServer/conf/DBMSConf.xml World/NameDBServer/conf/DBMSConf.xml Global/AccountDBServer/conf/DBMSConf_UniqUin.xml Global/AccountDBServer/conf/DBMSConf.xml
	perl -i -p -e 's/'#DB_PASSWD#'/'$8'/g' World/RoleDBServer/conf/DBMSConf.xml World/NameDBServer/conf/DBMSConf.xml Global/AccountDBServer/conf/DBMSConf_UniqUin.xml Global/AccountDBServer/conf/DBMSConf.xml
					
	#World中的所有分区服务器在WorldServer/conf下面，是需要手动配置的

	#设置所有启动脚本的启动参数
	perl -i -p -e 's/'#WORLDID#'/'$5'/g' World/AccountServer/admin/ScriptFrameSvr.sh
	perl -i -p -e 's/'#WORLDID#'/'$5'/g' World/RoleDBServer/admin/ScriptFrameSvr.sh
	perl -i -p -e 's/'#WORLDID#'/'$5'/g' World/WorldServer/admin/ScriptFrameSvr.sh
	perl -i -p -e 's/'#WORLDID#'/'$5'/g' Zone/ZoneServer/admin/ScriptFrameSvr.sh
	perl -i -p -e 's/'#ZONEID#'/'$6'/g' Zone/ZoneServer/admin/ScriptFrameSvr.sh
	perl -i -p -e 's/'#WORLDID#'/'$5'/g' World/NameDBServer/admin/ScriptFrameSvr.sh	

	echo "Success to Init Server development env......."
	echo "Have a good time......"
	echo "Finished!"

	exit 1
}

#更新部署服务器
Deploy_Server()
{
	#先停掉所有的Server
	Kill_Server
	
	#直接将安装包解压到对应目录即可
	cd $QMONSTER_UPDATE_PACKAGE_PATH
	
	
	tar -zxvf $1 -C $QMONSTER_RUNTIME_ENV_PATH --exclude LotusListener.tcm --exclude *svn* --exclude log --exclude conf --exclude TCP* --exclude ScriptFrameSvr* --exclude bills --exclude admin

	#重启所有的服务
	Start_Server

	echo "Success to deploy server runtime env!!!"

	exit 0	
}

#停掉所有的Server
Kill_Server()
{
	#停止所有的服务
	cd $QMONSTER_RUNTIME_ENV_PATH

	#停止AccountGatewayServer
	cd World/AccountGatewayServer/admin
	./stop.sh
	cd ../../../

	#停止AccountServer
	cd World/AccountServer/admin
	./stop.sh
	cd ../../../

	#停止ZoneGatewayServer
	cd Zone/ZoneGatewayServer/admin
	./stop.sh
	cd ../../..

	#停止ZoneServer
	cd Zone/ZoneServer/admin
	./stop.sh
	cd ../../..

	sleep 8

	#停止WorldServer
	cd World/WorldServer/admin
	./stop.sh
	cd ../../../

	#停止RoleDBServer
	cd World/RoleDBServer/admin
	./stop.sh
	cd ../../../

	#停止NameDBServer
	cd World/NameDBServer/admin
	./stop.sh
	cd ../../../

	#去掉Cluster
	#停止ClusterServer
	#cd Cluster/ClusterServer/admin
	#./stop.sh
	#cd ../../..

	#停止RegAuthGatewayServer
	cd Global/RegAuthGatewayServer/admin
	./stop.sh
	cd ../../..

	#停止RegAuthServer
	cd Global/RegAuthServer/admin
	./stop.sh
	cd ../../..

	#停止AccountDBServer
	cd Global/AccountDBServer/admin
	./stop.sh
	cd ../../..

	echo "Success to stop all servers!!!"
}

#启动所有的服务器
#todo jasonxiong 后面需要按功能进行服务器的分类启动
Start_Server()
{
	#重新启动所有的服务
	cd $QMONSTER_RUNTIME_ENV_PATH

	#启动AccountGatewayServer
	cd World/AccountGatewayServer/admin
	./restart.sh
	cd ../../../

	#启动AccountServer
	cd World/AccountServer/admin
	./restart.sh
	cd ../../../

	#启动WorldServer
	cd World/WorldServer/admin
	./restart.sh
	cd ../../../

	#启动RoleDBServer
	cd World/RoleDBServer/admin
	./restart.sh
	cd ../../../

	#启动NameDBServer
	cd World/NameDBServer/admin
	./restart.sh
	cd ../../../

	#启动ZoneGatewayServer
	cd Zone/ZoneGatewayServer/admin
	./restart.sh
	cd ../../..

	#启动ZoneServer
	cd Zone/ZoneServer/admin
	./restart.sh
	cd ../../..

	#去掉Cluster
	#启动ClusterServer
	#cd Cluster/ClusterServer/admin
	#./restart.sh
	#cd ../../..

	#启动RegAuthGatewayServer
	cd Global/RegAuthGatewayServer/admin
	./restart.sh
	cd ../../..
	
	#启动RegAuthServer
	cd Global/RegAuthServer/admin
	./restart.sh
	cd ../../..

	#启动AccountDBServer
	cd Global/AccountDBServer/admin
	./restart.sh
	cd ../../..

	echo "Success to restart all servers!!!"
}

#Reload所有的服务器
Reload_Server()
{
	#Reload所有的服务配置
	cd $QMONSTER_RUNTIME_ENV_PATH

	#重新加载AccountGatewayServer
	cd World/AccountGatewayServer/admin
	./reload.sh
	cd ../../../

	#重新加载AccountServer
	cd World/AccountServer/admin
	./reload.sh
	cd ../../../

	#重新加载WorldServer
	cd World/WorldServer/admin
	./reload.sh
	cd ../../../

	#重新加载RoleDBServer
	cd World/RoleDBServer/admin
	./reload.sh
	cd ../../../

	#重新加载NameDBServer
	cd World/NameDBServer/admin
	./reload.sh
	cd ../../../

	#重新加载ZoneGatewayServer
	cd Zone/ZoneGatewayServer/admin
	./reload.sh
	cd ../../..

	#重新加载ZoneServer
	cd Zone/ZoneServer/admin
	./reload.sh
	cd ../../..

	#去掉Cluster
	#重新加载ClusterServer
	#cd Cluster/ClusterServer/admin
	#./reload.sh
	#cd ../../..

	#重新加载RegAuthGatewayServer
	cd Global/RegAuthGatewayServer/admin
	./reload.sh
	cd ../../..
	
	#重新加载RegAuthServer
	cd Global/RegAuthServer/admin
	./reload.sh
	cd ../../..

	#重新加载AccountDBServer
	cd Global/AccountDBServer/admin
	./reload.sh
	cd ../../..

	echo "Success to reload all servers!!!"
}

#脚本主循环，根据输入参数来决定执行的脚本操作

while getopts ":I:D:KSR" options
do
	case $options in
		I)
			if [ $# -ne 9 ]
			then
				Usage

				exit 1
			fi

			Init_Server $2 $3 $4 $5 $6 $7 $8 $9

			exit 1
			;;

		D)
			if [ $# -ne 2 ]
			then
				Usage
				
				exit 1
			fi
			
			Deploy_Server $2
			
			exit 1
			;;
			
		K)
			Kill_Server
		
			exit 1
			;;

		S)
			Start_Server
		
			exit 1;;

		R)
			Reload_Server

			exit 1
			;;

		*)
			Usage

			exit 1
			;;

		esac

done

Usage
