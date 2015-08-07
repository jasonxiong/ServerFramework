// ********************************************************************
// 作    者： amacroli(idehong@gmail.com)
// 创建日期： 2011年7月8日
// 描    述： 附带属性的锁
// 应    用：
// ********************************************************************

#ifndef __TRANSACTIONLOCK_HPP__
#define __TRANSACTIONLOCK_HPP__

template<typename T>
class CTransactionLock
{
public:
    CTransactionLock(void)
    {
        UnLock();
    }
    ~CTransactionLock(void)
    {
        UnLock();
    }

public:
    const T* GetLockInfo()
    {
        return &m_stLockInfo;
    }

    bool IsLock()
    {
        return m_bLock;
    }

    void Lock(const T* pLockInfo)
    {
        m_stLockInfo = *pLockInfo;
        m_bLock = true;
    }

    void UnLock()
    {
        memset(&m_stLockInfo, 0, sizeof(m_stLockInfo));
        m_bLock = false;
    }

private:
    bool m_bLock;
    T m_stLockInfo;
};
#endif // __TRANSACTIONLOCK_HPP__
