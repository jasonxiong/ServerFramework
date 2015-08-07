/**
* Copyright (c) 2012, Tencent
* All rights reserved.
*
* @file Singleton.h
* @brief 单件模式基类, 参考 boost::serialization::singleton,去除了Debug锁操作 <br />
*        实例的初始化会在模块载入（如果是动态链接库则是载入动态链接库）时启动 <br />
*        在模块卸载时会自动析构 <br />
*
* Note that this singleton class is thread-safe.
*
* @version 1.0
* @author owentou, owentou@tencent.com
* @date 2012.02.13
*
* @history
*   2012.07.20 为线程安全而改进实现方式
*
*/

#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include "Noncopyable.h"

#include <cstddef>

namespace wrapper
{
    template<class T>
    class SingletonWrapper : public T
    {
    public:
        static bool ms_bDestroyed;
        ~SingletonWrapper()
        {
            ms_bDestroyed = true;
        }
    };

    template<class T>
    bool SingletonWrapper< T >::ms_bDestroyed = false;

}

template <typename T>
class Singleton : public Noncopyable
{
public:
	/**
	 * @brief 自身类型声明
	 */
    typedef T self_type;

protected:

	/**
	 * @brief 虚类，禁止直接构造
	 */
    Singleton() {}

    /**
     * @brief 用于在初始化阶段强制构造单件实例
     */
    static void use(self_type const &) {}

public:
	/**
	 * @brief 获取单件对象引用
	 * @return T& instance
	 */
    static T& GetInstance()
    {
        static wrapper::SingletonWrapper<self_type> stInstance;
        use(stInstance);
        return static_cast<T&>(stInstance);
    }

	/**
	 * @brief 获取单件对象常量引用
	 * @return const T& instance
	 */
    static const T& GetConstInstance()
    {
        return GetInstance();
    }

    /**
     * @brief 获取实例指针 (不推荐使用，仅作向前兼容)
     * @return T* instance
     */
    static self_type* Instance()
    {
        return &GetInstance();
    }

	/**
	 * @brief 判断是否已被析构
	 * @return bool 
	 */
    static bool IsInstanceDestroyed()
    {
        return wrapper::SingletonWrapper<T>::ms_bDestroyed;
    }
};

#endif
