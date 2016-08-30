
#ifndef _BASELIB_STATICIDXLIST_H_
#define _BASELIB_STATICIDXLIST_H_

#include <algorithm>
#include <cstdlib>
#include <assert.h>

namespace BaseLib
{
    template<typename TObj, typename TSize>
    struct StaticIdxListNode
    {
        TSize iPreIdx;
        TSize iNextIdx;
        bool bIsInited;
        char stObjBin[sizeof(TObj)];
    };

    /**
     * 可用于共享内存的C++型链表
     * @warning 注意：如果是新创建的结构，需要执行construct函数初始化,如果从共享内存恢复，无需执行
     * @note 目标结构体至少要有默认构造函数, 构造函数最多三个参数
     * @note 内存消耗为 (sizeof(TObj) + 2 * sizeof(size_type)) * (MAX_SIZE + 1)
     */
    template<typename TObj, int MAX_SIZE>
    class StaticIdxList
    {
    public:
        typedef int size_type;
        typedef StaticIdxListNode<TObj, size_type> node_type;
        typedef TObj value_type;
        typedef StaticIdxList<TObj, MAX_SIZE> self_type;

        /**
         * 迭代器类型
         */
        template<typename ITObj>
        class Iterator
        {
        public:
            typedef Iterator<ITObj> self_type;
            typedef typename StaticIdxList<TObj, MAX_SIZE>::self_type list_type;

        private:
            mutable size_type iIndex;
            mutable list_type* m_pListPtr;

        public:
            Iterator(size_type index, const list_type* const pListPtr):
                iIndex(index),
                m_pListPtr(const_cast<list_type*>(pListPtr))
            {
            }

            inline size_type index() const { return iIndex; }

            self_type& operator++() const
            {
                iIndex = m_pListPtr->GetNextIdx(iIndex);

                return const_cast<Iterator&>(*this);
            }

            self_type operator++(int) const
            {
                Iterator stRet = (*this);
                ++ stRet;
                return stRet;
            }

            self_type& operator--() const
            {
                iIndex = m_pListPtr->GetPreIdx(iIndex);

                return const_cast<Iterator&>(*this);
            }

            self_type operator--(int) const
            {
                Iterator stRet = (*this);
                -- stRet;
                return stRet;
            }

            ITObj* get()
            {
                return reinterpret_cast<ITObj*>(m_pListPtr->m_stData[iIndex].stObjBin);
            }

            const ITObj* get() const
            {
                return reinterpret_cast<const ITObj*>(m_pListPtr->m_stData[iIndex].stObjBin);
            }

            friend bool operator==(const Iterator& l, const Iterator& r)
            {
                return l.m_pListPtr == r.m_pListPtr && l.iIndex == r.iIndex;
            }

            friend bool operator!=(const Iterator& l, const Iterator& r)
            {
                return !(l == r);
            }

            inline ITObj* operator->()
            {
                return get();
            }

            inline const ITObj* operator->() const
            {
                return get();
            }

            inline ITObj& operator*()
            {
                return *get();
            }

            inline const ITObj& operator*() const
            {
                return *get();
            }

            void swap(const self_type& stIter) const // never throws
            {
                using std::swap;

                swap(m_pListPtr, stIter.m_pListPtr);
                swap(iIndex, stIter.iIndex);
            }

        };

        typedef Iterator<TObj> iterator;
        typedef const Iterator<TObj> const_iterator;

    private:
        /**
         * 数据交换函数优化(size_type)
         * @param [in] left
         * @param [in] right
         */
        inline void swap(size_type& left, size_type& right)
        {
            left ^= right ^= left ^= right;
        }

        struct IdxListHeader
        {
            size_type m_iLastUsedNode;
            size_type m_iSize;
        };

        IdxListHeader m_stHeader;
        node_type m_stData[MAX_SIZE + 1];

