#include <string.h>
#include <time.h>

#include "Base64URL.hpp"
#include "LogAdapter.hpp"

using namespace ServerLib;

#define     BASE64_BITS             6
#define     BASE64_CODE_NUMBER      (1 << BASE64_BITS)
#define     BASE64_DECODE_NUMBER    256

// 编码表
const unsigned char Base64Encodes[BASE64_CODE_NUMBER] = 
{
    'z', 'A', 'R', 'i',
    'j', '0', 'B', 'S',
    '1', 'C', 'k', 'T',
    'U', 'D', '2', 'l',
    '3', 'm', 'E', 'V',
    'n', 'F', '4', 'W',
    'X', '5', 'G', 'o',
    'p', 'H', '6', 'Y',
    '7', 'q', 'Z', 'I',
    'r', 'J', '8', 'a',
    's', 'K', '9', 'b',
    'c', 't', '-', 'L',
    '_', 'M', 'd', 'u',
    'e', 'v', 'N', 'w', 
    'O', 'f', 'g', 'x',
    'P', '=', 'y', 'h',
};

// 尾部填充码
#define     PAD_CODE    'Q'

// 解码表
unsigned char Base64Decodes[BASE64_DECODE_NUMBER];

// 初始化解码表
void InitBase64Decoder()
{
    static bool bInited = false;

    if (bInited)
    {
        return;
    }
    
    memset(Base64Decodes, BASE64_CODE_NUMBER, BASE64_DECODE_NUMBER);

    for (int i = 0; i < BASE64_CODE_NUMBER; i++)
    {
        Base64Decodes[Base64Encodes[i]] = i;
    }

    bInited = true;
}

// Base64URL编码, 成功返回0
int Base64URLEncode(unsigned char* pszDstBuffer, int& iDstLen, const char* pszSrcBuffer, int iSrcLen)
{
    ASSERT_AND_LOG_RTN_INT(pszSrcBuffer);
    ASSERT_AND_LOG_RTN_INT(pszSrcBuffer);
    
    if (iSrcLen < 0 || iDstLen < 4)
    {
		//nolog
        return -1;
    }

    int iSrcLeftLen = iSrcLen;
    int iDstLeftLen = iDstLen;

#define SELECTOR_1(x)   ((x) & 0x3F)
#define SELECTOR_2(x)   (((unsigned int)((x) & 0x0FC0)) >> 6)
#define SELECTOR_3(x)   (((unsigned int)((x) & 0x3F000)) >> 12)
#define SELECTOR_4(x)   (((unsigned int)((x) & 0xFC0000)) >> 18)


    // 每次对3个字符进行编码, 不足3个字符的用PAD_CODE补齐
    while (iSrcLeftLen > 0)
    {
        // 3个字符产生4个ASCII码
        if (iDstLeftLen < 4)
        {
			//nolog
            return -2;
        }
    
        if (iSrcLeftLen >= 3)
        {     
            int iSrcCode = *(int*)pszSrcBuffer;

            pszDstBuffer[0] = Base64Encodes[SELECTOR_1(iSrcCode)];
            pszDstBuffer[1] = Base64Encodes[SELECTOR_2(iSrcCode)];
            pszDstBuffer[2] = Base64Encodes[SELECTOR_3(iSrcCode)];
            pszDstBuffer[3] = Base64Encodes[SELECTOR_4(iSrcCode)];
        }
        else if (iSrcLeftLen == 2)
        {
            unsigned int uiSrcCode = *(unsigned short*)pszSrcBuffer;

            pszDstBuffer[0] = Base64Encodes[SELECTOR_1(uiSrcCode)];
            pszDstBuffer[1] = Base64Encodes[SELECTOR_2(uiSrcCode)];
            pszDstBuffer[2] = Base64Encodes[SELECTOR_3(uiSrcCode)];
            pszDstBuffer[3] = PAD_CODE;
        }
        else if (iSrcLeftLen == 1)
        {
            unsigned short usSrcCode = *(unsigned char*)pszSrcBuffer;

            pszDstBuffer[0] = Base64Encodes[SELECTOR_1(usSrcCode)];
            pszDstBuffer[1] = Base64Encodes[SELECTOR_2(usSrcCode)];
            pszDstBuffer[2] = PAD_CODE;
            pszDstBuffer[3] = PAD_CODE;
        }

        pszSrcBuffer += 3;
        iSrcLeftLen -= 3;

        pszDstBuffer += 4;
        iDstLeftLen -= 4;
    }

    ASSERT_AND_LOG_RTN_INT(iDstLeftLen >= 0 && iDstLeftLen <= iDstLen);
    iDstLen = iDstLen - iDstLeftLen;

    return 0;
}

