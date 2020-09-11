#include "Coroutine.h"

#include <stdlib.h>
#include <string.h>

#include <iostream>

using namespace std;


#define STACK_SIZE 1024*8

using namespace Ciri;

void Coroutine::mainfunc(Schedule* ptr){
    Schedule* S=ptr;
    //获取当前协程
    Coroutine::ptr co=S->getRunning();
    //运行协程的用户定义函数
	co->getFunc()();
    //设置运行的为-1
    S->setRunning(-1);
    // /
    // *
    // * 
    // * 这块按理说需要加一个从调度器中取出这个运行完毕的协程
    // * 但是考虑到实现协程池就不需要
    // *  
    // * /
    S->removeCoroutine(co->getId());
}

Coroutine::Coroutine(CallBack func, Schedule::ptr sch):
m_func(func),
m_sch(sch),
m_status(READY)
{
    m_stack=(char *)malloc(sizeof(char)*STACK_SIZE);
}

Coroutine::~Coroutine()
{
    free(m_stack);
    m_sch->setRunning(-1);
    //cout<<"xiao hui"<<endl;
}

void Coroutine::addtosch()
{
    m_sch->addCoroutine(shared_from_this());
}

void Coroutine::resume()
{
    switch(m_status){
    //初次启动这个协程的情况
    case READY:
        //将当前的上下文保存在m_ctx中
        getcontext(&m_ctx);
        //设置这个上下文的用户栈为调度器中的栈
        m_ctx.uc_stack.ss_sp = m_sch->getStack();
        m_ctx.uc_stack.ss_size = STACK_SIZE;
        //设置在makecontext结束后运行的上下文为调度器中的main
        m_ctx.uc_link = &(m_sch->getMain()); 
        //设置当前的运行协程为运行这个函数的协程
        m_sch->setRunning(m_id);
        m_status = RUNNING;
        //设置m_ctx上下文为运行mianfunc这个函数
        //如果mainfunc函数返回，则将m_ctx作为后继上下文
        //传入调度器的原始指针给mainfunc函数
        makecontext(&m_ctx, (void (*)(void)) Coroutine::mainfunc, 1, m_sch.get());
        //保存当前上下文到m_sch的main中，然后运行m_ctx这个上下文
        //由于在之前将main这个上下文设置为后继上下文
        //运行完之后还是会运行当前的上下文
        swapcontext(&(m_sch->getMain()), &m_ctx);
        break;
    case SUSPEND:
        //将当前协程的用户栈拷贝到调度器的公有用来运行的栈中
        memcpy(m_sch->getStack()+STACK_SIZE-m_size,m_stack,m_size);
        //设置当前协程为运行
        m_sch->setRunning(m_id);
        m_status = RUNNING;
        //保存当前上下文到main中，运行当前协程中的上下文
        //注意协程中的上下文在运行完mainfunc函数后的后继上下文还是main
        swapcontext(&m_sch->getMain(), &m_ctx);
        break;
    default:
        break;
    }
}
