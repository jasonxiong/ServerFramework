/**
*@file LineConfig.hpp
*@author jasonxiong
*@date 2009-11-18
*@version 1.0
*@brief 按行配置文件类
*
*	按行读取该配置文件，每行是一个或若干个配置项，各个配置项之间用空格或TAB分离开
*/

#ifndef __LINE_CONFIG_HPP__
#define __LINE_CONFIG_HPP__

#include <stdio.h>

#include "ErrorDef.hpp"

namespace ServerLib
{

class CLineConfig
{
public:
    CLineConfig();
    ~CLineConfig();

public:
    //!加载配置文件到内存
    int  OpenFile(const char *pszFilename);

    //!释放配置文件加载到内存后占用的资源
    void CloseFile();

    //!读取一行配置，返回读取的配置项个数
    int GetItemValue(const char* pcContent, ...);

    //!判断是否已经读取结束
    bool IsReachEOF() const
    {
        return m_bReachEOF;
    }

    //!在接口返回错误时，调用这个函数获取错误号
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    //!设置错误号
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    int m_iErrorNO; //!错误码
    FILE* m_pfCfgFile; //!<打开的配置文件指针
    bool m_bReachEOF; //!<文件读取到末尾
};

}

#endif //__LINE_CONFIG_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
