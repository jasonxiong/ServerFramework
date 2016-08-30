#include <string.h>

#include "StringUtility.hpp"

#include "CreateRoleRequestObj.hpp"

IMPLEMENT_DYN(CCreateRoleRequestObj)

using namespace ServerLib;

CCreateRoleRequestObj::CCreateRoleRequestObj()
{
    m_iRequestWorldID = 0;

    m_uRequestUin = 0;

    memset(m_szRequestNickName, 0, sizeof(m_szRequestNickName));
}

CCreateRoleRequestObj::~CCreateRoleRequestObj()
{

}

int CCreateRoleRequestObj::Initialize()
{
    m_iRequestWorldID = 0;
    m_uRequestUin = 0;

    memset(m_szRequestNickName, 0, sizeof(m_szRequestNickName));

    return 0;
}

void CCreateRoleRequestObj::SetRequestInfo(const CreateRole_Account_Request& stRequest)
{
    m_iRequestWorldID = stRequest.worldid();
    m_uRequestUin = stRequest.uin();

    //todo jasonxiong2 单机版不需要名字
    //SAFE_STRCPY(m_szRequestNickName, stRequest.sznickname().c_str(), sizeof(m_szRequestNickName)-1);

    return;
}

unsigned int CCreateRoleRequestObj::GetUin() const
{
    return m_uRequestUin;
}

int CCreateRoleRequestObj::GetWorldID() const
{
    return m_iRequestWorldID;
}

const char* CCreateRoleRequestObj::GetNickName() const
{
    return m_szRequestNickName;
}


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
