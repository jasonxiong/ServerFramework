/**
 * Copyright (c) 2013, Tencent
 * All rights reserved.
 *
 * @file AttributeManager.h
 * @brief 通用属性系统<br />
 *        支持自动处理依赖关系公式
 *
 * @version 1.0
 * @author owentou, owentou@tencent.com
 * @date 2013-04-19
 *
 * @history
 *
 */

#ifndef _BASELIB_HASH_H_
#define _BASELIB_HASH_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <map>
#include <set>
#include <list>
#include <vector>
#include <algorithm>
#include "std/smart_ptr.h"
#include "std/ref.h"

namespace BaseLib
{
    namespace Operator
    {
        /**
         * 属性关系表达式--基类
         */
        template<typename _TAttrMgr>
        struct AttrOprBase: public std::enable_shared_from_this< AttrOprBase<_TAttrMgr> >
        {
            typedef typename _TAttrMgr::attr_value_type attr_value_type;
            typedef typename _TAttrMgr::attr_class_type attr_class_type;
            typedef typename _TAttrMgr::attr_attach_set_type attr_attach_set_type;

            typedef AttrOprBase<_TAttrMgr> base_type;
            typedef std::shared_ptr<base_type> base_ptr_type;

            virtual ~AttrOprBase(){}
            virtual attr_value_type operator()(_TAttrMgr&) = 0;

            virtual void BuildFormularParam(attr_attach_set_type&) {}
        };

        /**
         * 属性关系表达式数--值类型
         */
        template<typename _TAttrMgr>
        struct AttrOprVal : public AttrOprBase<_TAttrMgr>
        {
            typedef AttrOprVal<_TAttrMgr> self_type;
            typedef AttrOprBase<_TAttrMgr> base_type;
            typedef std::shared_ptr<base_type> base_ptr_type;
            typedef typename base_type::attr_value_type attr_value_type;
            typedef typename base_type::attr_class_type attr_class_type;

            attr_value_type tVal;
            AttrOprVal(attr_value_type val): tVal(val){}

            virtual attr_value_type operator()(_TAttrMgr&)
            {
                return tVal;
            }

            static base_ptr_type Create(attr_value_type v)
            {
                return std::shared_ptr<base_type>(new self_type(v));
            }
        };

        /**
         * 属性关系表达式--属性类型
         */
        template<typename _TAttrMgr>
        struct AttrOprAttr : public AttrOprBase<_TAttrMgr>
        {
            typedef AttrOprAttr<_TAttrMgr> self_type;
            typedef AttrOprBase<_TAttrMgr> base_type;
            typedef std::shared_ptr<base_type> base_ptr_type;
            typedef typename base_type::attr_value_type attr_value_type;
            typedef typename base_type::attr_class_type attr_class_type;
            typedef typename _TAttrMgr::attr_attach_set_type attr_attach_set_type;

            attr_class_type tAttrType;
            AttrOprAttr(attr_class_type val): tAttrType(val){}

            virtual attr_value_type operator()(_TAttrMgr& stMgr)
            {
                return stMgr[tAttrType];
            }

            static base_ptr_type Create(attr_class_type v)
            {
                return std::shared_ptr<base_type>(new self_type(v));
            }

            virtual void BuildFormularParam(attr_attach_set_type& stAttachSet) 
            {
                stAttachSet.insert(tAttrType);
            }
        };

        /**
         * 属性关系表达式--双目运算符基类
         */
        template<typename _TAttrMgr, typename _TReal>
        struct AttrOprBinaryOperation: public AttrOprBase<_TAttrMgr>
        {
            typedef AttrOprBase<_TAttrMgr> base_type;
            typedef std::shared_ptr<base_type> base_ptr_type;
            typedef _TReal self_type;
            typedef typename _TAttrMgr::attr_attach_set_type attr_attach_set_type;

            base_ptr_type left, right;

            AttrOprBinaryOperation(base_ptr_type l, base_ptr_type r): left(l), right(r){}

            static base_ptr_type Create(base_ptr_type left, base_ptr_type right)
            {
                return std::shared_ptr<base_type>(new self_type(left, right));
            }

            virtual void BuildFormularParam(attr_attach_set_type& stAttachSet) 
            {
                left->BuildFormularParam(stAttachSet);
                right->BuildFormularParam(stAttachSet);
            }
        };