// Base64URL解码, 成功返回0
int Base64URLDecode(char* pszDstBuffer, int& iDstLen, const unsigned char* pszSrcBuffer, int iSrcLen)
{
    InitBase64Decoder();

    ASSERT_AND_LOG_RTN_INT(pszSrcBuffer);
    ASSERT_AND_LOG_RTN_INT(pszDstBuffer);
    
    if (iSrcLen < 0 || iDstLen < 0)
    {
		//nolog
        return -1;
    }

    // 原码长度必须是4的倍数
    if ((iSrcLen % 4) != 0)
    {
		//nolog
        return -2;
    }

    int iSrcLeftLen = iSrcLen;
    int iDstLeftLen = iDstLen;

    // 每次将4个字符解码为4个6bit数值, 然后拼接为3个8bit字符
    while (iSrcLeftLen > 0)
    {
        unsigned char ucDecode1 = Base64Decodes[pszSrcBuffer[0]];
        unsigned char ucDecode2 = Base64Decodes[pszSrcBuffer[1]];
        unsigned char ucDecode3 = Base64Decodes[pszSrcBuffer[2]];
        unsigned char ucDecode4 = Base64Decodes[pszSrcBuffer[3]];

        if (ucDecode1 >= BASE64_CODE_NUMBER || ucDecode2 >= BASE64_CODE_NUMBER)
        {
			//nolog
            return -3;
        }

        if (ucDecode4 < BASE64_CODE_NUMBER)
        {
            if (ucDecode3 >= BASE64_CODE_NUMBER)
            {
				//nolog
                return -4;
            }

            if (iDstLeftLen < 3)
            {
				//nolog
                return -5;
            }

            *(int*)pszDstBuffer = ucDecode4 << 18 | ucDecode3 << 12 | ucDecode2 << 6 | ucDecode1;
   
            pszDstBuffer += 3;
            iDstLeftLen -= 3;
        }
        else if (ucDecode3 < BASE64_CODE_NUMBER)
        {
            if (iDstLeftLen < 2)
            {
				//nolog
                return -6;
            }

            *(short*)pszDstBuffer = ucDecode3 << 12 | ucDecode2 << 6 | ucDecode1;

            pszDstBuffer += 2;
            iDstLeftLen -= 2;
        }
        else
        {
            if (iDstLeftLen < 1)
            {
				//nolog
                return -7;
            }

            *pszDstBuffer = ucDecode2 << 6 | ucDecode1;

            pszDstBuffer += 1;
            iDstLeftLen -= 1;
        }

        pszSrcBuffer += 4;
        iSrcLeftLen -= 4;
    }

    ASSERT_AND_LOG_RTN_INT(iDstLeftLen >= 0 && iDstLeftLen <= iDstLen);
    iDstLen = iDstLen - iDstLeftLen;

    return 0;
}

//
//int main()
//{
//    char szSrc[20] = {'a', '1', 'b', '0'};
//    char szDst[1024];
//
//    int iLen = 1023;
//    Base64URLEncode(szDst, iLen, szSrc, 4);
//    szDst[iLen] = '\0';
//    printf("%s\n", szDst);
//
//    int iLen2 = sizeof(szSrc);
//    memset(szSrc, 0, iLen2);
//    Base64URLDecode(szSrc, iLen2, szDst, iLen);
//
//    for (int i = 0; i < iLen2; i++)
//    {
//        printf("%c\n", szSrc[i]);
//    }
//
//    ////邀请码结构
//    typedef struct tagInviteKey
//    {
//        int m_iWorldID;     // 世界服务器ID
//        int m_iZoneID;      // 分线ID
//        int m_iCreateTime;  // 创建时间(秒): 每个玩家每秒只能创建一个邀请码. -1 表示此邀请码失效
//        int m_uiUin;        // 角色Uin
//        int m_uiSeq;        // 角色Seq
//    }TInviteKey;
//    
//    TInviteKey stKey;
//    stKey.m_iCreateTime = time(NULL);
//    stKey.m_iWorldID = 1;
//    stKey.m_iZoneID = 1;
//    stKey.m_uiUin = 51021141;
//    stKey.m_uiSeq = 1;
//
//    char szBuffer[1024];
//    int iKeyLen = 1023;
//    Base64URLEncode(szBuffer, iKeyLen, (char*)&stKey, sizeof(stKey));
//    szBuffer[iKeyLen] = '\0';
//    printf("%d: %s\n", iKeyLen, szBuffer);
//
//    memset((char*)&stKey, 0, sizeof(stKey));
//    int iDstLen = sizeof(stKey);
//    Base64URLDecode((char*)&stKey, iDstLen, szBuffer, iKeyLen);
//
//    printf("%d: %d, %d, %d, %d, %d\n", iDstLen, 
//        stKey.m_iWorldID, stKey.m_iZoneID, stKey.m_iCreateTime, stKey.m_uiUin, stKey.m_uiSeq);
//
//}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
