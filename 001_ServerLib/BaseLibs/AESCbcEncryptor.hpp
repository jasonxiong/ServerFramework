#ifndef __AES_CBC_ENCRYPTOR_HPP__
#define __AES_CBC_ENCRYPTOR_HPP__

#include <string>

#include "botan/botan.h"

//这个文件是对AES算法的封装，使用的是botan中的实现
//使用的是ECB加密，只使用加密Key

using namespace Botan;

class CAESCbcEncryptor
{
public:
    CAESCbcEncryptor();
    ~CAESCbcEncryptor();

    //设置AES加密使用的 Key
    int SetAESKey(const char* pszAESKey, int iAESKeyLen);

    //使用AES CBC模式对明文串进行加密
    std::string DoAESCbcEncryption(const std::string& strInput);

    //使用AES CBC模式对加密串进行解密
    std::string DoAESCbcDecryption(const std::string& strInput);

private:

    //加密器
    Pipe* m_pstEncryptor;

    //解密器
    Pipe* m_pstDecryptor;
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