        /**
         * 属性关系表达式--加法类型
         */
        template<typename _TAttrMgr>
        struct AttrOprPlus : public AttrOprBinaryOperation<_TAttrMgr, AttrOprPlus<_TAttrMgr> >
        {
            typedef AttrOprPlus<_TAttrMgr> self_type;
            typedef AttrOprBinaryOperation<_TAttrMgr, self_type > opr_base_type;
            typedef AttrOprBase<_TAttrMgr> base_type;
            typedef std::shared_ptr<base_type> base_ptr_type;
            typedef typename base_type::attr_value_type attr_value_type;

            using opr_base_type::left;
            using opr_base_type::right;

            AttrOprPlus(base_ptr_type l, base_ptr_type r): opr_base_type(l, r){}

            virtual attr_value_type operator()(_TAttrMgr& stMgr)
            {
                return (*left)(stMgr) + (*right)(stMgr);
            }
        };

        /**
         * 属性关系表达式--减法类型
         */
        template<typename _TAttrMgr>
        struct AttrOprMinu : public AttrOprBinaryOperation<_TAttrMgr, AttrOprMinu<_TAttrMgr> >
        {
            typedef AttrOprMinu<_TAttrMgr> self_type;
            typedef AttrOprBinaryOperation<_TAttrMgr, self_type > opr_base_type;
            typedef AttrOprBase<_TAttrMgr> base_type;
            typedef std::shared_ptr<base_type> base_ptr_type;
            typedef typename base_type::attr_value_type attr_value_type;

            using opr_base_type::left;
            using opr_base_type::right;

            AttrOprMinu(base_ptr_type l, base_ptr_type r): opr_base_type(l, r){}

            virtual attr_value_type operator()(_TAttrMgr& stMgr)
            {
                return (*left)(stMgr) - (*right)(stMgr);
            }
        };

        /**
         * 属性关系表达式--乘法类型
         */
        template<typename _TAttrMgr>
        struct AttrOprMult : public AttrOprBinaryOperation<_TAttrMgr, AttrOprMult<_TAttrMgr> >
        {
            typedef AttrOprMult<_TAttrMgr> self_type;
            typedef AttrOprBinaryOperation<_TAttrMgr, self_type > opr_base_type;
            typedef AttrOprBase<_TAttrMgr> base_type;
            typedef std::shared_ptr<base_type> base_ptr_type;
            typedef typename base_type::attr_value_type attr_value_type;

            using opr_base_type::left;
            using opr_base_type::right;

            AttrOprMult(base_ptr_type l, base_ptr_type r): opr_base_type(l, r){}

            virtual attr_value_type operator()(_TAttrMgr& stMgr)
            {
                return (*left)(stMgr) * (*right)(stMgr);
            }

        };

        /**
         * 属性关系表达式--除法类型
         */
        template<typename _TAttrMgr>
        struct AttrOprDevi : public AttrOprBinaryOperation<_TAttrMgr, AttrOprDevi<_TAttrMgr> >
        {
            typedef AttrOprDevi<_TAttrMgr> self_type;
            typedef AttrOprBinaryOperation<_TAttrMgr, self_type > opr_base_type;
            typedef AttrOprBase<_TAttrMgr> base_type;
            typedef std::shared_ptr<base_type> base_ptr_type;
            typedef typename base_type::attr_value_type attr_value_type;

            using opr_base_type::left;
            using opr_base_type::right;

            AttrOprDevi(base_ptr_type l, base_ptr_type r): opr_base_type(l, r){}

            virtual attr_value_type operator()(_TAttrMgr& stMgr)
            {
                return (*left)(stMgr) / (*right)(stMgr);
            }
        };

        template<typename _AttrMgr>
        typename AttrOprAttr<_AttrMgr>::base_ptr_type _(typename AttrOprAttr<_AttrMgr>::attr_class_type type)
        {
            return AttrOprAttr<_AttrMgr>::Create(type);
        }
    }

    namespace Wrapper
    {
        template<typename _TAttrMgr>
        class AttributeFormulaBuilderAttrWrapper
        {
        public:
            typedef typename _TAttrMgr::attr_class_type attr_class_type;
            typedef typename _TAttrMgr::formula_map_type formula_map_type;
            typedef typename _TAttrMgr::attr_formula_ptr_type attr_formula_ptr_type;
            typedef AttributeFormulaBuilderAttrWrapper<_TAttrMgr> self_type;

        private:
            attr_class_type m_tClass;

