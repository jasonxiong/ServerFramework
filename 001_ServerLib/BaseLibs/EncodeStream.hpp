/**
*@file EncodeStream.hpp
*@author jasonxiong
*@date 2009-11-30
*@version 1.0
*@brief 编码类
*
*	将基本数据类型以网络字节序编码到一段Buffer中
*/

#ifndef __ENCODE_STREAM_HPP__
#define __ENCODE_STREAM_HPP__

#include <stdint.h>

namespace ServerLib
{

class CEncodeStream
{
public:
    CEncodeStream();
    ~CEncodeStream();

public:
    /**
    *初始化编码器
    *@param[in] pucCodeBuf 存放编码的码流缓冲区
    *@param[in] iMaxCodeLen 码流缓冲区最大长度
    *@return 0 success
    */
    int Initialize(unsigned char* pucCodeBuf, int iMaxCodeLen);

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

    //编码char
    CEncodeStream& operator <<(char cChar);
    CEncodeStream& operator <<(unsigned char ucChar);

    //编码short16
    CEncodeStream& operator <<(short shShort16);
    CEncodeStream& operator <<(unsigned short ushShort16);

    //编码Int32
    CEncodeStream& operator <<(int iInt32);
    CEncodeStream& operator <<(unsigned int uiInt32);

    //编码Int64
    CEncodeStream& operator <<(uint64_t uiInt64);

    ////编码long
    //CEncodeStream& operator <<(long lLong);
    //CEncodeStream& operator <<(unsigned long ulLong);

    //编码String
    int EncodeString(char *strSrc, short sMaxStrLength);

    //编码Mem
    int EncodeMem(char *pcSrc, int iMemorySize);

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
    int m_iMaxCodeLen; //!<码流缓冲区最大长度
    unsigned char* m_aucCodeBuf; //!<存放编码的码流缓冲区
    unsigned char* m_pCurCodePos; //!<当前码流位置
};

}

#endif //__ENCODE_STREAM_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