        size_type _create_node()
        {
            assert(m_stHeader.m_iLastUsedNode >= 0 && m_stHeader.m_iLastUsedNode <= MAX_SIZE);

            size_type iNewIdx = m_stData[m_stHeader.m_iLastUsedNode].iNextIdx;
            while (IsExists(iNewIdx))
            {
                iNewIdx = m_stData[iNewIdx].iNextIdx;
            }

            // 队列满，返回-1
            if (iNewIdx >= MAX_SIZE || IsExists(iNewIdx))
            {
                return -1;
            }

            // 关闭未初始化符号、移动最后节点、计数 +1
            m_stData[iNewIdx].bIsInited = true;
            m_stHeader.m_iLastUsedNode = iNewIdx;
            ++ m_stHeader.m_iSize;

            return iNewIdx;
        }


        /**
         * 析构函数使用
         */
        template<typename CObj>
        struct _destruct_obj
        {
            self_type& stSelf;

            _destruct_obj(self_type& self): stSelf(self){}

            void operator()(size_type idx, CObj& stObj)
            {
                stSelf.m_stData[idx].bIsInited = false;
                stObj.~TObj();
            }
        };

        /**
         * 条件计数函数为普通函数时使用
         */
        template<typename CObj>
        struct _count_cc_func
        {
            size_type& counter;
            bool (*fn)(size_type, CObj&);

            _count_cc_func(size_type& _counter, bool (*_fn)(size_type, CObj&)):
                counter(_counter), fn(_fn){}

            void operator()(size_type idx, CObj& stObj)
            {
                counter += (*fn)(idx, stObj)? 1: 0;
            }
        };

        /**
         * 条件计数函数为仿函数时使用
         */
        template<typename _F, typename CObj>
        struct _count_obj_func
        {
            size_type& counter;
            _F& fn;

            _count_obj_func(size_type& _counter, _F& _fn):
                counter(_counter), fn(_fn){}

            void operator()(size_type idx, CObj& stObj)
            {
                counter += fn(idx, stObj)? 1: 0;
            }
        };

    public:
        StaticIdxList()
        {
        }

        ~StaticIdxList()
        {
            destruct();
        }

        /**
         * 如果是第一次创建，必须调用次函数进行初始化
         */
        void construct()
        {
            m_stHeader.m_iSize = 0;
            m_stHeader.m_iLastUsedNode = MAX_SIZE; // 最后一个为头结点

            for (size_type i = 0; i <= MAX_SIZE; ++i)
            {
                m_stData[i].iPreIdx = (i - 1);
                m_stData[i].iNextIdx = (i + 1);
                m_stData[i].bIsInited = false;
            }
            m_stData[0].iPreIdx = MAX_SIZE;
            m_stData[MAX_SIZE].iNextIdx = 0;
            m_stData[MAX_SIZE].bIsInited = true;
        }

        /**
         * 调用所有对象的析构函数(可选)
         */
        void destruct()
        {
            // 删除所有未释放对象
            Foreach(_destruct_obj<TObj>(*this));
        }

        /**
         * 判断下标节点存在
         * @param [in] idx idx
         * @return 存在返回true，否则返回false
         */
        bool IsExists(size_type idx) const
        {
            // 超出最大个数限制
            if (idx >= MAX_SIZE)
            {
                return false;
            }

            // 小于0
            if (idx < 0)
            {
                return false;
            }

            // flag 检查
            return m_stData[idx].bIsInited;
        }

        /**
         * 获取下一个元素的下标
         * @param [in] idx 当前元素下标
         * @return 存在返回下一个元素下标，不存在返回-1
         */
        size_type GetNextIdx(size_type idx) const
        {
            size_type iRet = -1;

            if (IsExists(idx))
            {
                iRet = m_stData[idx].iNextIdx;
            }

            iRet = IsExists(iRet)? iRet: -1;

            return iRet;

        }

        /**
         * 获取上一个元素的下标
         * @param [in] idx 当前元素下标
         * @return 存在返回上一个元素下标，不存在返回-1
         */
        size_type GetPreIdx(size_type idx) const
        {
            size_type iRet = -1;

            if (IsExists(idx))
            {
                iRet = m_stData[idx].iPreIdx;
            }

            iRet = IsExists(iRet)? iRet: -1;

            return iRet;

        }

        /**
         * 按Idx获取节点
         * @param [in] idx
         * @return 存在则返回数据迭代器，不存在则返回end迭代器
         */
        inline iterator Get(size_type idx)
        {
            return get(idx);
        }

