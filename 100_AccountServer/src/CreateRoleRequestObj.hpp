#ifndef __CREATE_ROLE_REQUEST_OBJ_HPP__
#define __CREATE_ROLE_REQUEST_OBJ_HPP__

#include "ObjAllocator.hpp"
#include "AccountPublic.hpp"
#include "CacheTime.hpp"

using namespace ServerLib;

class CCreateRoleRequestObj : public CObj, public CCacheTime
{
private:
    int m_iRequestWorldID;
    char m_szRequestNickName[64];
    unsigned int m_uRequestUin;

public:
    CCreateRoleRequestObj();
    virtual ~CCreateRoleRequestObj();

public:
    virtual int Initialize();
    DECLARE_DYN

public:
    void SetRequestInfo(const CreateRole_Account_Request& stRequest);

    unsigned int GetUin() const;
    int GetWorldID() const;
    const char* GetNickName() const;
};

#endif // __CREATE_ROLE_REQUEST_OBJ_HPP__


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
