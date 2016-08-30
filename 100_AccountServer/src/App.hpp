#ifndef __APP_HPP__
#define __APP_HPP__

// 应用程序抽象类
class CApp
{
protected:
    CApp() {}

public:
    virtual ~CApp() {}

    // 相关的初始化，bResume表示是否直接attach到程序上次退出后未删除的共享内存
    // 返回值为0表示成功，其他表示失败
    virtual int Initialize(bool bResume, int iWorldID)
    {
        return 0;
    }

    // 运行，一般为死循环: 接收code->处理->发送处理结果
    virtual void Run() = 0;
};

#endif // __APP_HPP__


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
