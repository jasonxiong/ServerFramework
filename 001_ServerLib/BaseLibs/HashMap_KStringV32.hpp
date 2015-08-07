/**
*@file HashMap_KStringV32.hpp
*@author
*@date
*@version 1.0
*@brief
*
*
*/

#ifndef __HASH_MAP_KSTRINGV32_HPP__
#define __HASH_MAP_KSTRINGV32_HPP__



#include <new>

#include "ErrorDef.hpp"



namespace ServerLib
{
#include "StringUtility.hpp"

#define MAX_KEY_SIZE 64

typedef enum enmHashNodeStatus
{
    EHNS_KSTRINGV32_FREE = 0, //!<Hash节点未使用
    EHNS_KSTRINGV32_USED = 1, //!<Hash节点已使用
} ENMHASHNODESTATUS;

typedef struct tagDataStringV32
{
    char KeyStringV32[MAX_KEY_SIZE];

    tagDataStringV32()
    {
        KeyStringV32[0] = '\0';
    }

    tagDataStringV32(const char* pszStringV32)
    {
        SAFE_STRCPY(KeyStringV32, pszStringV32, sizeof(KeyStringV32) - 1);
        KeyStringV32[MAX_KEY_SIZE - 1] = '\0';
    }
} TDataStringV32;
//
//const int HASHMAP_KSTRINGV32_MAX_NODE_NUMBER = 200000;

class CHashMap_KStringV32
{
    typedef struct tagHashMapNode_KSTRINGV32
    {
        TDataStringV32 m_stPriKey; //!<char[]类型的节点主键值，根据这个Key计算出Hash值来找到节点
        int m_iValue; //!<存放数据
        int m_iIsNodeUsed; //!<节点是否使用 1-使用 0-未使用
        int m_iHashNext; //!<当Hash值冲突值，将新加节点放在节点后面，形成冲突链
    } THashMapNode_KSTRINGV32;

public:

    static CHashMap_KStringV32* CreateHashMap(char* pszMemoryAddress,
            const unsigned int uiFreeMemorySize,
            const int iNodeNumber);




    bool CompareKey(const TDataStringV32& key1,const TDataStringV32& key2);

    static size_t CountSize(const int iNodeNumber);

private:
    CHashMap_KStringV32();

public:
    CHashMap_KStringV32(int iNodeNumber);

    ~CHashMap_KStringV32();

public:

    //设置相应的指针
    int AttachHashMap(char* pszMemoryAddress);

    ////设置m_iUseFlag1
    //int SetUseFlagNW(const int iUseFlag);

    ////获取m_iUseFlag1
    //int& GetUseFlagNW();

    ////设置m_iUseFlag2
    //int SetUseFlagNR(const int iUseFlag);

    ////获取m_iUseFlag2
    //int& GetUseFlagNR();

    //!指定索引值来获取数据，一般用于遍历中，返回值0表示成功
    int GetValueByIndex(const int iNodeIndex, int& riValue);

    //!指定索引值来获取key，一般用于遍历中，返回值0表示成功
    int GetKeyByIndex(const int iNodeIndex, TDataStringV32& riHashMapKey);

    //两个用于Assist工具的辅助函数
    //!指定索引值来获取数据，一般用于遍历中，返回值0表示成功
    int GetValueByIndexAssist(const int iNodeIndex, int& riValue);

    //!指定索引值来获取key，一般用于遍历中，返回值0表示成功
    int GetKeyByIndexAssist(const int iNodeIndex, TDataStringV32& riHashMapKey);




    //!指定Key值来获取数据，返回值0表示成功
    int GetValueByKey(const TDataStringV32 stPriKey, int& riValue);

    int* GetValuePtrByKey(const TDataStringV32 stPriKey);

    //!清除对应Key的节点
    int DeleteByKey(const TDataStringV32 stPriKey, int& riValue);

    //!指定Key值来插入一个数据（如果有相同Key值的节点存在，则失败）
    int InsertValueByKey(const TDataStringV32 stPriKey, const int iValue);

    //!指定Key值来更新一个数据（如果未发现该Key值的数据则不做任何事）
    int UpdateValueByKey(const TDataStringV32 stPriKey, const int iValue);

    //!指定Key值来更新一个数据（如果未发现该Key值的数据则插入一个数据）
    int ReplaceValueByKey(const TDataStringV32 stPriKey, const int iValue);

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
    int HashKeyToIndex(const TDataStringV32 stPriKey) const;

    int BKDRHash(const char* szStr,int iStrLength) const;

    //!设置错误号
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    int m_iNodeNumber;

    //int m_iUseFlagNW;//标记该HashMap是否被备份工具占用中，其值参见ENMHASHUSEFLAG，该标记NameServer只能写
    //int m_iUseFlagNR;//标记该HashMap是否被备份工具占用中，其值参见ENMHASHUSEFLAG，该标记NameServer只能读

    int m_iErrorNO; //!错误码
    int m_iUsedNodeNumber; //!<已经使用的节点个数
    int m_iFirstFreeIndex; //!<空闲链表头节点
    THashMapNode_KSTRINGV32* m_pastHashNode; //!<所有存放的数据节点
    int* m_paiHashFirstIndex; //!<通过Key来Hash计算出的冲突链表的头节点索引

    //读HashMap的辅助指针
    THashMapNode_KSTRINGV32* m_pastHashNodeAssist; //!<所有存放的数据节点
    int* m_paiHashFirstIndexAssist; //!<通过Key来Hash计算出的冲突链表的头节点索引

};
}

#endif //__HASH_MAP_KSTRINGV32_HPP__