        public:
            AttributeFormulaBuilderAttrWrapper(attr_class_type tClass): m_tClass(tClass){}

            attr_formula_ptr_type operator()() const
            {
                return Operator::_<_TAttrMgr>(m_tClass);
            }

            /**
             * 提供自动转为属性参数的功能
             */
            operator attr_formula_ptr_type() const
            {
                return (*this)(m_tClass);
            }

            /**
             * 公式赋值控制
             */
            self_type& operator=(attr_formula_ptr_type pFormula)
            {
                formula_map_type& stFormulaMap = _TAttrMgr::GetFormulaMapObj();
                stFormulaMap[m_tClass] = pFormula;
                return *this;
            }

            /**
             * 复制赋值控制(支持连等号)
             */
            self_type& operator=(const self_type& stFormulaWrapper)
            {
                formula_map_type& stFormulaMap = _TAttrMgr::GetFormulaMapObj();
                typename formula_map_type::iterator iter = stFormulaMap.find(stFormulaWrapper.m_tClass);

                // [优化] 如果目标属性由公式生成，则减少一层节点
                if (iter == stFormulaMap.end())
                {
                    stFormulaMap[m_tClass] = Operator::_<_TAttrMgr>(stFormulaWrapper.m_tClass);
                }
                else
                {
                    stFormulaMap[m_tClass] = iter->second;
                }

                return *this;
            }
        };


        template<typename _TAttrMgr>
        class AttributeFormulaBuilderWrapper
        {
        public:
            typedef typename _TAttrMgr::attr_class_type attr_class_type;
            typedef typename _TAttrMgr::attr_value_type attr_value_type;
            typedef AttributeFormulaBuilderAttrWrapper<_TAttrMgr> builder_attr_type;

        public:

            builder_attr_type operator[](attr_class_type tClass)
            {
                return builder_attr_type(tClass);
            }

            builder_attr_type operator()(attr_class_type tClass)
            {
                return builder_attr_type(tClass);
            }
        };

        /**
         * 属性值包装器
         */
        template<typename _TAttrMgr>
        class AttributeWrapper
        {
        public:
            typedef typename _TAttrMgr::attr_class_type attr_class_type;
            typedef typename _TAttrMgr::attr_value_type attr_value_type;
            typedef typename _TAttrMgr::attr_attach_set_type attr_attach_set_type;
            typedef typename _TAttrMgr::attr_attach_list_type attr_attach_list_type;
            typedef typename _TAttrMgr::formula_map_type formula_map_type;
            typedef typename _TAttrMgr::attr_attach_type attr_attach_type;

            typedef AttributeWrapper<_TAttrMgr> self_type;

        private:
            std::reference_wrapper<_TAttrMgr> m_stMgrRef;
            attr_class_type m_tIndex;

        public:
            AttributeWrapper(_TAttrMgr& stMgr, attr_class_type tIndex): m_stMgrRef(stMgr), m_tIndex(tIndex){}

            operator attr_value_type() const
            {
                return m_stMgrRef.get().get(m_tIndex);
            }

            /**
             * 值发生变化
             * @note 同步改变关联项
             * @param [in] tVal 改变的属性项
             * @return 依据等号规范, 返回*this
             */
            self_type& operator=(attr_value_type tVal)
            {
                m_stMgrRef.get().get(m_tIndex) = tVal;

                attr_attach_list_type stAttachedList;
                GetAttachedAttributes(stAttachedList, false);

                formula_map_type& stFormulaMap = m_stMgrRef.get().GetFormulaMap();
                
                typename attr_attach_list_type::iterator iter = stAttachedList.begin();
                for(; iter != stAttachedList.end(); ++ iter)
                {
                    typename formula_map_type::iterator itAttached = stFormulaMap.find(*iter);
                    if (itAttached == stFormulaMap.end() || *iter == m_tIndex)
                    {
                        // 这里有问题，那就是程序有BUG
                        // 或者依赖关系成环
                        // 或者出现自依赖
                        continue;
                    }

                    // 循环赋值
                    m_stMgrRef.get()[*iter] = (*itAttached->second)(m_stMgrRef);
                }

                return (*this);
            }

            self_type& operator=(const self_type& tVal)
            {
                m_stMgrRef = std::ref(tVal.m_stMgrRef);
                m_tIndex = tVal.m_tIndex;

                return (*this);
            }

