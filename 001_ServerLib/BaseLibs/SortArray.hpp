
#ifndef __ARRAY_SORT_HPP__
#define __ARRAY_SORT_HPP__

#include <stdlib.h>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 排序数组算法, 参考自幻想代码
// 排序的顺序为按 compar 返回值进行排序. 即:
// compar(1, 2): 返回-1, 则从小到大排序
// compar(1, 2): 返回1, 则从大到小排序

template <int offset, typename T>
bool CheckZero(const void *p1)
{
    T a1 = *(T *)((char *)p1 + offset);
    return (bool)a1;
}

#define check_zero(s, m) CheckZero<offsetof(s, m), typeof(((s*)0)->m)>

template <class T>
int Cmp(const void *a1, const void *a2)
{
    T *t1 = (T*)a1;
    T *t2 = (T*)a2;
    if (*t1 < *t2)
    {
        return -1;
    }
    else if (*t1 > *t2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

template <class T>
int DescCmp(const void *a1, const void *a2)
{
    return -Cmp<T>(a1, a2);
}

template <int offset, typename T>
int CmpOneKey(const void *p1, const void *p2)
{
    T a1 = *(T *)((char *)p1 + offset);
    T a2 = *(T *)((char *)p2 + offset);
    if (a1 < a2)
    {
        return -1;
    }
    else if (a1 > a2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 比较对象的某个成员,可用于MyDeleteArrayElement的函数指针
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <int offset, typename T>
int CmpOneMem(const void *pObj, const void *pMem)
{
    T a1 = *(T *)((char *)pObj + offset);
    T a2 = *(T *)(pMem);
    if (a1 < a2)
    {
        return -1;
    }
    else if (a1 > a2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


template <int offset, typename T>
int DescCmpOneKey(const void *p1, const void *p2)
{
    return -CmpOneKey<offset, T>(p1, p2);
}

template <int offset1, typename T1, int offset2, typename T2>
int CmpTwoKey(const void *p1, const void *p2)
{
    int diff = CmpOneKey<offset1, T1>(p1, p2);
    if (diff == 0)
    {
        return CmpOneKey<offset2, T2>(p1, p2);
    }
    return diff;
}

template <int offset1, typename T1, int offset2, typename T2>
int DescCmpTwoKey(const void *p1, const void *p2)
{
    return -CmpTwoKey<offset1, T1, offset2, T2>(p1, p2);
}

#define cmp_one_key(s, m) CmpOneKey<offsetof(s, m), typeof(((s*)0)->m)>
#define cmp_two_key(s, m1, m2) CmpTwoKey<offsetof(s, m1), typeof(((s*)0)->m1), offsetof(s, m2), typeof(((s*)0)->m2)>

#define cmp_one_key_desc(s, m) DescCmpOneKey<offsetof(s, m), typeof(((s*)0)->m)>
#define cmp_two_key_desc(s, m1, m2) DescCmpTwoKey<offsetof(s, m1), typeof(((s*)0)->m1), offsetof(s, m2), typeof(((s*)0)->m2)>

#define cmp_one_mem(s, m) CmpOneMem<offsetof(s, m), typeof(((s*)0)->m)>

typedef int (*MYBCOMPAREFPTR) (const void *, const void *);

// 查找一个值为key的元素
// 返回值: piEqual为1时, 返回找到等于key的索引, piEqual为0时, 找到第一个小于key的值
int MyBSearch (const void *key, const void *base, int nmemb, int size, int *piEqual, MYBCOMPAREFPTR compar);

// 插入一个值为key的元素
// iUnique: 1为不允许重复, 0 - 为允许重复
// 返回值: -1 失败, >=0 成功
int MyBInsert (const void *key, const void *base, int *pnmemb, int size, int iUnique, MYBCOMPAREFPTR compar);

// 删除索引为index的元素
// 返回值: -1 失败, 0 成功
int MyIDelete (const void *base, int *pnmemb, int size, int index);

// 删除值为key的元素
// 返回值: -1 失败, 0 成功
int MyBDelete (const void *key, const void *base, int *pnmemb, int size, MYBCOMPAREFPTR compar);


// 插入一个值为key的元素
// iUnique: 1为不允许重复, 0 - 为允许重复
// 返回值: -1 失败, >=0 成功
int MyBInsert2 (const void *key, const void *base, int *pnmemb, int size, int iUnique, MYBCOMPAREFPTR compar);

int MyBDelete2 (const void *key, const void *base, int *pnmemb, int size, int (*compar) (const void *, const void *));


// 以下操作适合情况：
// 元素索引为key。但元素查找和删除时的相等是以objcmp的比较结果来判断。一个key可能对应多个元素。这些元素没有排序比较，而是以先后顺序来插入
int MyBSearchDup (const void *key, const void *base, int nmemb, int size, int *piEqual,  MYBCOMPAREFPTR compar, MYBCOMPAREFPTR objcmp);
int MyBInsertDup (const void *key, const void *base, int *pnmemb, int size, int iUnique, bool bTail, MYBCOMPAREFPTR compar, MYBCOMPAREFPTR objcmp);
int MyBDeleteDup (const void *key, const void *base, int *pnmemb, int size,  MYBCOMPAREFPTR compar, MYBCOMPAREFPTR objcmp);


// 返回删除指定成员值后剩下的数组（无序）元素个数
int MyDeleteArrayElement (const void *key, const void *base, int *pnmemb, int size, int (*compar) (const void *, const void *));

int MyDeleteArray_If(const void *base, int *pnmemb, int iSize, bool (*IfPred)(const void *));

#endif
