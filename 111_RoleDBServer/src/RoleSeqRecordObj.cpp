#include <string.h>
#include "RoleSeqRecordObj.hpp"

IMPLEMENT_DYN(CRoleSeqRecordObj)

const unsigned char CRoleSeqRecordObj::OR_MASK[BITS_PER_BYTE] =
{
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

const unsigned char CRoleSeqRecordObj::AND_MASK[BITS_PER_BYTE] =
{
    0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0xFE
};

CRoleSeqRecordObj::CRoleSeqRecordObj()
{
    m_uiUin = 0;
    memset(m_ucaRoleSeqBits, 0, sizeof(m_ucaRoleSeqBits));
}

CRoleSeqRecordObj::~CRoleSeqRecordObj()
{

}

void CRoleSeqRecordObj::SetUin(unsigned int uiUin)
{
    m_uiUin = uiUin;
}

unsigned int CRoleSeqRecordObj::GetUin()
{
    return m_uiUin;
}

int CRoleSeqRecordObj::Initialize()
{
    return 0;
}

// 内部函数
unsigned char CRoleSeqRecordObj::GetBitValue(short nSeq)
{
    int iByteIndex = nSeq / BITS_PER_BYTE; // 找到数组byte的下标
    int iBitIndex = nSeq % BITS_PER_BYTE;  // 找到byte中的bit下标

    unsigned char ucTmp = m_ucaRoleSeqBits[iByteIndex] >> (BITS_PER_BYTE - 1 - iBitIndex);
    ucTmp &= 0x1;

    return ucTmp;
}

void CRoleSeqRecordObj::SetBitValue(short nSeq)
{
    int iByteIndex = nSeq / BITS_PER_BYTE; // 找到数组byte的下标
    int iBitIndex = nSeq % BITS_PER_BYTE;  // 找到byte中的bit下标

    m_ucaRoleSeqBits[iByteIndex] |= OR_MASK[iBitIndex];
}

void CRoleSeqRecordObj::ResetBitValue(short nSeq)
{
    int iByteIndex = nSeq / BITS_PER_BYTE; // 找到数组byte的下标
    int iBitIndex = nSeq % BITS_PER_BYTE;  // 找到byte中的bit下标

    m_ucaRoleSeqBits[iByteIndex] &= AND_MASK[iBitIndex];
}

// 生成算法：假设seq的范围是[0,99]共100个，并且记已经存在的最大的seq为A，记本次生成的seq为B。
// 如果A小于99，则B等于A+1；如果A等于99，则B等于[0,98]范围内的最小的未使用的那个值。
short CRoleSeqRecordObj::GenerateSeq()
{
    short i = 0;
    short nSeq = -1;

    // 先判断max seq是否已经使用
    short nMaxSeqIndex = 0;
    unsigned char ucMaxSeqValue = GetBitValue(nMaxSeqIndex);

    if (BS_UNUSED == ucMaxSeqValue) // max seq未使用
    {
        // 找到已使用的最大的seq
        for (i = nMaxSeqIndex - 1; i >= 0; i--)
        {
            if (BS_USED == GetBitValue(i))
            {
                nSeq = i + 1;
                break;
            }
        }

        // 如果[0,99]都未使用，则i等于-1
    }
    else // max seq已使用
    {
        // 找到未使用的最小的seq
        for (i = 0; i < nMaxSeqIndex; i++)
        {
            if (BS_UNUSED == GetBitValue(i))
            {
                nSeq = i;
                break;
            }
        }

        // 如果未找到可用的seq，则i等于nMaxSeqIndex，nSeq仍然等于-1
    }

    // [0,99]都未使用
    if (-1 == i)
    {
        nSeq = 0;
    }

    return nSeq;
}
