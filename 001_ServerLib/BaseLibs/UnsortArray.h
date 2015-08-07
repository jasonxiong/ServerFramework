/**
* Copyright (c) 2012, Tecent
* All rights reserved.
*
* @file UnsortArray.h
* @brief 顺序无关数组，对元素顺序不关心的case可以使用
*        模板类支持数组类型为任意类型，对于复杂类型需要
*        实现拷贝构造函数、赋值构造函数
*
* @version 1.0
* @author philipyao, owentou, philipyao@tencent.com, owentou@tencent.com
* @date 2012.05.14
*
* @history
*
*
*/

#ifndef  __UNSORT_ARRAY_H__
#define  __UNSORT_ARRAY_H__

namespace BaseLib
{

/**
 * @class UnsortArray array which elements sequence is not cared about
 */
template<class T, int SIZE >
class UnsortArray
{
private:
    T m_aContent[SIZE];
    int m_iSize;        /** current elements count */

public:
    typedef T* iterator;
    typedef const T* const_iterator;

    /**
     * @brief constructor
     * @param[in] iCapcity maximal elements count
     */
    UnsortArray()
    {
    }

    /**
     * @brief deconstructor
     */
    ~UnsortArray()
    {
    }

public:

    inline T& get(int iIdx) { return m_aContent[iIdx]; }

    inline const T& get(int iIdx) const { return m_aContent[iIdx]; }

    /**
     * @brief overide [] operator, get the element with index iIdx
     * @param[in] iIdx index
     * @return array element at iIdx
     * @see get()
     */
    inline T& operator [](int iIdx)
    {
        return get(iIdx);
    }

    /**
     * @brief overide [] operator, get the element with index iIdx
     * @param[in] iIdx index
     * @return array element at iIdx
     * @see get()
     */
    inline const T& operator [](int iIdx) const
    {
        return get(iIdx);
    }

    /**
     * @brief get the element with index iIdx
     * @param[in] iIdx index
     * @return array element at iIdx
     * @see operator []()
     */
    T& At(int iIdx)
    {
        return get(iIdx);
    }

    /**
     * @brief remove the element with index iIdx
     * @param[in] iIdx index
     */
    void Remove(int iIdx)
    {
        if (iIdx >= m_iSize)
        {
            return ;
        }
        else if (iIdx == (m_iSize - 1))
        {
            m_iSize --;
        }
        else
        {
            m_aContent[iIdx] = m_aContent[m_iSize - 1];
            m_iSize--;
        }
    }

    /**
     * @brief add one element
     * @param[in] T element value
     * @return 0 on success, -1 on error
     *
     * If there are no available space to hold the value, -1 will be returned.
     *
     */
    int Add(const T& value)
    {
        if (Full())
        {
            return -1;
        }

        m_aContent[m_iSize ++] = value;

        return 0;
    }

    /**
     * @brief clear the whole array
     */
    void Clear()
    {
        m_iSize = 0;
    }

    /**
     * @see stl begin
     * @return first element
     */
    const_iterator begin() const
    {
        return m_aContent;
    }

    /**
     * @see stl begin
     * @return first element
     */
    iterator begin()
    {
        return m_aContent;
    }

    /**
     * @see stl end
     * @return end
     */
    const_iterator end() const
    {
        return m_aContent + m_iSize;
    }

    /**
     * @see stl end
     * @return end
     */
    iterator end()
    {
        return m_aContent + m_iSize;
    }

    void pop_back(int iNum = 1)
    {
        m_iSize -= iNum;
        m_iSize = (m_iSize > 0)? m_iSize: 0;
    }

    int push_back(const T& value)
    {
        return Add(value);
    }

    /**
     * @brief check if the array is empty
     * @return true if array is empty, false otherwise
     */
    bool Empty() const
    {
        return (m_iSize == 0);
    }

    /**
     * @brief check if the array is full
     * @return true if array is full, false otherwise
     */
    bool Full() const
    {
        return m_iSize >= SIZE;
    }

    /**
     * @brief get array element count
     * @return array size, not capacity
     */
    int GetSize() const
    {
        return m_iSize;
    }

};

}

#endif   /* __UNSORT_ARRAY_H__ */

