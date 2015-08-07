/**
*@file SymmetryEncryption.hpp
*@author jasonxiong
*@date 2009-12-04
*@version 1.0
*@brief 对称加密
*
*
*/

#ifndef __SYMMETRY_ENCRYPTION_HPP__
#define __SYMMETRY_ENCRYPTION_HPP__

namespace ServerLib
{

typedef enum enmSymmetryEncryptionAlgorithm
{
    ESEA_NO_ENCRYPT = 0, //!<直接拷贝，不加密
    ESEA_OI_SYMMETRY_ENCRYPT = 1, //!<使用OI库的对称加密
} ENMSYMMETRYENCRYPTIONALGORITHM;

class CSymmetryEncryption
{
public:
    CSymmetryEncryption() {}
    ~CSymmetryEncryption() {}

public:
    /**
    *对称加密的加密函数
    *@param[in] nAlgorithm 加密算法
    *@param[in] pbyKey 对称密钥
    *@param[in] pbyIn 明文
    *@param[in] nInLength 明文长度
    *@param[in] pnOutLength 密文最大长度
    *@param[out] pbyOut 密文
    *@param[out] pnOutLength 密文实际长度
    *@return 0 success
    */
    static int EncryptData(short nAlgorithm, const unsigned char *pbyKey,
                           const unsigned char *pbyIn, short nInLength,
                           unsigned char *pbyOut, short *pnOutLength);

    /**
    *对称加密的解密函数
    *@param[in] nAlgorithm 加密算法
    *@param[in] pbyKey 对称密钥
    *@param[in] pbyIn 密文
    *@param[in] nInLength 密文长度
    *@param[in] pnOutLength 明文最大长度
    *@param[out] pbyOut 明文
    *@param[out] pnOutLength 明文实际长度
    *@return 0 success
    */
    static int DecryptData(short nAlgorithm, const unsigned char *pbyKey,
                           const unsigned char *pbyIn, short nInLength,
                           unsigned char *pbyOut, short *pnOutLength);

};
}

#endif //__SYMMETRY_ENCRYPTION_HPP__
///:~
