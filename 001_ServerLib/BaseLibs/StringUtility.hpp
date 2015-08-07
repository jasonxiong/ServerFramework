/**
*@file StringUtility.hpp
*@author jasonxiong
*@date 2009-11-18
*@version 1.0
*@brief 字符串操作工具类
*
*
*/

#ifndef __STRING_UTIL_HPP__
#define __STRING_UTIL_HPP__

#define SAFE_STRCPY(pszDest, pszSrc, iDestBufLen) \
({ \
    char* pStr = strncpy(pszDest, pszSrc, (iDestBufLen)); \
    (pszDest)[(iDestBufLen) - 1] = '\0'; \
    pStr; \
})

#define SAFE_STRCAT(pszDest, pszSrc, iDestBufLen)               \
({                                                              \
    int iLeftLen = iDestBufLen - strlen(pszDest) - 1;           \
    char* pStr = strncat(pszDest, pszSrc, iLeftLen);        \
    (pszDest)[(iDestBufLen) - 1] = '\0';                        \
    pStr;                                                       \
})                                                          \
 
#define SAFE_STRLEN(pszStr, iStrBufLen) \
({ \
    strnlen(pszStr, (iStrBufLen)); \
})

#define SAFE_STRCMP(pszStr1, pszStr2, iMinStrBufLen) \
({ \
    strncmp(pszStr1, pszStr2, (iMinStrBufLen)); \
})

//NOTE: 这个地方必须传入实际剩余缓冲区的大小，否则有可能报错
#define SAFE_SPRINTF(pszBuf, iBufLen, pszFormat, ...) \
({ \
    int iSafeRet = snprintf(pszBuf, iBufLen, pszFormat, ##__VA_ARGS__); \
    (pszBuf)[iBufLen - 1] = '\0'; \
    (unsigned int)iSafeRet < (unsigned int)iBufLen? iSafeRet : iBufLen - 1; \
})

#define SAFE_VSPRINTF(pszBuf, iBufLen, pszFormat, valist) \
    ({ \
    int iSafeRet = vsnprintf(pszBuf, iBufLen, pszFormat, valist); \
    (pszBuf)[iBufLen - 1] = '\0'; \
    (unsigned int)iSafeRet < (unsigned int)iBufLen? iSafeRet : iBufLen - 1; \
})

namespace ServerLib
{

class CStringUtility
{
public:
    CStringUtility() {}
    ~CStringUtility() {}

public:
    /**
    *删除字符串首部和尾部的空白字符
    *@param[in] szTrimString 需要删除空白的字符串
    *@return 0 success
    */
    static int TrimString(char* szTrimString);

    /**
    *判断传入的字符是否是空白字符
    *@param[in] c 检查的字符
    *@return true or false
    */
    static bool IsBlankChar(char c);

    /**
    *转换字符串编码
    *@param[in] szInBuffer 需要转换的字符串
    *@param[in] iInLength 需要转换的字符串的实际长度
    *@param[out] szOutBuffer 转换后的字符串长度
    *@param[in/out] iOutLength szOutBuffer的最大长度，out为转换后的长度
    *@param[in] szToCode 转换后的格式
    *@param[in] szFromCode 转换前的格式
    */
    static int ConvertCode(char* szInBuffer, size_t uiInLength, char* szOutBuffer, size_t  uiOutLength, const char* szToCode, const char* szFromCode);

};

}


#endif //__STRING_UTIL_HPP__
///:~
