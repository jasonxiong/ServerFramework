// ********************************************************************
// 作    者： amacroli(idehong@gmail.com)
// 创建日期： 2011年5月1日
// 描    述： 打包工具类
// 应    用：
// ********************************************************************

class CPacker
{
public:
    CPacker(char* pBuffer, unsigned int uiLen)
        :m_pSrcBuffer(pBuffer), m_uiBufferLen(uiLen),m_pCurrBuffer(pBuffer), m_uiCurrLen(0)
    {
    }

    ~CPacker(void) {}

private:
    char* m_pSrcBuffer;
    unsigned m_uiBufferLen;

    char* m_pCurrBuffer;
    unsigned m_uiCurrLen;

public:
    template<typename WT>
    int Write(WT* pData, unsigned int uiLen = 0)
    {
        unsigned int uiSize = uiLen;
        if (0 == uiLen)
        {
            uiSize = sizeof(WT);
        }

        if (m_pCurrBuffer - m_pSrcBuffer + uiSize > m_uiBufferLen)
        {
            return -1;
        }

        memcpy(m_pCurrBuffer, pData, uiSize);
        m_pCurrBuffer += uiSize;
        m_uiCurrLen += uiSize;
        return 0;
    }

    template<typename RT>
    int Read(RT** ppData, unsigned int& uiLen = 0)
    {
        unsigned int uiSize = uiLen;
        if (0 == uiLen)
        {
            uiSize = sizeof(RT);
        }

        if (m_pCurrBuffer - m_pSrcBuffer + uiSize > m_uiBufferLen)
        {
            return -1;
        }

        *ppData = (RT*)m_pCurrBuffer;
        m_pCurrBuffer += uiSize;
        m_uiCurrLen += uiSize;
        return 0;
    }

    char* GetPackSize(unsigned int& uiLen)
    {
        uiLen = m_uiCurrLen;
        return m_pSrcBuffer;
    }

    int Seek(int iOffset)
    {
        if (m_pCurrBuffer - m_pSrcBuffer + iOffset > m_uiBufferLen)
        {
            return -1;
        }

        if (m_pCurrBuffer - m_pSrcBuffer + iOffset < 0)
        {
            return -2;
        }

        m_pCurrBuffer += iOffset;
        m_uiCurrLen += iOffset;
        return 0;
    }
};
