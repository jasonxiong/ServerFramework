#ifndef __UINPRV_MANAGER_HPP__
#define __UINPRV_MANAGER_HPP__

// Uin权限管理池

#include "SharedMemory.hpp"

// 权限信息
typedef struct tagUinPrivInfo
{
    unsigned int m_uiUin;                                    	/*   Mt号码 */
    unsigned int m_uiPriv;                                   	/*   权限 */
} TUinPrivInfo;

// 权限管理类, 管理共享内存中的权限对象
// 同时被应用程序和更新进程使用

typedef struct tagUinPrivController
{
    int* m_piNumber;
    TUinPrivInfo* m_pstCache;
} TUinPrivController;

class CUinPrivManager
{
public:

    // 初始化管理池
    //   参数: pszShmName 共享内存Key
    int Initialize(int iUinNumber, const char* pszShmName, bool bIsUinPrivServer = false);

    // 查询指定Uin的权限信息
    //   uiUin:  查询Uin
    //   bActiveCache: 查询主备缓冲区 (默认主)
    //   返回值: 0 - 成功, 负值失败
    TUinPrivInfo* GetUinPrivInfo(unsigned int uiUin, bool bActiveCache = true);

    // 打印
    void Trace();
public:
    //////////////////////////////////////////////////////////////////////////
    // 以下接口给ServerUinPrivServer使用

    // 切换主备缓冲区
    void SwitchActiveCache();

    // 清空备份缓冲区
    int CleanBackupCache();

    // 创建对象
    // bActiveCache: 是否在主缓冲区创建
    int InsertUinPrivInfo(const TUinPrivInfo& rstUinPrivInfo, bool bActiveCache);

    // 删除对象
    // bActiveCache: 是否在主缓冲区删除
    int DeleteUinPrivInfo(const unsigned int uiUin, bool bActiveCache);

    // 添加对象， 不排序
    // bActiveCache: 是否在主缓冲区创建
    int InsertUinPrivInfoWithoutSort(const TUinPrivInfo& rstUinPrivInfo, bool bActiveCache);

    // 排序对象
    void SortUinPrivInfo(bool bActiveCache);

    // 获取数量
    int GetUinPrivNumber(bool bActiveCache);

    // 获取权限
    const TUinPrivInfo* GetUinPrivInfoByIndex(int iIndex, bool bActiveCache);

private:
    //////////////////////////////////////////////////////////////////////////
    // 内部接口
    size_t GetObjTotalSize();

private:
    // 共享内存
    CSharedMemory m_stSharedMemory;
    int m_iMaxUinNumber;
    size_t m_iObjTotalSize;

    // 主备缓冲区控制器
    TUinPrivController m_astUinPrivControllers[2];

    // 主备标识
    int* m_piActiveController;
};

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
