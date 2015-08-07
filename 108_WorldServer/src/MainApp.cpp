
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/file.h>

#include "GameProtocol.hpp"
#include "SignalUtility.hpp"
#include "NowTime.hpp"
#include "AppDefW.hpp"
#include "AppLoopW.hpp"
#include "ShmObjectCreator.hpp"

#include "FileUtility.hpp"
#include "StringUtility.hpp"
#include "AppUtility.hpp"
#include "ConfigHelper.hpp"

using namespace ServerLib;

extern void AppDaemon_ShowUsage();

int main(int argc, char* *argv)
{
    bool bResume;
    int iWorldID;
    int iInstanceID;

    CAppUtility::AppLaunch(argc, argv, CAppLoopW::SetAppCmd, bResume, iWorldID, iInstanceID, NULL, NULL, NULL, true);

	CSharedMemory stShmMain;
	CAppLoopW* pAppLoop = CShmObjectCreator<CAppLoopW>::CreateObjectByKey(&stShmMain, GenerateServerShmKey(GAME_SERVER_WORLD,0));
	if(!pAppLoop)
	{
		printf("CreateObject CAppLoopW Failed!\n");
		exit(3);
	}

	int iRet = pAppLoop->Initialize(bResume, iWorldID);
	if(iRet)
	{
		printf("MainLoop Initialize Error:%d. So Quit!\n", iRet);
		exit(4);
	}

	pAppLoop->Run();

	return 0;
}

