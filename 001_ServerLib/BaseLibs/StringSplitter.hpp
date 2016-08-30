/**
*@file StringSplitter.hpp
*@author jasonxiong
*@date 2009-11-18
*@version 1.0
*@brief 字符串分离类
*
*	将一个字符串根据一个分隔符（separtor）分隔开来，该类提取分隔开来的串
*	如 "derek yu"这个字符串，可以设置分隔符" "分为derek和yu
*	再如 "hello&every one这个字符串，可以根据分隔符"&"分为hello和every one
*	使用方法：
*	（1）调用SetString()将需要分隔的字符串传入Splitter，注意这里传的指针，在Splitter内部使用时也是直接使用该指针
*	（2）调用GetNextToken()指定分隔符来获取下一个分隔串，并将当前寻找的偏移位置向后移，找不到时返回-1
*/

#ifndef __STRING_SPLITTER_HPP__
#define __STRING_SPLITTER_HPP__

#include "ErrorDef.hpp"

namespace ServerLib
{
class CStringSplitter
{
public:
    CStringSplitter();
    ~CStringSplitter();

public:
    //!指定要被分隔符解析的字符串，这里仅仅是指针拷贝
    int SetString(const char* pszString);

    //!重置Token指针开始位置
    void ResetTokenBegPos()
    {
        m_iCurTokenBeg = 0;
    }

    /**
    *获取被分隔符分隔的字符串，并将当前寻找的偏移位置向后移
    *@param[in] pszSepartor 分隔符
    *@param[out] szToken 用于存放找到的Token串
    *@param[in] iMaxTokenLength szToken串最大能存放的大小
    *@return 0 success, 非0表示找不到
    */
    int GetNextToken(const char* pszSepartor, char* szToken, int iMaxTokenLength);

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
    const char* m_pszString;
    int m_iCurTokenBeg; //当前Token指针开始位置
    int m_iErrorNO; //!错误码

};
}

#endif //__STRING_SPLITTER_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
