#ifndef __CONFIG_MGR_H__
#define __CONFIG_MGR_H__

//#include "ConfigDef.hpp"
//#include "WhiteListConfig.hpp"
#include "UinPrivManager.hpp"

class CConfigMgr
{
private:
    //TZoneConfList m_stZoneConfList;
    CUinPrivManager m_stUinPrivManager;
    static int m_iWhiteListEnabled;
    static int m_iCreateRoleEnabled;

public:
    int LoadAllConf();

    TUinPrivInfo* GetUinPrivInfo(unsigned int uiUin);

public:

    //检查白名单
    static int IsWhiteListEnabled();

    //是否允许创建角色
    static bool IsCreateRoleEnabled();
};

#endif // __CONFIG_MGR_H__

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
