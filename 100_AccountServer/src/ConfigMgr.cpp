#include <fstream>
#include <sstream>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ConfigMgr.hpp"
#include "SectionConfig.hpp"
#include "AccountPublic.hpp"

using namespace std;
using namespace ServerLib;

int CConfigMgr::m_iWhiteListEnabled = 0;
int CConfigMgr::m_iCreateRoleEnabled = 1;

int CConfigMgr::LoadAllConf()
{
    int iRet = -1;
    CSectionConfig stConfigFile;
    iRet = stConfigFile.OpenFile("../conf/AccountServer.tcm");
    if (iRet == 0)
    {
        stConfigFile.GetItemValue("module", "CreateRoleEnabled", m_iCreateRoleEnabled, 1);
        stConfigFile.GetItemValue("WhiteList", "IsWhiteListEnabled", m_iWhiteListEnabled, 0);
    }

    stConfigFile.CloseFile();

    iRet = m_stUinPrivManager.Initialize(MAX_UINPRIV_NUMBER, "../../../Common/UinPriv.shm");
    if (iRet != 0)
    {
        TRACESVR("failed to init UinPriv config\n");
        return -1;
    }

    return 0;
}

int CConfigMgr::IsWhiteListEnabled()
{
    return m_iWhiteListEnabled;
}

TUinPrivInfo* CConfigMgr::GetUinPrivInfo(unsigned int uiUin)
{
    return m_stUinPrivManager.GetUinPrivInfo(uiUin);
}

//是否允许创建角色
bool CConfigMgr::IsCreateRoleEnabled()
{
    return m_iCreateRoleEnabled != 0;
}

