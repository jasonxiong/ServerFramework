#ifndef __SESSION_KEY_UTILITY_HPP__
#define __SESSION_KEY_UTILITY_HPP__

#include "AESCbcEncryptor.hpp"
#include "Base64.hpp"

//Session Key 生成和校验工具类，用于保证登录玩家的 session 安全

//说明： Session Key 的生成算法为：
//          1.根据逻辑要求生成字符串传入;
//          2.使用 AES 算法 ECB模式 对生成的字符串使用 Key 进行加密;
//          3.使用 Base64 对二进制串进行 Base64 编码，方便传输;
//          4.SessionKey的解密算法为上述算法的逆过程; 

#define SESSION_ENCRYPT_KEY "FkuL9QD1IjWpbOh"

class CSessionKeyUtility
{
public:
    //根据传入字符串生成登录玩家的Session Key
    static int GenerateSessionKey(const char* pszInBuff, int iInBuffLen, char* pszOutBuff, int& iOutBuffLen);

    //根据Session Key解码传入的字符串
    static int DecryptSessionKey(const char* pszInBuff, int iInBuffLen, char* pszOutBuff, int& iOutBuffLen);
};

#endif
