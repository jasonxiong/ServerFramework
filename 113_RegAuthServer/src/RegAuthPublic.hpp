#ifndef __REGAUTH_PUBLIC_HPP__
#define __REGAUTH_PUBLIC_HPP__

#include "Public.hpp"

#include "HashUtility.hpp"

const unsigned int MAX_REGAUTH_HANDLER_NUMBER = 16384;
const unsigned int MAX_UINPRIV_NUMBER = 10;

inline unsigned int GetAccountHash(const std::string& strAccount)
{
    long lHash = CHashUtility::BKDRHash(strAccount.c_str(), strAccount.size());

    return (unsigned)lHash;
}

#endif // __REGAUTH_PUBLIC_HPP__
