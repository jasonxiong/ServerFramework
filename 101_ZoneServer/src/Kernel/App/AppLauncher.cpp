#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/file.h>

#include "AppLoop.hpp"
#include "AppDef.hpp"
#include "SignalUtility.hpp"
#include "NowTime.hpp"
#include "AppUtility.hpp"
#include "ShmObjectCreator.hpp"
#include "FileUtility.hpp"
#include "StringUtility.hpp"
#include "Random.hpp"
#include "ConfigHelper.hpp"

using namespace ServerLib;

// 测试启动模式
void RunTestMode(int argc, char** argv);

// 内存调试模式
void CheckShareMemoryMode(int argc, char** argv)
{
	//TODO -Werror=unused-but-set-variable
	//int iWorldID;
	int iZoneID;
	int iInstanceID;

	if (argc < 5)
	{
		printf("Usage: ./StarZoneServer -vv worldid zoneid insid\n");
		return;
	}

	//TODO -Werror=unused-but-set-variable
	//iWorldID = atoi(argv[2]);
	iZoneID = atoi(argv[3]);
	iInstanceID = atoi(argv[4]);

	key_t iShmKey = GenerateServerShmKey(GAME_SERVER_ZONE, iZoneID * 10 + iInstanceID);

	int iTotalMemSize = sizeof(CAppLoop) + sizeof(CSharedMemory);

	//获取共享内存
	int iMemID = shmget(iShmKey, iTotalMemSize, 0444);//只读方式
	if (iMemID < 0)
	{
		printf("Error: get shm:key %d size:%d fail. error:%s\n", iShmKey, iTotalMemSize, strerror(errno));
		return;
	}

	//链接共享内存
	char* pShm = (char*)shmat(iMemID, NULL, SHM_RDONLY);//只读方式链接
	if (!pShm)
	{
		printf("Error: attach shm fail. shm id:%d. error:%s\n", iMemID, strerror(errno));
		return;
	}

	CAppLoop* pAppLoop = (CAppLoop*)(pShm + sizeof(CSharedMemory));

	while (true)
	{
		//这里面是为了能编译，unused pAppLoop
		if (pAppLoop)
		{
			sleep(60);
		}
	}
}

// 正常服务器模式
void NormalServerMode(int argc, char** argv)
{
	bool bResume;
	int iWorldID;
	int iZoneID;
	int iInstanceID;

	CAppUtility::AppLaunch(argc, argv, CAppLoop::SetAppCmd, bResume, iWorldID, iInstanceID, &iZoneID, NULL, NULL, true);

	//todo jasonxiong 这边使用共享内存有点问题，所以直接new出来算了
	/*
	CSharedMemory stShmMain;
	CAppLoop* pAppLoop = CShmObjectCreator<CAppLoop>::CreateObjectByKey
		(&stShmMain, GenerateServerShmKey(GAME_SERVER_ZONE,iZoneID * 10 + iInstanceID));
	if(!pAppLoop)
	{
		TRACESVR("CreateObject CAppLoop Failed!\n");
		return;
	}
	*/
	
	CAppLoop* pAppLoop = new CAppLoop;
	int iRet = pAppLoop->Initialize(bResume, iWorldID, iZoneID, iInstanceID);
	if(iRet)
	{
		TRACESVR("MainLoop Initialize Error:%d. So Quit!\n", iRet);
		return;
	}

	pAppLoop->Run();
}

int main(int argc, char** argv)
{
	if (strcmp(argv[1], "-vv") == 0)
	{
		CheckShareMemoryMode(argc, argv);
	}
    else if (strcmp(argv[1], "-test") == 0)
    {
        RunTestMode(argc, argv);
    }
	else
	{
		NormalServerMode(argc, argv);
	}

	return 0;
}

// 测试模式
void RunTestMode(int argc, char** argv)
{
#ifdef _DEBUG_

    bool bResume = false;
    int iWorldID = 1;
    int iZoneID = 1;
    int iInstanceID = 1;

    // 时间优化
    NowTimeSingleton::Instance()->RefreshNowTime();
    NowTimeSingleton::Instance()->RefreshNowTimeVal();

    // 设置进程随机数种子
    CRandomCalculator::Initialize();

    CSharedMemory stShmMain;
    CAppLoop* pAppLoop = CShmObjectCreator<CAppLoop>::CreateObjectByKey
        (&stShmMain, GenerateServerShmKey(GAME_SERVER_ZONE, iZoneID * 10 + iInstanceID));
    if(!pAppLoop)
    {
        TRACESVR("CreateObject CAppLoop Failed!\n");
        return;
    }

    int iRet = pAppLoop->Initialize(bResume, iWorldID, iZoneID, iInstanceID);
    if(iRet)
    {
        TRACESVR("MainLoop Initialize Error:%d. So Quit!\n", iRet);
        return;
    }
#endif

}

