#ifndef __ACCOUNT_UIN_OBJ_HPP__
#define __ACCOUNT_UIN_OBJ_HPP__

#include "RegAuthPublic.hpp"
#include "ObjAllocator.hpp"
#include "HashMap_KString.hpp"

using namespace ServerLib;

//todo jasonxiong 这个里面的字符串存储需要根据后续实际内存使用的情况进行优化
//todo jasonxiong 后续根据接入不同的平台帐号，可能需要除密码之外的其他的认证方式

class CAccountUinObj : public CObj
{
private:
    //todo jasonxiong TDataString 里面缓存的字符串的长度可以根据后续的帐号平台的实际需要进行优化，避免浪费内存
    TDataString m_stAccountID;

    //玩家的uin
    unsigned int m_uiUin;

    //玩家的password的缓存，避免每次认证都需要查找数据库
    //todo jasonxiong PASSWORD的最大长度和帐号的最大长度可能不一样，可能也需要对实际的字符串进行优化
    TDataString m_stPassword;

public:
    CAccountUinObj();
    virtual ~CAccountUinObj();

public:
    virtual int Initialize();

public:
    const TDataString& GetKey() const;
    void SetKey(const TDataString& stKey);

    const TDataString& GetPassword() const;
    void SetPassword(const TDataString& stPassword);

    const unsigned int GetUin() const;
    void SetUin(unsigned int uiUin);

    DECLARE_DYN
};

#endif // __ROLE_NUMBER_OBJ_HPP__

