#include "Schedule.h"
#include "Coroutine.h"
#include <string.h>

using namespace Ciri;

void Schedule::addCoroutine(std::shared_ptr<Coroutine> co)
{
    co->setId(m_co.size());
    m_co[m_co.size()]=co;
}

//yield必须在用户定义的函数中使用，否则会出现问题
//原因是运行到这里取用户栈的时候，上下文使用的用户栈必须为m_stack.
//即为之前在初始运行协程时设置的
void Schedule:: yield()
{
    char dummy = 0;
    //获得当前的协程
    std::shared_ptr<Coroutine> co = getRunning();
    //这个是当前上下文中用户栈的栈底
    char* top = m_stack+STACK_SIZE;
    co->setSize(top - &dummy);
    //当前用户栈的数据存放于当前协程中的私有栈中
    memcpy(co->getStack(), &dummy, co->getSize());
    //设置状态为停止
    co->setStatus(Coroutine::SUSPEND);
    //设置未有运行的协程
    setRunning(-1);
    //将当前的上下文保存到调用这个函数的协程的上下文变量中
    //运行main上下文
    swapcontext(&co->getCtx() , &m_main); 
}