            /**
             * 获取所有被当前属性依赖的属性<br />
             * 即，当前属性是获得的属性的参数
             * @param [in, out] stAttrList 结果属性集
             * @param [in] bRecursion 是否递归查找依赖项
             */
            void GetAttachedAttributes(attr_attach_list_type& stAttrList, bool bRecursion = false)
            {
                _TAttrMgr& stMgr = m_stMgrRef;
                attr_attach_type& stAttachMap = stMgr.GetAttachAttrMap();
                typename attr_attach_type::iterator iter = stAttachMap.find(m_tIndex);

                // 没找到关系式
                if (iter == stAttachMap.end())
                {
                    return;
                }

                // 填充数据
                stAttrList.insert(stAttrList.end(), iter->second.begin(), iter->second.end());
                if ( false == bRecursion || false == _TAttrMgr::CheckValid())
                {
                    return;
                }

                // 需要递归查找
                typedef typename attr_attach_type::mapped_type attach_mapped;
                typename attach_mapped::iterator itm = iter->second.begin();
                for (; itm != iter->second.end(); ++ itm)
                {
                    self_type stTmpObj(stMgr, *itm);
                    stTmpObj.GetAttachedAttributes(stAttrList, bRecursion);
                }
            }

            /**
             * 获取所有当前属性的计算参数属性<br />
             * 即，获得的属性是的当前属性参数
             * @param [in, out] stAttrSet 结果属性集
             * @param [in] bRecursion 是否递归查找参数
             */
            void GetAttachAttributes(attr_attach_set_type& stAttrSet, bool bRecursion = false)
            {
                _TAttrMgr& stMgr = m_stMgrRef;
                formula_map_type& stFormulaMap = stMgr.GetFormulaMap();
                typename formula_map_type::iterator iter = stFormulaMap.find(m_tIndex);

                // 没找到公式
                if (iter == stFormulaMap.end())
                {
                    return;
                }

                // 填充数据
                attr_attach_set_type stCurrentSet;
                iter->second->BuildFormularParam(stCurrentSet);
                stAttrSet.insert(stCurrentSet.begin(), stCurrentSet.end());

                if ( false == bRecursion || false == _TAttrMgr::CheckValid())
                {
                    return;
                }

                // 需要递归查找
                typename attr_attach_set_type::iterator itm = stCurrentSet.begin();
                for (; itm != stCurrentSet.end(); ++ itm)
                {
                    self_type stTmpObj(stMgr, *itm);
                    stTmpObj.GetAttachAttributes(stAttrSet, bRecursion);
                }
            }
        };
    }

    /**
     * 属性管理器
     * @note 应该包含至少一条属性公式
     * @note 附属类_TOwner必须包含GenAttrFormulaMap(formula_builder_type&)函数以初始化属性公式表
     * @note 为了提高查询效率, 属性编号是连续的, 故而_MAXCOUNT 不宜过大
     */
    template<typename _TAttrType, int _MAXCOUNT, typename _TOwner, typename _TAttr = int>
    class AttributeManager
    {
    public:
        typedef _TAttrType attr_class_type;
        typedef _TAttr attr_value_type;
        typedef AttributeManager<_TAttrType, _MAXCOUNT, _TOwner, _TAttr> self_type;

        typedef std::set<_TAttrType> attr_attach_set_type;
        typedef std::list<_TAttrType> attr_attach_list_type;
        typedef std::map<_TAttrType, attr_attach_list_type > attr_attach_type;

        typedef Operator::AttrOprBase<self_type> attr_formula_type;
        typedef typename attr_formula_type::base_ptr_type attr_formula_ptr_type;
        typedef std::map<_TAttrType, attr_formula_ptr_type > formula_map_type;
        typedef Wrapper::AttributeFormulaBuilderWrapper<self_type> formula_builder_type;

        typedef Wrapper::AttributeWrapper<self_type> attr_wrapper_type;

        typedef std::list< std::list<int> > formula_loops_type;

        friend class Wrapper::AttributeWrapper<self_type>;
        friend class Wrapper::AttributeFormulaBuilderWrapper<self_type>;
        friend class Wrapper::AttributeFormulaBuilderAttrWrapper<self_type>;

    private:
        _TAttr m_arrAttrs[_MAXCOUNT];

        static attr_attach_type& GetAttachAttrMapObj()
        {
            static attr_attach_type functor;
            return functor;
        }

        static formula_map_type& GetFormulaMapObj()
        {
            static formula_map_type functor;
            return functor;
        }