        /**
         * 按Idx获取节点(const)
         * @param [in] idx
         * @return 存在则返回数据迭代器，不存在则返回end迭代器
         */
        inline const_iterator Get(size_type idx) const
        {
            return get(idx);
        }

        /**
         * 按Idx获取节点(const)
         * @param [in] idx
         * @return 存在则返回数据引用
         */
        inline TObj& operator[](size_type idx) { return *get(idx); };

        /**
         * 按Idx获取节点(const)
         * @param [in] idx
         * @return 存在则返回数据常量引用
         */
        inline const TObj& operator[](size_type idx) const { return *get(idx); };

        /**
         * 创建节点，返回idx
         * @return 新节点的idx，失败返回-1
         */
        size_type Create()
        {
            size_type ret = _create_node();
            if (ret >= 0)
            {
                new (m_stData[ret].stObjBin)TObj();
            }

            return ret;
        }

        /**
         * 创建节点，返回idx
         * @param [in] param1 构造函数参数1
         * @return 新节点的idx，失败返回-1
         */
        template<typename _TP1>
        size_type Create(const _TP1& param1)
        {
            size_type ret = _create_node();
            if (ret >= 0)
            {
                new (m_stData[ret].stObjBin)TObj(param1);
            }

            return ret;
        }

        /**
         * 创建节点，返回idx
         * @param [in] param1 构造函数参数1
         * @param [in] param2 构造函数参数2
         * @return 新节点的idx，失败返回-1
         */
        template<typename _TP1, typename _TP2>
        size_type Create(const _TP1& param1, const _TP2& param2)
        {
            size_type ret = _create_node();
            if (ret >= 0)
            {
                new (m_stData[ret].stObjBin)TObj(param1, param2);
            }

            return ret;
        }

        /**
         * 创建节点，返回idx
         * @param [in] param1 构造函数参数1
         * @param [in] param2 构造函数参数2
         * @param [in] param3 构造函数参数3
         * @return 新节点的idx，失败返回-1
         */
        template<typename _TP1, typename _TP2, typename _TP3>
        size_type Create(const _TP1& param1, const _TP2& param2, const _TP3& param3)
        {
            size_type ret = _create_node();
            if (ret >= 0)
            {
                new (m_stData[ret].stObjBin)TObj(param1, param2, param3);
            }

            return ret;
        }

        /**
         * 移除一个元素
         * @param [in] idx 下标
         */
        void Remove(size_type idx)
        {
            using std::swap;

            // 不存在直接返回
            if(!IsExists(idx))
            {
                return;
            }

            size_type iPreIdx = m_stData[idx].iPreIdx;
            size_type iNextIdx = m_stData[idx].iNextIdx;
            size_type iFreeFirst = m_stData[m_stHeader.m_iLastUsedNode].iNextIdx;

            // 删除节点，交换链接
            swap(m_stData[iPreIdx].iNextIdx, m_stData[idx].iNextIdx);
            swap(m_stData[iNextIdx].iPreIdx, m_stData[idx].iPreIdx);

            // 防止自己和自己交换
            if (idx == m_stHeader.m_iLastUsedNode)
            {
                m_stHeader.m_iLastUsedNode = iPreIdx;
            }

            // 处理自由节点，交换链接
            swap(m_stData[m_stHeader.m_iLastUsedNode].iNextIdx, m_stData[idx].iNextIdx);
            swap(m_stData[iFreeFirst].iPreIdx, m_stData[idx].iPreIdx);

            // 节点行为
            m_stData[idx].bIsInited = false;
            m_stHeader.m_iLastUsedNode = iPreIdx;

            // 执行析构
            ((TObj*)m_stData[idx].stObjBin)->~TObj();
            // 计数减一
            -- m_stHeader.m_iSize;
        }

        /**
         * 是否为空
         * @return 为空返回true
         */
        inline bool IsEmpty() const { return empty(); }

        /**
         * 是否已满
         * @return 已满返回true
         */
        inline bool IsFull() const { return size() >= max_size(); }

        // ===============================
        // =====        迭代器 操作                  =====
        // ===============================
        iterator get(size_type idx)
        {
            if (!IsExists(idx))
            {
                return iterator(-1, this);
            }

            return iterator(idx, this);
        }

