#include <string.h>

#include "AccountUinObj.hpp"
#include "LogAdapter.hpp"

IMPLEMENT_DYN(CAccountUinObj)

CAccountUinObj::CAccountUinObj()
{
    m_uiUin = 0;

    memset(&m_stAccountID, 0, sizeof(m_stAccountID));
    memset(&m_stPassword, 0, sizeof(m_stPassword));
}

CAccountUinObj::~CAccountUinObj()
{

}

int CAccountUinObj::Initialize()
{
    m_uiUin = 0;

    memset(&m_stAccountID, 0, sizeof(m_stAccountID));
    memset(&m_stPassword, 0, sizeof(m_stPassword));

    return 0;
}

const TDataString& CAccountUinObj::GetKey() const
{
    return m_stAccountID;
}

void CAccountUinObj::SetKey(const TDataString& stKey)
{
    m_stAccountID = stKey;
}

const TDataString& CAccountUinObj::GetPassword() const
{
    return m_stPassword;
}

void CAccountUinObj::SetPassword(const TDataString& stPassword)
{
    m_stPassword = stPassword;
}

const unsigned int CAccountUinObj::GetUin() const
{
    return m_uiUin;
}

void CAccountUinObj::SetUin(unsigned int uiUin)
{
    m_uiUin = uiUin;
}



----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