        static attr_attach_type& GetAttachAttrMap()
        {
            // 保证已初始化
            GetFormulaMap();

            return GetAttachAttrMapObj();
        }

        static formula_map_type& GetFormulaMap()
        {
            formula_map_type& stFormula = GetFormulaMapObj();
            if (stFormula.begin() == stFormula.end())
            {
                // 生成属性公式
                formula_builder_type stBuilder;
                _TOwner::GenAttrFormulaMap(stBuilder);

                // 建立属性关系邻接表
                attr_attach_type& stAttrAttach = GetAttachAttrMapObj();
                stAttrAttach.clear();
                for(typename formula_map_type::iterator iter = stFormula.begin();
                    iter != stFormula.end();
                    ++ iter)
                {
                    attr_attach_set_type stAttachAttrs;

                    iter->second->BuildFormularParam(stAttachAttrs);
                    for (typename attr_attach_set_type::iterator itAttr = stAttachAttrs.begin();
                        itAttr != stAttachAttrs.end();
                        ++ itAttr
                        )
                    {
                        typename attr_attach_type::mapped_type& stList = stAttrAttach[*itAttr];

                        stList.push_back(iter->first);
                    }
                }
            }

            return stFormula;
        }

    public:

        void Construct()
        {
            memset(m_arrAttrs, 0, sizeof(m_arrAttrs));
        }

        /**
         * 获取属性原始引用对象
         * @param [in] uIndex 对象索引
         * @return 原始属性引用
         */
        _TAttr& get(_TAttrType uIndex)
        {
            return m_arrAttrs[uIndex];
        }

        /**
         * 获取属性原始引用对象
         * @param [in] uIndex 对象索引
         * @return 原始属性引用
         */
        const _TAttr& get(_TAttrType uIndex) const
        {
            return m_arrAttrs[uIndex];
        }

        /**
         * 获取属性
         * @param [in] uIndex 对象索引
         * @return 属性对象
         */
        attr_wrapper_type operator[](_TAttrType uIndex)
        {
            return attr_wrapper_type(*this, uIndex);
        }


    private:
        struct CheckValidNode
        {
            int iLeftDstEdge; /** 剩余到达当前节点的边数 **/
            std::list<int> stSrcList; /** 以当前节点为起点的边列表(邻接表) **/
            CheckValidNode(): iLeftDstEdge(0){}
        };

        /**
         * 检查公式是否成环
         * @note 检测有向图中是否存在环
         * @note 主要算法：拓扑排序, 完整时间复杂度[O(n+2m)], 完整空间复杂度[O(2n+m)]
         * @note { n为所有公式中涉及的属性ID总个数, m为属性的会直接影响的目标属性总个数 }
         * @param [in, out] stRelationMap 关系节点缓冲区(如果存在环, 输出结果中的边列表包含所有有连接的边和节点)
         * @return 不存在环返回true
         */
        static bool CheckFormulaNoLoop(CheckValidNode stRelationMap[_MAXCOUNT])
        {
            std::list<int> stWaitForReach;

            // Step 1. 建图, 时间复杂度[O(m)], 空间复杂度[O(n + m)]
            attr_attach_type& stAttachedMap = GetAttachAttrMap();
            for(typename attr_attach_type::iterator iter = stAttachedMap.begin();
                iter != stAttachedMap.end();
                ++ iter)
            {
                    attr_attach_list_type& stAttachList = iter->second;
                    int iSrcIndex = static_cast<int>(iter->first);

                    for (typename attr_attach_list_type::iterator itAttr = stAttachList.begin();
                        itAttr != stAttachList.end();
                        ++ itAttr
                        )
                    {
                        int iDstIndex = static_cast<int>(*itAttr);
                        ++ stRelationMap[iDstIndex].iLeftDstEdge;
                        stRelationMap[iSrcIndex].stSrcList.push_back(iDstIndex);
                    }
            }

            // Step 2. 初始化拓扑排序节点, 可到达节点推入就绪链表, 时间复杂度[O(n)], 空间复杂度[O(n)]
            int iLeftUnreached = _MAXCOUNT;
            for (int i = 0; i < _MAXCOUNT; ++ i)
            {
                if (stRelationMap[i].iLeftDstEdge <= 0)
                {
                    stWaitForReach.push_back(i);
                }
            }

            // Step 3. 拓扑排序, 时间复杂度[O(m)], 空间复杂度[O(n)][与上面共用就绪链表]
            while(false == stWaitForReach.empty())
            {
                -- iLeftUnreached;
                int iSrcIndex = stWaitForReach.front();
                stWaitForReach.pop_front();

                std::list<int>& stEdgeList =  stRelationMap[iSrcIndex].stSrcList;
                while(!stEdgeList.empty())
                {
                    int iDstIndex = stEdgeList.front();
                    stEdgeList.pop_front();

                    -- stRelationMap[iDstIndex].iLeftDstEdge;
                    if (stRelationMap[iDstIndex].iLeftDstEdge == 0)
                    {
                        stWaitForReach.push_back(iDstIndex);
                    }
                }
            }

            // Step 4. 如果所有节点均已到达，则无环
            return iLeftUnreached <= 0;
        }

