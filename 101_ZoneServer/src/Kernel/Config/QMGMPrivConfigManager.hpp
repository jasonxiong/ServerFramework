#ifndef __QM_GM_PRIV_CONFIG_MANAGER_HPP__
#define __QM_GM_PRIV_CONFIG_MANAGER_HPP__

//GM权限控制配置管理器

//拥有GM权限的IP号段
struct GMIPSection
{
    unsigned uIPBegin;
    unsigned uIPEnd;

    GMIPSection()
    {
        memset(this, 0, sizeof(*this));
    }
};

//最大支持的GM有效IP号段的数量
const int MAX_VALID_GM_IP_SECTION = 30;

//拥有GM权限的IP列表
struct GMValidIPList
{
    int iIPSectionNum;
    GMIPSection stIPSections[MAX_VALID_GM_IP_SECTION];

    GMValidIPList()
    {
        memset(this, 0, sizeof(*this));
    };
};

//最大支持的GM有效uin的数量
const int MAX_VALID_GM_USER_NUM = 1000;

//拥有GM权限的玩家列表
struct GMValidUserList
{
    int iValidUserNum;
    unsigned int uValidUin[MAX_VALID_GM_USER_NUM];

    GMValidUserList()
    {
        memset(this, 0, sizeof(*this));
    };
};

//GM玩家权限配置
struct QMGMUserPrivConfig
{
    GMValidIPList stValidIpSection;
    GMValidUserList stValidUsers;

    QMGMUserPrivConfig()
    {
        memset(this, 0, sizeof(*this));
    }
};

class CQMGMPrivConfigManager
{
public:
    CQMGMPrivConfigManager();
    ~CQMGMPrivConfigManager();

    //加载GM权限配置文件
    int LoadGMPrivConfig();

    //判断是否是有效的GM用户的IP
    bool CheckIsGMIP(unsigned int uClientIP);

    //判断是否是有效的GM用户uin
    bool CheckIsGMUin(unsigned int uin);
    
private:
    static QMGMUserPrivConfig m_stGMPrivConfig;
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
