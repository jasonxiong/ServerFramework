#ifndef __PASSWORD_ENCRYPTION_UTILITY_HPP__
#define __PASSWORD_ENCRYPTION_UTILITY_HPP__

#include "AESCbcEncryptor.hpp"

//用户 Password 加密工具类

//直接使用MD5算法加密，不可逆

//加密用的Key
#define PASSWORD_ENCRYPT_KEY "FkuL+QD1IjWp-Oh"

class CPasswordEncryptionUtility
{
public:
    //对密码进行加密，生成密码的密文
    static int DoPasswordEncryption(const char* pszPasswd, int iPasswdLen, char* pszOutBuff, int& iOutBuffLen);
};

#endif
