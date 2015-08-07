#ifndef __HASH_MAP_KSTR_HPP__
#define __HASH_MAP_KSTR_HPP__

#include <new>
#include <LogAdapter.hpp>
#include "ErrorDef.hpp"
#include "StringUtility.hpp"

namespace ServerLib
{
typedef enum enmHashNodeStatus_str
{
    EHNS_KSTR_FREE = 0, //!<Hash节点未使用
    EHNS_KSTR_USED = 1, //!<Hash节点已使用
} ENMHASHNODESTATUS_STR;

template <int MAX_KEY_LEN>
class  CKeyString
{
public:
    char m_szKeyString[MAX_KEY_LEN];

    CKeyString()
    {
        m_szKeyString[0] = '\0';
    }

    CKeyString(const char* pszString)
    {
        SAFE_STRCPY(m_szKeyString, pszString, sizeof(m_szKeyString) - 1);
        m_szKeyString[MAX_KEY_LEN - 1] = '\0';
    }
};

template <typename VALUE_TYPE, int KEYSIZE>
class CHashMap_KStr
{
    typedef struct tagHashMapNode_KSTR
    {
        CKeyString<KEYSIZE> m_stPriKey; //!<char[]类型的节点主键值，根据这个Key计算出Hash值来找到节点
        VALUE_TYPE m_iValue; //!<存放数据
        int m_iIsNodeUsed; //!<节点是否使用 1-使用 0-未使用
        int m_iHashNext; //!<当Hash值冲突值，将新加节点放在节点后面，形成冲突链
    } THashMapNode_KSTR;

public:

    static CHashMap_KStr* CreateHashMap(char* pszMemoryAddress,
                                        const unsigned int uiFreeMemorySize,
                                        const int iNodeNumber);

    bool CompareKey(const CKeyString<KEYSIZE>& key1,const CKeyString<KEYSIZE>& key2);

    static size_t CountSize(const int iNodeNumber);

private:
    CHashMap_KStr();

public:
    CHashMap_KStr(int iNodeNumber);

    ~CHashMap_KStr();

public:

    //设置相应的指针
    int AttachHashMap(char* pszMemoryAddress);

    //!指定索引值来获取数据，一般用于遍历中，返回值0表示成功
    int GetValueByIndex(const int iNodeIndex, VALUE_TYPE& riValue);

    //!指定索引值来获取key，一般用于遍历中，返回值0表示成功
    int GetKeyByIndex(const int iNodeIndex, CKeyString<KEYSIZE>& riHashMapKey);

    //两个用于Assist工具的辅助函数
    //!指定索引值来获取数据，一般用于遍历中，返回值0表示成功
    int GetValueByIndexAssist(const int iNodeIndex, VALUE_TYPE& riValue);

    //!指定索引值来获取key，一般用于遍历中，返回值0表示成功
    int GetKeyByIndexAssist(const int iNodeIndex, CKeyString<KEYSIZE>& riHashMapKey);


    //!指定Key值来获取数据，返回值0表示成功
    int GetValueByKey(const CKeyString<KEYSIZE> stPriKey, VALUE_TYPE& riValue);

    VALUE_TYPE* GetValuePtrByKey(const CKeyString<KEYSIZE> stPriKey);

    //!清除对应Key的节点
    int DeleteByKey(const CKeyString<KEYSIZE> stPriKey, VALUE_TYPE& riValue);

    //!指定Key值来插入一个数据（如果有相同Key值的节点存在，则失败）
    int InsertValueByKey(const CKeyString<KEYSIZE> stPriKey, const VALUE_TYPE iValue);

    //!指定Key值来更新一个数据（如果未发现该Key值的数据则不做任何事）
    int UpdateValueByKey(const CKeyString<KEYSIZE> stPriKey, const VALUE_TYPE iValue);

    //!指定Key值来更新一个数据（如果未发现该Key值的数据则插入一个数据）
    int ReplaceValueByKey(const CKeyString<KEYSIZE> stPriKey, const VALUE_TYPE iValue);

    //!获取已用节点个数
    int GetUsedNodeNumber() const
    {
        return m_iUsedNodeNumber;
    }

    //!获取可用节点个数
    int GetFreeNodeNumber() const
    {
        return m_iNodeNumber - m_iUsedNodeNumber;
    }

