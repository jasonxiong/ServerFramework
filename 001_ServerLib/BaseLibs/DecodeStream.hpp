/**
*@file DecodeStream.hpp
*@author jasonxiong
*@date 2009-12-01
*@version 1.0
*@brief 解码流
*
*	从一段Buffer中以网络字节序解码出数据
*/

#ifndef __DECODE_STREAM_HPP__
#define __DECODE_STREAM_HPP__

#include <stdint.h>

namespace ServerLib
{

class CDecodeStream
{
public:
    CDecodeStream();
    ~CDecodeStream();

public:
public:
    /**
    *初始化解码流
    *@param[in] pucCodeBuf 存放编码的码流缓冲区
    *@param[in] iCodeLen 编码长度
    *@return 0 success
    */
    int Initialize(unsigned char* pucCodeBuf, int iCodeLen);

    //!获取码流缓冲区
    unsigned char* GetCodeBuf() const
    {
        return m_aucCodeBuf;
    }

    //!获取当前码流偏移
    int GetCurOffset() const
    {
        return m_pCurCodePos - m_aucCodeBuf;
    }

    //!设置当前码流偏移
    void SetCurOffset(int iOffset)
    {
        m_pCurCodePos = m_aucCodeBuf + iOffset;
    }

    //!返回编码剩余长度
    int GetLeftLength() const
    {
        return m_iCodeLen - (int)(m_pCurCodePos - m_aucCodeBuf);
    }

    //解码char
    CDecodeStream& operator >>(char& rcChar);
    CDecodeStream& operator >>(unsigned char& rucChar);

    //解码short16
    CDecodeStream& operator >>(short& rshShort16);
    CDecodeStream& operator >>(unsigned short& rushShort16);

    //解码Int32
    CDecodeStream& operator >>(int& riInt32);
    CDecodeStream& operator >>(unsigned int& ruiInt32);

    //解码Int64
    CDecodeStream& operator >>(uint64_t& ruiInt64);

    ////解码long
    //CDecodeStream& operator >>(long& rlLong);
    //CDecodeStream& operator >>(unsigned long& rulLong);

    //解码String
    int DecodeString(char *strDest, short shMaxStrLength);

    //解码Mem
    int DecodeMem(char *pcDest, int iMemorySize);

    //!在接口返回错误时，调用这个函数获取错误号
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    //!设置错误号
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    int m_iErrorNO; //!错误码
    int m_iCodeLen; //!<编码长度
    unsigned char* m_aucCodeBuf; //!<需要解码的码流缓冲区
    unsigned char* m_pCurCodePos; //!<当前码流位置
};


}

#endif //__DECODE_STREAM_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
