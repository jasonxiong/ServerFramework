#!/bin/sh

## server admin script
ProcessName="NameDBServer.dbg"
BIN_DIR="../bin"
SleepTime=5
SIG_STOP=5
SIG_DUMP=6
USER=`whoami`
PROCESSID=`pgrep -u $USER -f ${ProcessName}`

if [ $# -lt 1 ]
then
        echo "$0 start|stop|restart|check|init|dump"
        exit
fi

Kill_Server()
{
	if [ "$PROCESSID" ]
	then
		echo $PROCESSID | xargs kill -9
	fi
}

#先切换工作目录到bin
cd ${BIN_DIR}

ProcessInfo=$(ps afx| grep $ProcessName| grep -v grep | grep -v Script | grep -v log)

if [ $1 == "start" ]
then
        echo "start $ProcessName ..."
        if [ -n "$ProcessInfo" ]
        then
                echo "$ProcessName is up now ,please stop first."
                exit
        fi

        ulimit -c unlimited

        ./$ProcessName -w #WORLDID# &

        sleep $SleepTime

        ProcessInfo=$(ps afx| grep $ProcessName| grep -v grep | grep -v Script | grep -v log)
        if [ -z "$ProcessInfo" ]
        then
                echo "start $ProcessName failed."
                exit
        fi
        echo "done."
elif [ $1 == "stop" ]
then
        echo "stop $ProcessName ..."
        if [ -z "$ProcessInfo" ]
        then
                echo "$ProcessName is already down now."
                exit
        fi

        #killall -s $SIG_STOP $ProcessName
        #sleep $SleepTime
        #killall $ProcessName

		Kill_Server

        sleep $SleepTime


        ProcessInfo=$(ps afx| grep $ProcessName| grep -v grep | grep -v Script | grep -v log)
        if [ -n "$ProcessInfo" ]
        then
                echo "stop $ProcessName failed."
                exit
        fi
        echo "done"
elif [ $1 == "restart" ]
then
    if [ -n "$ProcessInfo" ]
    then
        echo "$ProcessName is up now, stop it now."
		
		Kill_Server

		sleep $SleepTime
    fi

        ProcessInfo=$(ps afx| grep $ProcessName| grep -v grep | grep -v Script | grep -v log)
        if [ -n "$ProcessInfo" ]
        then
                echo "stop $ProcessName failed."
                exit
        fi

    echo "restart $ProcessName..."

    ./$ProcessName -w #WORLDID# &

    sleep $SleepTime
    echo "done"
elif [ $1 == "reload" ]
then
	if [ ! -n "$ProcessInfo" ]
	then
		echo "$ProcessName is not up now, start it first!"
		exit
	fi

	echo "reload $ProcessName..."

	./$ProcessName reload &

	echo "done"

elif [ $1 == "check" ]
then
        echo "check $ProcessName ..."
        if [ -z "$ProcessInfo" ]
        then
                echo "++++++++++++Warning++++++++++++"
                echo "$ProcessName is down"
                echo "++++++++++++Warning++++++++++++"
        else
                echo "++++++++++++Info++++++++++++"
                echo "$ProcessName Info $ProcessInfo"
                echo "++++++++++++Info++++++++++++"
        fi
        echo "done"
elif [ $1 == "dump" ]
then
    echo "dump file..."
    if [ -z "$ProcessInfo" ]
    then
        echo "$ProcessName is out of service, please start it first."
        exit
    fi

    #killall -s $SIG_DUMP $ProcessName
    Kill_Server
	sleep $SleepTime
    echo "dumping finished..."

elif [ $1 == "init" ]
then
        echo "initializing... "
        echo 1805306368 > /proc/sys/kernel/shmmax
        echo "done"
else
        echo "$0 start|stop|check|init"
fi
