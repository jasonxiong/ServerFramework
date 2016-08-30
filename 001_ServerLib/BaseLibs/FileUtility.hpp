/**
*@file FileUtility.hpp
*@author jasonxiong
*@date 2009-10-29
*@version 1.0
*@brief 文件相关操作函数集合类
*
*
*/

#ifndef __FILE_UTIL_HPP__
#define __FILE_UTIL_HPP__

namespace ServerLib
{

class CFileUtility
{
private:
    CFileUtility() {}
    ~CFileUtility() {}

public:
    /**
    *创建一个文件夹
    *@param[in] pszDirName 需要创建的文件夹名
    *@return 0 success
    */
    static int MakeDir(const char* pszDirName);

    /**
    *更换所在目录
    *@param[in] pszDirName 需要改变到的工作目录
    *@return 0 success
    */
    static int ChangeDir(const char* pszDirName);
};

}


#endif //__FILE_UTIL_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
