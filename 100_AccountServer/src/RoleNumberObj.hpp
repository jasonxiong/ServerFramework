#ifndef __ROLE_NUMBER_OBJ_HPP__
#define __ROLE_NUMBER_OBJ_HPP__

#include "AccountPublic.hpp"
#include "ObjAllocator.hpp"
#include "GameProtocol.hpp"

using namespace ServerLib;

class CRoleNumberObj : public CObj
{
private:
    unsigned int m_uiUin;
    bool m_bNoneRole; // 如果该用户在所有的world上都没有角色，则该标志为true
    int m_iRoleNumber; // 该用户在world上角色个数
    RoleID m_astRoleID[10];   //支持创建多个角色

public:
    CRoleNumberObj();
    virtual ~CRoleNumberObj();

public:
    virtual int Initialize();

public:
    unsigned int GetUin() const;
    void SetUin(unsigned int uiUin);
    void SetNoneRoleFlag();
    void ResetNoneRoleFlag();
    unsigned char GetRoleNumberOnWorld() const;
    void AddOneRoleToWorld(const RoleID& stRoleID);
    void SubOneRoleFromWorld(const RoleID& stRoleID);

    DECLARE_DYN
};

#endif // __ROLE_NUMBER_OBJ_HPP__

