#include <string.h>
#include "RoleNumberObj.hpp"
#include "SortArray.hpp"
#include "LogAdapter.hpp"

IMPLEMENT_DYN(CRoleNumberObj)

CRoleNumberObj::CRoleNumberObj()
{
    m_uiUin = 0;
    m_bNoneRole = false;
    m_iRoleNumber = 0;

    for(unsigned i=0; i<sizeof(m_astRoleID)/sizeof(RoleID); ++i)
    {
        m_astRoleID[i].Clear();
    }
}

CRoleNumberObj::~CRoleNumberObj()
{

}

int CRoleNumberObj::Initialize()
{
    m_bNoneRole = true;
    m_iRoleNumber = 0;

    for(unsigned i=0; i<sizeof(m_astRoleID)/sizeof(RoleID); ++i)
    {
        m_astRoleID[i].Clear();
    }

    return 0;
}

void CRoleNumberObj::SetUin(unsigned int uiUin)
{
    m_uiUin = uiUin;
}

unsigned int CRoleNumberObj::GetUin() const
{
    return m_uiUin;
}

void CRoleNumberObj::SetNoneRoleFlag()
{
    m_bNoneRole = true;
}

void CRoleNumberObj::ResetNoneRoleFlag()
{
    m_bNoneRole = false;
}

unsigned char CRoleNumberObj::GetRoleNumberOnWorld() const
{
    return m_iRoleNumber;
}

int CompareRoleID(const void* pArg1, const void* pArg2)
{
    ASSERT_AND_LOG_RTN_INT(pArg1);
    ASSERT_AND_LOG_RTN_INT(pArg2);

    RoleID* pRoleID1 = (RoleID*)pArg1;
    RoleID* pRoleID2 = (RoleID*)pArg2;

    if (pRoleID1->uiseq() > pRoleID2->uiseq())
    {
        return 1;
    }
    else if (pRoleID1->uiseq() < pRoleID2->uiseq())
    {
        return -1;
    }

    return 0;
}

void CRoleNumberObj::AddOneRoleToWorld(const RoleID& stRoleID)
{
    MyBInsert(&stRoleID, m_astRoleID, &m_iRoleNumber, sizeof(RoleID), 1, CompareRoleID);
}

void CRoleNumberObj::SubOneRoleFromWorld(const RoleID& stRoleID)
{
    MyBDelete(&stRoleID, m_astRoleID, &m_iRoleNumber, sizeof(RoleID), CompareRoleID);
}