    //!获取总共的节点个数
    int GetNodeSize() const
    {
        return m_iNodeNumber;
    }

    //!清除所有节点
    int EraseAll();

    //!在接口返回错误时，调用这个函数获取错误号
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    //!将数据Key值通过Hash转换成索引
    int HashKeyToIndex(const CKeyString<KEYSIZE> stPriKey) const;

    int BKDRHash(const char* szStr,int iStrLength) const;

    //!设置错误号
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    int m_iNodeNumber;

    int m_iErrorNO; //!错误码
    int m_iUsedNodeNumber; //!<已经使用的节点个数
    int m_iFirstFreeIndex; //!<空闲链表头节点
    THashMapNode_KSTR* m_pastHashNode; //!<所有存放的数据节点
    int* m_paiHashFirstIndex; //!<通过Key来Hash计算出的冲突链表的头节点索引

    //读HashMap的辅助指针
    THashMapNode_KSTR* m_pastHashNodeAssist; //!<所有存放的数据节点
    int* m_paiHashFirstIndexAssist; //!<通过Key来Hash计算出的冲突链表的头节点索引

};



template <typename VALUE_TYPE, int KEYSIZE>
CHashMap_KStr<VALUE_TYPE, KEYSIZE>* CHashMap_KStr<VALUE_TYPE, KEYSIZE>::CreateHashMap(char* pszMemoryAddress,
        const unsigned int uiFreeMemorySize,
        const int iNodeNumber)
{
    if(CountSize(iNodeNumber) > uiFreeMemorySize)
    {
        return NULL;
    }

    return new(pszMemoryAddress) CHashMap_KStr(iNodeNumber);
}

template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::AttachHashMap(char* pszMemoryAddress)
{
    __ASSERT_AND_LOG(m_iNodeNumber >= 0);
    m_pastHashNodeAssist = (THashMapNode_KSTR*)((char*)(pszMemoryAddress) + sizeof(CHashMap_KStr));
    m_paiHashFirstIndexAssist = (int*)((char*)(m_pastHashNodeAssist) + sizeof(THashMapNode_KSTR) * m_iNodeNumber);
    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNodeAssist,(unsigned int)m_paiHashFirstIndexAssist);

    m_pastHashNode = m_pastHashNodeAssist;
    m_paiHashFirstIndex = m_paiHashFirstIndexAssist;

    return 0;
}

template <typename VALUE_TYPE, int KEYSIZE>
size_t CHashMap_KStr<VALUE_TYPE, KEYSIZE>::CountSize(const int iNodeNumber)
{
    return sizeof(CHashMap_KStr) + (sizeof(THashMapNode_KSTR) + sizeof(int)) * iNodeNumber;
}

template <typename VALUE_TYPE, int KEYSIZE>
CHashMap_KStr<VALUE_TYPE, KEYSIZE>::CHashMap_KStr()
{


}

template <typename VALUE_TYPE, int KEYSIZE>
CHashMap_KStr<VALUE_TYPE, KEYSIZE>::CHashMap_KStr(int iNodeNumber)
{
    __ASSERT_AND_LOG(iNodeNumber > 0);

    m_iErrorNO = 0;

    m_iNodeNumber = iNodeNumber;

    m_pastHashNode = (THashMapNode_KSTR*)((char*)(this) + sizeof(CHashMap_KStr));
    m_paiHashFirstIndex = (int*)((char*)(m_pastHashNode) + sizeof(THashMapNode_KSTR) * iNodeNumber);
    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNode,(unsigned int)m_paiHashFirstIndex);

    EraseAll();
}

template <typename VALUE_TYPE, int KEYSIZE>
CHashMap_KStr<VALUE_TYPE, KEYSIZE>::~CHashMap_KStr()
{
}

template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::EraseAll()
{
    m_iUsedNodeNumber = 0;
    m_iFirstFreeIndex = 0;

    int i;
    for(i = 0; i < m_iNodeNumber; ++i)
    {
        m_pastHashNode[i].m_iHashNext = i + 1;
        m_pastHashNode[i].m_iIsNodeUsed = EHNS_KSTR_FREE;
    }

    m_pastHashNode[m_iNodeNumber-1].m_iHashNext = -1;

    for(i = 0; i < m_iNodeNumber; ++i)
    {
        m_paiHashFirstIndex[i] = -1;
    }

    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNode,(unsigned int)m_paiHashFirstIndex);
    return 0;
}

