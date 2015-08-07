
#ifndef __TEMPLATE_CONFIG_MANAGER_HPP__
#define __TEMPLATE_CONFIG_MANAGER_HPP__

#include "GameConfigDefine.hpp"
#include "ConfigHelper.hpp"

using namespace GameConfig;

//配置管理器模版
template <class TSERVERCONFIG, int MAX_TEMPLATE_CONFIG_NUM, const char CONFIG_FILE_NAME[]>
class CTemplateConfigManager
{
public:
    CTemplateConfigManager()
    {
        m_iConfigNum = 0;

        memset(m_astServerConfig, 0, sizeof(m_astServerConfig));
    };
    
    ~CTemplateConfigManager()
    {
        
    };

    int LoadConfig()
    {
        int iRet = LoadTemplateCfg<TSERVERCONFIG, MAX_TEMPLATE_CONFIG_NUM>(CONFIG_FILE_NAME, m_astServerConfig, &m_iConfigNum, NULL);
        if(iRet)
        {
            LOGERROR("Fail to load template config , ret %d\n", iRet);
            return iRet;
        }

        //排序
        qsort(m_astServerConfig, m_iConfigNum, sizeof(TSERVERCONFIG), cmp_one_key(TSERVERCONFIG, iConfigID));

        return 0;
    };

    const TSERVERCONFIG* GetConfig(int iConfigID)
    {
        if(iConfigID == 0)
        {
            return NULL;
        }

        TSERVERCONFIG stKey;
        stKey.iConfigID = iConfigID;

        return (TSERVERCONFIG*)bsearch(&stKey, m_astServerConfig, m_iConfigNum, sizeof(TSERVERCONFIG), cmp_one_key(TSERVERCONFIG, iConfigID));
    };

private:
   
    //模版管理器实际的配置数量
    int m_iConfigNum;

    //实际的内容配置
    TSERVERCONFIG m_astServerConfig[MAX_TEMPLATE_CONFIG_NUM];
};

#endif
