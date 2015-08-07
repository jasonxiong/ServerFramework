/**
*@file SingletonTemplate.hpp
*@author MtGame.Server
*@date 2006.05.10
*@version 1.0
*@brief 单件类定义
*
*	该单件类的实现是将单件类的实例定义为static，在分配具体类时是用动态分配的方式。
*	需要注意有两个缺点：
*	（1）不适合于在多线程的情况下创建单件对象
*	（2）不适合于需要在程序结束的时候调用具体类的析构函数
*/

#ifndef __SINGLETON_TEMPLATE_HPP__
#define __SINGLETON_TEMPLATE_HPP__

//在这添加标准库头文件
#include <stdio.h>
#include <stdlib.h>

namespace ServerLib
{

template <class TYPE>
class CSingleton
{
public:
    static TYPE* Instance(void)
    {
        if(m_pSingleton == NULL)
        {
            m_pSingleton = new CSingleton;
        }
        return &m_pSingleton->m_stInstance;
    }
protected:
    CSingleton()
    {

    }
protected:
    TYPE m_stInstance;
    static CSingleton<TYPE>* m_pSingleton;

};

template <class TYPE>
CSingleton<TYPE>* CSingleton<TYPE>::m_pSingleton = NULL;

}

#endif /* __SINGLETON_TEMPLATE_HPP__ */
///:~
