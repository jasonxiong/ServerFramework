/**
*@file SortAlgorithm.hpp
*@author jasonxiong
*@date 2009-01-08
*@version 1.0
*@brief ≈≈–ÚÀ„∑®¿‡
*
*
*/

#ifndef __SORT_ALGORITHM_HPP__
#define __SORT_ALGORITHM_HPP__

namespace ServerLib
{

template <typename T>
class CDefaultSortCmp
{
public:
    CDefaultSortCmp() {}
    ~CDefaultSortCmp() {}

public:
    int operator () (const T& rstValue1, const T& rstValue2);
};

template <typename T>
int CDefaultSortCmp<T>::operator() (const T& rstValue1, const T& rstValue2)
{
    if(rstValue1 == rstValue2)
    {
        return 0;
    }
    else if(rstValue1 > rstValue2)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

template <typename T>
class CDefaultSwapFunc
{
public:
    CDefaultSwapFunc() {}
    ~CDefaultSwapFunc() {}

public:
    int operator () (T& rstValue1, T& rstValue2);
};

template <typename T>
int CDefaultSwapFunc<T>::operator() (T& rstValue1, T& rstValue2)
{
    T stTempValue = rstValue1;
    rstValue1 = rstValue2;
    rstValue2 = stTempValue;

    return 0;
}

template <typename T, typename CMP_FUNC = CDefaultSortCmp<T>, typename SWAP_FUNC = CDefaultSwapFunc<T> >
class CBubbleSort
{
public:
    static int Sort(T* astItem, int iItemNum, bool bOrderDesc = false);
};

template <typename T, typename CMP_FUNC, typename SWAP_FUNC>
int CBubbleSort<T, CMP_FUNC, SWAP_FUNC>::Sort(T* astItem, int iItemNum, bool bOrderDesc)
{
    if(astItem == NULL || iItemNum <= 0)
    {
        return -1;
    }

    CMP_FUNC SortCompare;
    SWAP_FUNC MySwap;
    bool bFinish = false;

    int i;
    for(i = iItemNum - 1; i > 0 && !bFinish; --i)
    {
        bFinish = true;

        int j;
        for(j = 0; j < i; ++j)
        {
            if(bOrderDesc)
            {
                if(SortCompare(astItem[j], astItem[j+1]) < 0)
                {
                    MySwap(astItem[j], astItem[j+1]);
                    bFinish = false;
                }
            }
            else
            {
                if(SortCompare(astItem[j], astItem[j+1]) > 0)
                {
                    MySwap(astItem[j], astItem[j+1]);
                    bFinish = false;
                }
            }
        }
    }

    return 0;
}

template <typename T, typename CMP_FUNC = CDefaultSortCmp<T>, typename SWAP_FUNC = CDefaultSwapFunc<T> >
class CQuickSort
{
public:
    static int Sort(T* astItem, int iItemNum, bool bOrderDesc = false);
private:
    static void RecursiveSort(T* astItem, int left, int right, bool bOrderDesc, CMP_FUNC& SortCompare, SWAP_FUNC& MySwap);
};

template <typename T, typename CMP_FUNC, typename SWAP_FUNC>
int CQuickSort<T, CMP_FUNC, SWAP_FUNC>::Sort(T* astItem, int iItemNum, bool bOrderDesc)
{
    if(astItem == NULL || iItemNum <= 0)
    {
        return -1;
    }
    CMP_FUNC SortCompare;
    SWAP_FUNC MySwap;

    RecursiveSort(astItem, 0, iItemNum - 1, bOrderDesc, SortCompare, MySwap);
    return 0;
}

template <typename T, typename CMP_FUNC, typename SWAP_FUNC>
void CQuickSort<T, CMP_FUNC, SWAP_FUNC>::RecursiveSort(T* astItem, int left, int right, bool bOrderDesc, CMP_FUNC& SortCompare, SWAP_FUNC& MySwap)
{
    int i,j,iTemp;
    i = left;
    j = right;
    iTemp = 0;
    T middle=astItem[(rand() % (right-left+1)) + left];
    if(bOrderDesc)
    {
        do
        {
            while(SortCompare(astItem[i], middle) > 0 && i < right)
            {
                i++;
            }
            while(SortCompare(astItem[j], middle) < 0 && j > left)
            {
                j--;
            }
            if(i <= j)
            {
                MySwap(astItem[i], astItem[j]);
                i++;
                j--;
            }
        }
        while(i<=j);
    }
    else
    {
        do
        {
            while(SortCompare(astItem[i], middle) < 0 && i < right)
            {
                i++;
            }
            while(SortCompare(astItem[j], middle) > 0 && j > left)
            {
                j--;
            }
            if(i <= j)
            {
                MySwap(astItem[i], astItem[j]);
                i++;
                j--;
            }
        }
        while(i<=j);
    }

    if(left<j)
    {
        RecursiveSort(astItem,left,j, bOrderDesc, SortCompare, MySwap);
    }
    if(right>i)
    {
        RecursiveSort(astItem,i,right, bOrderDesc, SortCompare, MySwap);
    }
}

}

#endif //__SORT_ALGORITHM_HPP__
///:~