        // 获取循环关系链--深度优先搜索函数
        static void GetInvalidLoopsDFS(CheckValidNode stRelationMap[_MAXCOUNT], int iPos, std::vector<bool>& stReachedFlags, std::vector<bool>& stIsInRoute, std::vector<int>& stRoute, formula_loops_type& stResult)
        {
            // 如果已经算过经过当前节点的环，无需重算
            if(stReachedFlags[iPos])
            {
                return;
            }

            // 出现循环节
            if (stIsInRoute[iPos])
            {
                // 新增环记录
                stResult.push_back(std::list<int>());
                stResult.back().assign(
                    std::find(stRoute.begin(), stRoute.end(), iPos),
                    stRoute.end()
                );

                return;
            }

            stRoute.push_back(iPos); // 入栈
            stIsInRoute[iPos] = true;
            for (std::list<int>::iterator iter = stRelationMap[iPos].stSrcList.begin();
                iter != stRelationMap[iPos].stSrcList.end();
                ++ iter)
            {
                GetInvalidLoopsDFS(stRelationMap, *iter, stReachedFlags, stIsInRoute, stRoute, stResult);
            }
            stIsInRoute[iPos] = false;
            stRoute.pop_back(); // 出栈

            // 标记为已经经过当前节点
            stReachedFlags[iPos] = true;
        }

    public:
        /**
         * 检查关系链是否合法(不能出现 环或图 关系)
         * @return 关系链合法返回true
         */
        static bool CheckValid()
        {
            // 检查公式正确的缓存
            static bool bCheckRes = false;
            if (bCheckRes)
            {
                return bCheckRes;
            }

            // 检测有向图中是否存在环
            CheckValidNode stRelationMap[_MAXCOUNT];
            return bCheckRes = CheckFormulaNoLoop(stRelationMap);
        }

        /**
         * 获取不合法的关系链
         * @note 主要算法：DFS { 时间复杂度O(mn), 空间复杂度O(3n) }
         * @param [in, out] stRelationMap 关系节点缓冲区
         * @param [out] stLoops 所有的环列表
         */
        static void GetInvalidLoops(CheckValidNode stRelationMap[_MAXCOUNT], formula_loops_type& stLoops)
        {
            stLoops.clear();
            bool bCheckRes = CheckFormulaNoLoop(stRelationMap);
            if (bCheckRes)
            {
                return;
            }

            int iStartIndex = 0;
            std::vector<bool> stReachedFlags, stIsInRoute;
            stReachedFlags.assign(_MAXCOUNT, false);
            stIsInRoute.assign(_MAXCOUNT, false);
            std::vector<int> stRoute;

            // 枚举起点
            for (; iStartIndex < _MAXCOUNT; ++ iStartIndex )
            {
                // 已被到达过，跳过
                if (stReachedFlags[iStartIndex])
                {
                    continue;
                }

                // 无后续节点，跳过
                if (stRelationMap[iStartIndex].stSrcList.size() <= 0)
                {
                    continue;
                }

                // 深度优先搜索，查找所有的环
                stRoute.clear();
                GetInvalidLoopsDFS(stRelationMap, iStartIndex, stReachedFlags, stIsInRoute, stRoute, stLoops);
            }

            
        }

