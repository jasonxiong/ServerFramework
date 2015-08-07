/**
* Copyright (c) 2012, Tecent
* All rights reserved.
*
* @file Noncopyable.h
* @brief 禁止复制基类,继承该类的子类不允许复制
*
*
* @version 1.0
* @author owentou, owentou@tencent.com
* @date 2012.02.21
*
* @history
*
*
*/

#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_

class Noncopyable
{
protected:
    Noncopyable() {}
    ~Noncopyable() {}
private:
    Noncopyable( const Noncopyable& );
    const Noncopyable& operator=( const Noncopyable& );
};

#endif
