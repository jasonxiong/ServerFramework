#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/file.h>
#include <stdlib.h>

#include "AppDef.hpp"
#include "NameDBApp.hpp"
#include "SignalUtility.hpp"
#include "NowTime.hpp"
#include "ShmObjectCreator.hpp"
#include "FileUtility.hpp"
#include "StringUtility.hpp"
#include "AppUtility.hpp"
#include "ConfigHelper.hpp"

using namespace ServerLib;

int main(int argc, char* *argv)
{
    bool bResume;
    int iWorldID;
    int iInstanceID;

    CAppUtility::AppLaunch(argc, argv, CNameDBApp::SetAppCmd, bResume, iWorldID, iInstanceID);

    CSharedMemory stShmMain;
    CNameDBApp* pApp = CShmObjectCreator<CNameDBApp>::CreateObjectByKey(&stShmMain, GenerateServerShmKey(GAME_SERVER_NAMEDB,0));
    if (!pApp)
    {
        exit(3);
    }

    CAppType<CNameDBApp>::RegApp(pApp);

    int iRet = pApp->Initialize(bResume, iWorldID);
    if (iRet != 0)
    {
        exit(4);
    }

    pApp->Run();

    return 0;
}