        /**
         * 输出不合法的关系链和公式循环到输出流
         * @param [in, out] ostream 输出目标
         */
        template<typename _TOStream>
        static void PrintInvalidLoops(_TOStream& ostream)
        {
            formula_loops_type stLoops;
            CheckValidNode stRelationMap[_MAXCOUNT];
            GetInvalidLoops(stRelationMap, stLoops);
            ostream<< "All node links:\n";

            // 打印关系图谱
            for (int i = 0; i < _MAXCOUNT; ++ i)
            {
                if (stRelationMap[i].iLeftDstEdge <= 0)
                {
                    continue;
                }

                ostream<< "\t"<< i<< " -> ";
                for (std::list<int>::iterator iter = stRelationMap[i].stSrcList.begin();
                    iter != stRelationMap[i].stSrcList.end();
                    ++ iter)
                {
                    ostream<< " "<< *iter;
                }
                ostream<< "\n";
            }

            // 打印所有的公式依赖循环
            ostream<< "All formula loops:\n";
            for (typename formula_loops_type::iterator iter = stLoops.begin();
                iter != stLoops.end();
                ++ iter)
            {
                // 略过空列表
                if (iter->empty())
                {
                    continue;
                }

                ostream<< "\t"<< *(iter->begin());
                for (std::list<int>::reverse_iterator riter = iter->rbegin();
                    riter != iter->rend();
                    ++ riter)
                {
                    ostream<<" -> "<< *riter;
                }
                ostream<< "\n";
            }
        }

    };


