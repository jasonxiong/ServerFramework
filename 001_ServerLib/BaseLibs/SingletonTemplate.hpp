
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

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
