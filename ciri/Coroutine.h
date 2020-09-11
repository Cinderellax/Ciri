#ifndef __CIRI_COROUTINE_H__
#define __CIRI_COROUTINE_H__

#include <memory>
#include <functional>
#include <ucontext.h>
#include "Schedule.h"

namespace Ciri{

class Coroutine : public std::enable_shared_from_this<Coroutine>{
public:
    using ptr = std::shared_ptr<Coroutine>;
    using CallBack = std::function<void ()>;
    //协程的状态
    enum STATUS{
        //DEAD,
        READY,
        SUSPEND,
        RUNNING
    };
public:
    Coroutine(CallBack func, Schedule::ptr sch);
    ~Coroutine();
    void addtosch();

    std::function<void ()> getFunc(){return m_func;}

    void setSize(int size){m_size=size;}
    int getSize(){return m_size;}
    void setStatus(int status){m_status=status;}
    ucontext_t& getCtx(){return m_ctx;}
    //启动运行
    void resume(); 
    int getId(){return m_id;}
    void setId(int id){m_id=id;}
    char* getStack(){return m_stack;}
private:
    //需要运行的函数
    CallBack m_func;
    //运行时上下文
    ucontext_t m_ctx;
    //协程调度器
    Schedule::ptr m_sch;

    int m_cap;
    int m_size;
    //当前的状态
    int m_status;
    int m_id;
    char *m_stack;
    //为了使用makecontext而声明的一个静态函数
    static void mainfunc(Schedule* ptr);
};

}

#endif