    // ==================== 操作符重载 ====================
    namespace Operator
    {
        // ==================== 操作符重载: 表达式 -- 表达式 ====================
        
        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator+(std::shared_ptr<AttrOprBase<_TAttrMgr> > l, std::shared_ptr<AttrOprBase<_TAttrMgr> > r)
        {
            return AttrOprPlus<_TAttrMgr>::Create(l, r);
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator-(std::shared_ptr<AttrOprBase<_TAttrMgr> > l, std::shared_ptr<AttrOprBase<_TAttrMgr> > r)
        {
            return AttrOprMinu<_TAttrMgr>::Create(l, r);
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator*(std::shared_ptr<AttrOprBase<_TAttrMgr> > l, std::shared_ptr<AttrOprBase<_TAttrMgr> > r)
        {
            return AttrOprMult<_TAttrMgr>::Create(l, r);
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator/(std::shared_ptr<AttrOprBase<_TAttrMgr> > l, std::shared_ptr<AttrOprBase<_TAttrMgr> > r)
        {
            return AttrOprDevi<_TAttrMgr>::Create(l, r);
        }

        // ==================== 操作符重载: 表达式 -- 常量 ====================
        
        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator+(std::shared_ptr<AttrOprBase<_TAttrMgr> > l, typename _TAttrMgr::attr_value_type r)
        {
            return AttrOprPlus<_TAttrMgr>::Create(l, AttrOprVal<_TAttrMgr>::Create(r));
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator-(std::shared_ptr<AttrOprBase<_TAttrMgr> > l, typename _TAttrMgr::attr_value_type r)
        {
            return AttrOprMinu<_TAttrMgr>::Create(l, AttrOprVal<_TAttrMgr>::Create(r));
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator*(std::shared_ptr<AttrOprBase<_TAttrMgr> > l, typename _TAttrMgr::attr_value_type r)
        {
            return AttrOprMult<_TAttrMgr>::Create(l, AttrOprVal<_TAttrMgr>::Create(r));
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator/(std::shared_ptr<AttrOprBase<_TAttrMgr> > l, typename _TAttrMgr::attr_value_type r)
        {
            return AttrOprDevi<_TAttrMgr>::Create(l, AttrOprVal<_TAttrMgr>::Create(r));
        }

        // ==================== 操作符重载: 常量 -- 表达式 ====================
        
        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator+(typename _TAttrMgr::attr_value_type l, std::shared_ptr<AttrOprBase<_TAttrMgr> > r)
        {
            return AttrOprPlus<_TAttrMgr>::Create(AttrOprVal<_TAttrMgr>::Create(l), r);
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator-(typename _TAttrMgr::attr_value_type l, std::shared_ptr<AttrOprBase<_TAttrMgr> > r)
        {
            return AttrOprMinu<_TAttrMgr>::Create(AttrOprVal<_TAttrMgr>::Create(l), r);
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator*(typename _TAttrMgr::attr_value_type l, std::shared_ptr<AttrOprBase<_TAttrMgr> > r)
        {
            return AttrOprMult<_TAttrMgr>::Create(AttrOprVal<_TAttrMgr>::Create(l), r);
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator/(typename _TAttrMgr::attr_value_type l, std::shared_ptr<AttrOprBase<_TAttrMgr> > r)
        {
            return AttrOprDevi<_TAttrMgr>::Create(AttrOprVal<_TAttrMgr>::Create(l), r);
        }

        // ==================== 操作符重载: 属性 -- 属性 ====================
        
        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator+(const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& l, const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& r)
        {
            return AttrOprPlus<_TAttrMgr>::Create(l(), r());
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator-(const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& l, const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& r)
        {
            return AttrOprMinu<_TAttrMgr>::Create(l(), r());
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator*(const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& l, const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& r)
        {
            return AttrOprMult<_TAttrMgr>::Create(l(), r());
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator/(const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& l, const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& r)
        {
            return AttrOprDevi<_TAttrMgr>::Create(l(), r());
        }

        // ==================== 操作符重载: 属性 -- 常量 ====================
        
        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator+(const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& l, typename _TAttrMgr::attr_value_type r)
        {
            return AttrOprPlus<_TAttrMgr>::Create(l(), AttrOprVal<_TAttrMgr>::Create(r));
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator-(const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& l, typename _TAttrMgr::attr_value_type r)
        {
            return AttrOprMinu<_TAttrMgr>::Create(l(), AttrOprVal<_TAttrMgr>::Create(r));
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator*(const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& l, typename _TAttrMgr::attr_value_type r)
        {
            return AttrOprMult<_TAttrMgr>::Create(l(), AttrOprVal<_TAttrMgr>::Create(r));
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator/(const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& l, typename _TAttrMgr::attr_value_type r)
        {
            return AttrOprDevi<_TAttrMgr>::Create(l(), AttrOprVal<_TAttrMgr>::Create(r));
        }

        // ==================== 操作符重载: 常量 -- 属性 ====================
        
        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator+(typename _TAttrMgr::attr_value_type l, const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& r)
        {
            return AttrOprPlus<_TAttrMgr>::Create(AttrOprVal<_TAttrMgr>::Create(l), r());
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator-(typename _TAttrMgr::attr_value_type l, const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& r)
        {
            return AttrOprMinu<_TAttrMgr>::Create(AttrOprVal<_TAttrMgr>::Create(l), r());
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator*(typename _TAttrMgr::attr_value_type l, const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& r)
        {
            return AttrOprMult<_TAttrMgr>::Create(AttrOprVal<_TAttrMgr>::Create(l), r());
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator/(typename _TAttrMgr::attr_value_type l, const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& r)
        {
            return AttrOprDevi<_TAttrMgr>::Create(AttrOprVal<_TAttrMgr>::Create(l), r());
        }

        // ==================== 操作符重载: 属性 -- 表达式 ====================
        
        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator+(const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& l, std::shared_ptr<AttrOprBase<_TAttrMgr> > r)
        {
            return AttrOprPlus<_TAttrMgr>::Create(l(), r);
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator-(const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& l, std::shared_ptr<AttrOprBase<_TAttrMgr> > r)
        {
            return AttrOprMinu<_TAttrMgr>::Create(l(), r);
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator*(const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& l, std::shared_ptr<AttrOprBase<_TAttrMgr> > r)
        {
            return AttrOprMult<_TAttrMgr>::Create(l(), r);
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator/(const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& l, std::shared_ptr<AttrOprBase<_TAttrMgr> > r)
        {
            return AttrOprDevi<_TAttrMgr>::Create(l(), r);
        }

        // ==================== 操作符重载: 表达式 -- 属性 ====================
        
        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator+(std::shared_ptr<AttrOprBase<_TAttrMgr> > l, const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& r)
        {
            return AttrOprPlus<_TAttrMgr>::Create(l, r());
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator-(std::shared_ptr<AttrOprBase<_TAttrMgr> > l, const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& r)
        {
            return AttrOprMinu<_TAttrMgr>::Create(l, r());
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator*(std::shared_ptr<AttrOprBase<_TAttrMgr> > l, const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& r)
        {
            return AttrOprMult<_TAttrMgr>::Create(l, r());
        }

        template<typename _TAttrMgr>
        std::shared_ptr<AttrOprBase<_TAttrMgr> > operator/(std::shared_ptr<AttrOprBase<_TAttrMgr> > l, const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>& r)
        {
            return AttrOprDevi<_TAttrMgr>::Create(l, r());
        }
    }

}

#endif
