#ifndef __ROLE_SEQ_RECORD_OBJ_HPP__
#define __ROLE_SEQ_RECORD_OBJ_HPP__

#include "ObjAllocator.hpp"
#include "RoleDBPublic.hpp"

using namespace ServerLib;

class CRoleSeqRecordObj : public CObj
{
private:
    //static const int MAX_ROLE_NUMBER_PER_USER = 100; // 一个用户可以创建的角色的序号范围，为[0,99]
    static const int BITS_PER_BYTE = 8;
    static const unsigned char OR_MASK[BITS_PER_BYTE];
    static const unsigned char AND_MASK[BITS_PER_BYTE];

    typedef enum
    {
        BS_UNUSED = 0,
        BS_USED = 1
    } EBitState;

private:
    unsigned int m_uiUin;
    // 缓存uin对应的seq的使用情况，第i个bit对应seq i，为1表示该seq已经使用，为0表示未使用
    unsigned char m_ucaRoleSeqBits[1 / BITS_PER_BYTE + 1];

public:
    CRoleSeqRecordObj();
    virtual ~CRoleSeqRecordObj();

public:
    virtual int Initialize();
    DECLARE_DYN

private:
    // 返回数组ucaRoleSeqBits中index为nSeq的bit的值，有效值为0或1,
    // 为0表示这个seq没有被使用，为1表示这个seq已经被使用，参数范围为[0,99]
    unsigned char GetBitValue(short nSeq);

public:
    void SetUin(unsigned int uiUin);
    unsigned int GetUin();

    void SetBitValue(short nSeq);   // 设置数组ucaRoleSeqBits中index为nSeq的bit的值为1
    void ResetBitValue(short nSeq); // 设置数组ucaRoleSeqBits中index为nSeq的bit的值为0
    short GenerateSeq(); // 根据某种算法生成一个在[0,99]范围内的未使用的seq，返回-1表示失败
};

#endif // __ROLE_SEQ_RECORD_OBJ_HPP__


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
