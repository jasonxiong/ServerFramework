#ifndef __HASH_UTILITY_HPP__
#define __HASH_UTILITY_HPP__

//对各种常用的Hash算法的封装

class CHashUtility
{
public:

    //Robert Sedgwicks hash
    static long RsHash(const char* pszStr, int iLen);

    //Justin Sobel hash
    static long JSHash(const char* pszStr, int iLen);

    //PJW hash
    static long PJWHash(const char* pszStr, int iLen);

    //ELF hash, just like PJW
    static long ELFHash(const char* pszStr, int iLen);

    //BKDR hash
    static long BKDRHash(const char* pszStr, int iLen);

    //SDBM hash
    static long SDBMHash(const char* pszStr, int iLen);

    //DJB hash, 目前公认的最有效的hash算法
    static long DJBHash(const char* pszStr, int iLen);

    //DEK hash
    static long DEKHash(const char* pszStr, int iLen);
};

#endif
