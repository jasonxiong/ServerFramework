/**
*@file HashMap_K32.hpp
*@author
*@date
*@version 1.0
*@brief
*
*
*/

#ifndef __HASH_MAP_K32_HPP__
#define __HASH_MAP_K32_HPP__



#include <new>



#include "ErrorDef.hpp"



namespace ServerLib
{
class CHashMap_K32
{

    typedef enum enmHashNodeStatus
    {
        EHNS_K32_FREE = 0, //!<Hash节点未使用
        EHNS_K32_USED = 1, //!<Hash节点已使用
    } ENMHASHNODESTATUS;

    //const int HASHMAP_K32_MAX_NODE_NUMBER = 200000;

    typedef struct tagHashMapNode_K32
    {
        unsigned int m_uiKey; //!<节点主键值，根据这个Key计算出Hash值来找到节点
        int m_iValue; //!<存放数据
        int m_iIsNodeUsed; //!<节点是否使用 1-使用 0-未使用
        int m_iHashNext; //!<当Hash值冲突值，将新加节点放在节点后面，形成冲突链
    } THashMapNode_K32;

public:
    static CHashMap_K32* CreateHashMap(char* pszMemoryAddress,
                                       const unsigned int uiFreeMemorySize,
                                       const int iNodeNumber);

    static size_t CountSize(const int iNodeNumber);

    static CHashMap_K32* ResumeHashMap(char* pszMemoryAddress,
                                       const unsigned int uiFreeMemorySize, const int iNodeNumber);

private:
    CHashMap_K32();

public:
    CHashMap_K32(int iNodeNumber);

    ~CHashMap_K32();

public:
    //!指定索引值来获取数据，一般用于遍历中，返回值0表示成功
    int GetValueByIndex(const int iNodeIndex, int& riValue);

    //!指定Key值来获取数据，返回值0表示成功
    int GetValueByKey(const unsigned int uiKey, int& riValue);

    int* GetValuePtrByKey(const unsigned int uiKey);

    //!清除对应Key的节点
    int DeleteByKey(const unsigned int uiKey, int& riValue);

    //!指定Key值来插入一个数据（如果有相同Key值的节点存在，则失败）
    int InsertValueByKey(const unsigned int uiKey, const int iValue);

    //!指定Key值来更新一个数据（如果未发现该Key值的数据则不做任何事）
    int UpdateValueByKey(const unsigned int uiKey, const int iValue);

    //!指定Key值来更新一个数据（如果未发现该Key值的数据则插入一个数据）
    int ReplaceValueByKey(const unsigned int uiKey, const int iValue);

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
    int HashKeyToIndex(const unsigned int uiKey) const;

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
    THashMapNode_K32* m_pastHashNode; //!<所有存放的数据节点
    int* m_paiHashFirstIndex; //!<通过Key来Hash计算出的冲突链表的头节点索引

};



}

#endif //__HASH_MAP_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