template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::GetValueByIndex(const int iNodeIndex, VALUE_TYPE& riValue)
{
    if(iNodeIndex < 0 || iNodeIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    if(!m_pastHashNode[iNodeIndex].m_iIsNodeUsed)
    {
        return -2;
    }

    riValue = m_pastHashNode[iNodeIndex].m_iValue;

    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNode,(unsigned int)m_paiHashFirstIndex);
    return 0;
}


//!指定索引值来获取Hash节点，一般用于遍历中，返回值0表示成功
template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::GetKeyByIndex(const int iNodeIndex, CKeyString<KEYSIZE>& riHashMapKey)
{
    if(iNodeIndex < 0 || iNodeIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    if(!m_pastHashNode[iNodeIndex].m_iIsNodeUsed)
    {
        return -2;
    }

    memcpy(&riHashMapKey,&(m_pastHashNode[iNodeIndex].m_stPriKey),sizeof(riHashMapKey) );

    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNode,(unsigned int)m_paiHashFirstIndex);

    return 0;
}

//////////////用于Assist工具////////////////////////

template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::GetValueByIndexAssist(const int iNodeIndex, VALUE_TYPE& riValue)
{
    if(iNodeIndex < 0 || iNodeIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    if(!m_pastHashNodeAssist[iNodeIndex].m_iIsNodeUsed)
    {
        return -2;
    }

    riValue = m_pastHashNodeAssist[iNodeIndex].m_iValue;

    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNodeAssist,(unsigned int)m_paiHashFirstIndexAssist);
    return 0;
}


//!指定索引值来获取Hash节点，一般用于遍历中，返回值0表示成功
template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::GetKeyByIndexAssist(const int iNodeIndex, CKeyString<KEYSIZE>& riHashMapKey)
{
    if(iNodeIndex < 0 || iNodeIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    if(!m_pastHashNodeAssist[iNodeIndex].m_iIsNodeUsed)
    {
        return -2;
    }

    memcpy(&riHashMapKey,&(m_pastHashNodeAssist[iNodeIndex].m_stPriKey),sizeof(riHashMapKey) );

    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNodeAssist,(unsigned int)m_paiHashFirstIndexAssist);

    return 0;
}

///////////////////////////////////////