        const_iterator get(size_type idx) const
        {
            if (!IsExists(idx))
            {
                return const_iterator(-1, this);
            }

            return const_iterator(idx, this);
        }

        iterator begin()
        {
            return get(m_stData[MAX_SIZE].iNextIdx);
        }

        const_iterator begin() const
        {
            return get(m_stData[MAX_SIZE].iNextIdx);
        }

        iterator end()
        {
            return get(-1);
        }

        const_iterator end() const
        {
            return get(-1);
        }

        size_t size() const
        {
            return static_cast<size_t>(Count());
        }

        size_t max_size() const
        {
            return MAX_SIZE;
        }

        inline bool empty() const
        {
            return size() == 0;
        }

        // 高级功能
        // ===============================
        // =====       Lambda 操作             =====
        // ===============================


        /**
         * foreach 操作
         * @param fn 执行仿函数，参数必须为 (size_type, TObj&)
         */
        template<typename _F>
        void Foreach(_F fn)
        {
            size_type iIter = m_stData[MAX_SIZE].iNextIdx;
            while (IsExists(iIter))
            {
                fn(iIter, *reinterpret_cast<TObj*>(m_stData[iIter].stObjBin));
                iIter = m_stData[iIter].iNextIdx;
            }
        }

        /**
         * const foreach 操作
         * @param fn 执行仿函数，参数必须为 (size_type, TObj&)
         */
        template<typename _F>
        void Foreach(_F fn) const
        {
            size_type iIter = m_stData[MAX_SIZE].iNextIdx;
            while (IsExists(iIter))
            {
                fn(iIter, *reinterpret_cast<const TObj*>(m_stData[iIter].stObjBin));
                iIter = m_stData[iIter].iNextIdx;
            }
        }

        /**
         * foreach 操作
         * @param fn 执行函数，参数必须为 (size_type, TObj&)
         */
        template<typename _R>
        void Foreach(_R (*fn)(size_type, TObj&))
        {
            size_type iIter = m_stData[MAX_SIZE].iNextIdx;
            while (IsExists(iIter))
            {
                (*fn)(iIter, *reinterpret_cast<TObj*>(m_stData[iIter].stObjBin));
                iIter = m_stData[iIter].iNextIdx;
            }
        };

        /**
         * const foreach 操作
         * @param fn 执行函数，参数必须为 (size_type, TObj&)
         */
        template<typename _R>
        void Foreach(_R (*fn)(size_type, const TObj&)) const
        {
            size_type iIter = m_stData[MAX_SIZE].iNextIdx;
            while (IsExists(iIter))
            {
                (*fn)(iIter, *reinterpret_cast<const TObj*>(m_stData[iIter].stObjBin));
                iIter = m_stData[iIter].iNextIdx;
            }
        };

        /**
         * 获取元素个数
         * @return 元素个数
         */
        size_type Count() const { return m_stHeader.m_iSize; };

    public:
        /**
         * 获取符合条件的元素个数
         * @param [in] fn 条件函数
         * @return 符合条件的元素个数
         */
        size_type Count(bool (*fn)(size_type, const TObj&)) const
        {
            size_type iRet = 0;

            Foreach(_count_cc_func<const TObj>(iRet, fn));

            return iRet;
        };

        /**
         * 获取符合条件的元素个数
         * @param [in] fn 条件函数
         * @return 符合条件的元素个数
         */
        size_type Count(bool (*fn)(size_type, TObj&))
        {
            size_type iRet = 0;

            Foreach(_count_cc_func<TObj>(iRet, fn));

            return iRet;
        };

    public:
        /**
         * 获取符合条件的元素个数
         * @param [in] fn 条件仿函数
         * @return 符合条件的元素个数
         */
        template<typename _F>
        size_type Count(_F fn) const
        {
            size_type iRet = 0;

            Foreach(_count_obj_func<_F, const TObj>(iRet, fn));

            return iRet;
        };

        /**
         * 获取符合条件的元素个数
         * @param [in] fn 条件仿函数
         * @return 符合条件的元素个数
         */
        template<typename _F>
        size_type Count(_F fn)
        {
            size_type iRet = 0;

            Foreach(_count_obj_func<_F, TObj>(iRet, fn));

            return iRet;
        };

    };
}


#endif /* IDXLIST_H_ */

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
