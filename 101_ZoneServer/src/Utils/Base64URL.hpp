#ifndef __BASE64_URL_HPP__
#define __BASE64_URL_HPP__

// Base64编码, 并对URL特殊字符进行了替换
// 参考: http://tools.ietf.org/html/rfc4648
//
// Author: Brianzhang
// Date: 2010-10-24
//
// Base64的基本原理是将6-bit的二进制数值替换为64个8-bitASCII可打印字符中的一个
// 因此, 原始数值中每3个8-bit字符(即4个6-bit数值)会被替换为4个8-bitASCII码, 编码后长度增加1/3
// 
// 为了和标准的Base64编码不同, 本算法打乱了ASCII码的次序, 以起到防破解的作用
// 具体替换关系见实现文件Base64URL.cpp
//

// Base64URL编码, 成功返回0
int Base64URLEncode(unsigned char* pszDstBuffer, int& iDstLen, const char* pszSrcBuffer, int iSrcLen);

// Base64URL解码, 成功返回0
int Base64URLDecode(char* pszDstBuffer, int& iDstLen, const unsigned char* pszSrcBuffer, int iSrcLen);


#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