template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::GetValueByKey(const CKeyString<KEYSIZE> stPriKey, VALUE_TYPE& riValue)
{
    int iHashIndex = HashKeyToIndex(stPriKey);

    if(iHashIndex < 0 || iHashIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    int iCurrentIndex = m_paiHashFirstIndex[iHashIndex];

    while(iCurrentIndex != -1)
    {
        //if(stPriKey == m_pastHashNode[iCurrentIndex].m_stPriKey)
        if(CompareKey(stPriKey,m_pastHashNode[iCurrentIndex].m_stPriKey))
        {
            riValue = m_pastHashNode[iCurrentIndex].m_iValue;
            break;
        }

        iCurrentIndex = m_pastHashNode[iCurrentIndex].m_iHashNext;
    }

    if(iCurrentIndex == -1)
    {
        return -2;
    }

    return 0;
}

template <typename VALUE_TYPE, int KEYSIZE>
VALUE_TYPE* CHashMap_KStr<VALUE_TYPE, KEYSIZE>::GetValuePtrByKey(const CKeyString<KEYSIZE> stPriKey)
{
    int iHashIndex = HashKeyToIndex(stPriKey);

    if(iHashIndex < 0 || iHashIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return NULL;
    }

    int iCurrentIndex = m_paiHashFirstIndex[iHashIndex];
    VALUE_TYPE* piValue = NULL;

    while(iCurrentIndex != -1)
    {
        //if(stPriKey == m_pastHashNode[iCurrentIndex].m_stPriKey)
        if(CompareKey(stPriKey,m_pastHashNode[iCurrentIndex].m_stPriKey))
        {
            piValue = &m_pastHashNode[iCurrentIndex].m_iValue;
            break;
        }

        iCurrentIndex = m_pastHashNode[iCurrentIndex].m_iHashNext;
    }

    if(iCurrentIndex == -1)
    {
        return NULL;
    }

    return piValue;
}

template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::DeleteByKey(const CKeyString<KEYSIZE> stPriKey, VALUE_TYPE& riValue)
{
    int iHashIndex = HashKeyToIndex(stPriKey);

    if(iHashIndex < 0 || iHashIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    int iPreIndex = -1;
    int iCurrentIndex = m_paiHashFirstIndex[iHashIndex];

    while(iCurrentIndex != -1)
    {
        //已经存在该Key则返回失败
        //if(stPriKey == m_pastHashNode[iCurrentIndex].m_stPriKey)
        if(CompareKey(stPriKey,m_pastHashNode[iCurrentIndex].m_stPriKey))
        {
            break;
        }

        iPreIndex = iCurrentIndex;
        iCurrentIndex = m_pastHashNode[iCurrentIndex].m_iHashNext;
    }

    //找到需要删除的节点
    if(iCurrentIndex != -1)
    {
        //是冲突链表头节点
        if(m_paiHashFirstIndex[iHashIndex] == iCurrentIndex)
        {
            m_paiHashFirstIndex[iHashIndex] = m_pastHashNode[iCurrentIndex].m_iHashNext;
        }
        //将上一个节点的Next索引等于当前要删除节点的Next索引
        else
        {
            m_pastHashNode[iPreIndex].m_iHashNext = m_pastHashNode[iCurrentIndex].m_iHashNext;
        }

        riValue = m_pastHashNode[iCurrentIndex].m_iValue;

        m_pastHashNode[iCurrentIndex].m_iIsNodeUsed = EHNS_KSTR_FREE;
        m_pastHashNode[iCurrentIndex].m_iHashNext = m_iFirstFreeIndex;
        m_iFirstFreeIndex = iCurrentIndex;
        --m_iUsedNodeNumber;
    }
    else
    {
        // 未找到，返回失败
        SetErrorNO(EEN_HASH_MAP__NODE_NOT_EXISTED);
        return -2;
    }

    return 0;
}

template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::InsertValueByKey(const CKeyString<KEYSIZE> stPriKey, const VALUE_TYPE iValue)
{
    int iHashIndex = HashKeyToIndex(stPriKey);

    if(iHashIndex < 0 || iHashIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    //已经没有可用的节点了
    if(m_iFirstFreeIndex < 0)
    {
        SetErrorNO(EEN_HASH_MAP__NODE_IS_FULL);

        return -2;
    }

    int iPreIndex = -1;
    int iCurrentIndex = m_paiHashFirstIndex[iHashIndex];

    while(iCurrentIndex != -1)
    {
        //已经存在该Key则返回失败
        //if(stPriKey == m_pastHashNode[iCurrentIndex].m_stPriKey)
        if(CompareKey(stPriKey,m_pastHashNode[iCurrentIndex].m_stPriKey))
        {
            SetErrorNO(EEN_HASH_MAP__INSERT_FAILED_FOR_KEY_DUPLICATE);

            return -3;
        }

        iPreIndex = iCurrentIndex;
        iCurrentIndex = m_pastHashNode[iCurrentIndex].m_iHashNext;
    }

    int iNowAssignIdx = m_iFirstFreeIndex;
    m_iFirstFreeIndex = m_pastHashNode[m_iFirstFreeIndex].m_iHashNext;
    ++m_iUsedNodeNumber;
    m_pastHashNode[iNowAssignIdx].m_stPriKey = stPriKey;
    m_pastHashNode[iNowAssignIdx].m_iValue = iValue;
    m_pastHashNode[iNowAssignIdx].m_iIsNodeUsed = EHNS_KSTR_USED;
    m_pastHashNode[iNowAssignIdx].m_iHashNext = -1;

    //是冲突链表的第一个节点
    if(m_paiHashFirstIndex[iHashIndex] == -1)
    {
        m_paiHashFirstIndex[iHashIndex] = iNowAssignIdx;
    }
    else
    {
        m_pastHashNode[iPreIndex].m_iHashNext = iNowAssignIdx;
    }

    return 0;
}

template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::UpdateValueByKey(const CKeyString<KEYSIZE> stPriKey, const VALUE_TYPE iValue)
{
    VALUE_TYPE* piValue = GetValuePtrByKey(stPriKey);
    if(!piValue)
    {
        SetErrorNO(EEN_HASH_MAP__NODE_NOT_EXISTED);
        return -1;
    }

    *piValue = iValue;

    return 0;
}

template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::ReplaceValueByKey(const CKeyString<KEYSIZE> stPriKey, const VALUE_TYPE iValue)
{
    int iHashIndex = HashKeyToIndex(stPriKey);

    if(iHashIndex < 0 || iHashIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    int iPreIndex = -1;
    /*assert(m_paiHashFirstIndex);
    assert(m_pastHashNode);*/
    int iCurrentIndex = m_paiHashFirstIndex[iHashIndex];

    //while(iCurrentIndex != -1)
    while(iCurrentIndex >= 0)
    {
        //已经存在该Key则直接更新
        //if(stPriKey == m_pastHashNode[iCurrentIndex].m_stPriKey)
        if(CompareKey(stPriKey,m_pastHashNode[iCurrentIndex].m_stPriKey))
        {
            m_pastHashNode[iCurrentIndex].m_iValue = iValue;
            return 0;
        }

        iPreIndex = iCurrentIndex;
        iCurrentIndex = m_pastHashNode[iCurrentIndex].m_iHashNext;
    }

    //已经没有可用的节点了
    if(m_iFirstFreeIndex < 0)
    {
        SetErrorNO(EEN_HASH_MAP__NODE_IS_FULL);

        return -2;
    }

    int iNowAssignIdx = m_iFirstFreeIndex;
    m_iFirstFreeIndex = m_pastHashNode[m_iFirstFreeIndex].m_iHashNext;
    ++m_iUsedNodeNumber;
    m_pastHashNode[iNowAssignIdx].m_stPriKey = stPriKey;
    m_pastHashNode[iNowAssignIdx].m_iValue = iValue;
    m_pastHashNode[iNowAssignIdx].m_iIsNodeUsed = EHNS_KSTR_USED;
    m_pastHashNode[iNowAssignIdx].m_iHashNext = -1;

    //是冲突链表的第一个节点
    if(m_paiHashFirstIndex[iHashIndex] == -1)
    {
        m_paiHashFirstIndex[iHashIndex] = iNowAssignIdx;
    }
    else
    {
        m_pastHashNode[iPreIndex].m_iHashNext = iNowAssignIdx;
    }

    return 0;
}


const unsigned int STR_SEED = 131;
const unsigned int STR_MASK = 0x7FFFFFFF;

// BKDR Hash Function
template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::BKDRHash(const char* szStr,int iStrLength) const
{
    //unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;
    unsigned int *piTemp = (unsigned int*)szStr;

    //将szStr按每个字节进行操作
    unsigned int i;
    for( i = 0; i < iStrLength / sizeof(int); ++i)
    {
        //hash += piTemp[i];
        hash = hash * STR_SEED + (piTemp[i]);
    }

    if(iStrLength % sizeof(int) > 0)
    {
        unsigned char* pByte = (unsigned char*)szStr;
        pByte += (iStrLength - (iStrLength % sizeof(int)));
        unsigned int uiTemp = 0; // 之前未初始化，导致对相同的key生成的hash location不相同
        memcpy((void *)&uiTemp, (const void *)pByte, iStrLength%sizeof(int));
        hash += uiTemp;
    }

    return (hash & STR_MASK);
}


template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::HashKeyToIndex(const CKeyString<KEYSIZE> stPriKey) const
{
    int iRealLength = strnlen(stPriKey.m_szKeyString,sizeof(stPriKey.m_szKeyString));
    int hash_index=BKDRHash(stPriKey.m_szKeyString,iRealLength);

    hash_index=hash_index % m_iNodeNumber;
    //TRACESVR("HashKeyToIndex.Key=%s, hash_index=%d ,m_iNodeNumber=%d  \n",(char*)stPriKey.KeyString, hash_index,m_iNodeNumber);
    return hash_index;
}

template <typename VALUE_TYPE, int KEYSIZE>
bool CHashMap_KStr<VALUE_TYPE, KEYSIZE>::CompareKey(const CKeyString<KEYSIZE>& key1,const CKeyString<KEYSIZE>& key2)
{
    //if(memcmp((const void*)&key1, (const void*)&key2, sizeof(CKeyString<KEYSIZE>)) == 0)
    //if(strcmp((const char*)key1.KeyString,(const char*)key2.KeyString) == 0)
    if(strncmp((const char*)key1.m_szKeyString,(const char*)key2.m_szKeyString,sizeof(key1.m_szKeyString)) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
}

#endif //__HASH_MAP_KSTR_HPP__
